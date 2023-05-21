#include "../include/Raytracer.h"

//TODO: Replace the Triangle list with a BvhNode
void traceRays(Triangle* triangles, unsigned int numTriangles, Camera* camera, RayHitBuffer rayHitBuffer, RayHitpointBuffer rayHitPointBuffer, FbDescriptor fbDescriptor, float invHeightMinus1, float invWidthMinus1){

  for(int j = 0; j < fbDescriptor[HEIGHT]; j++)
    {
      float jj = (2.0f * j) * invHeightMinus1;//normalised screen cord in the vertical
      Vec3 ja;
      vec3ScalarMult(camera->up, camera->dimensions[1], ja);
      Vec3 jb;
      vec3ScalarMult(ja, jj, jb);
      int i;
      //	  printf("camera up * cam height: ");
      //	  printVec3(ja);

      for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	{
	  //	  printf("I,J: %d,%d\n",i, j);
	  float distance = 0xFFFF;

	  float ii = (2.0f * i) * invWidthMinus1;//normalised screen cord in the horisontal
	  Vec3 ia;
	  vec3ScalarMult(camera->right, camera->dimensions[0], ia);
	  Vec3 ib;
	  vec3ScalarMult(ia, ii, ib);

	  Vec3 ija;
	  vec3Add(ib, jb, ija);
	  Vec3 ijza;
	  vec3Add(camera->forwards, ija, ijza);
	  vec3Normalise(ijza, ijza);
	  Ray ray = rayInit((Vec3){i,j,camera->origin[2]}, ijza, 10000);
	  for(unsigned short tID = 0; tID < numTriangles; tID++)//go through the triangles
	    {
	      //trace some rays
	      Vec3 intersectionPoint;//This is the place in space where the ray intersects with the triangle
#ifdef RELATIVE_VERTS
	      Vec3 tVert0Pos, tVert1Pos, tVert2Pos;
	      vec3Add(triangles[tID].verts[0].position, triangles[tID].pos, tVert0Pos);
	      vec3Add(triangles[tID].verts[1].position, triangles[tID].pos, tVert1Pos);
	      vec3Add(triangles[tID].verts[2].position, triangles[tID].pos, tVert2Pos);
	      if(triangleIntersect(tVert0Pos, tVert1Pos, tVert2Pos, &ray, intersectionPoint))
#else
		if(triangleIntersect(triangles[tID].verts[0].position, triangles[tID].verts[1].position, triangles[tID].verts[2].position, &ray, intersectionPoint))
#endif
		  {

		    if(ray.distance < distance)
		      {//we hit a triangle thats closer to whatever we have hit before; if anything

			distance = ray.distance;
			rayHitBuffer[((j * fbDescriptor[WIDTH]) + i)] = tID;//mark the hit buffer for this pixel with the tri that we hit

			memcpy(rayHitPointBuffer[((j * fbDescriptor[WIDTH]) + i)], intersectionPoint, sizeof(Vec3));
		      }
		  }
	    }
	}
    }
}


inline void shading(Triangle* triangles, FrameBuffer frameBuffer,  RayHitBuffer rayHitBuffer, RayHitpointBuffer rayHitpointBuffer, FbDescriptor fbDescriptor)
{

  //now we know what we hit, colour it to the fb
  for(int j = 0; j < fbDescriptor[HEIGHT]; j++)
    {
      for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	{
	  //Read from the buffer what triangle the ray intersected
	  unsigned short triangleID = rayHitBuffer[((j * fbDescriptor[WIDTH]) + i)];
	  if(triangleID == 0xFFFF){//if we didn't hit a triangle, colour the pixel "dark"
	    Vec3 dark = {80,80,80};//TODO: Pass in the colours through a colour array
	    frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = dark[0];
	    frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = dark[1];
	    frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = dark[2];
	  }
	  else {//else lets colour the pixel whatever the triangle's colour is at this postiion
		//read back from the buffer where we hit the triangle
	    Vec3 hitpoint;
	    memcpy(hitpoint,  rayHitpointBuffer[((j * fbDescriptor[WIDTH]) + i)], 3 * sizeof(float));

	    //just unpack things to be more readable and get at the vertices
	    Vertex* vert0 = &triangles[triangleID].verts[0]; Vertex* vert1 = &triangles[triangleID].verts[1]; Vertex* vert2 = &triangles[triangleID].verts[2];
	    float wv[3];//barrycentric cords
#ifdef RELATIVE_VERTS
	    Vec3 tVert0Pos, tVert1Pos, tVert2Pos;
	    vec3Add(vert0->position, triangles[triangleID].pos, tVert0Pos);
	    vec3Add(vert1->position, triangles[triangleID].pos, tVert1Pos);
	    vec3Add(vert2->position, triangles[triangleID].pos, tVert2Pos);
	    barycentricCoords(wv, tVert0Pos, tVert1Pos, tVert2Pos, hitpoint);//calculate barycentric coords in the triangle
#else
	    barycentricCoords(wv, vert0->position, vert1->position, vert2->position, hitpoint);
#endif
	    ///Colouring the triangle
	    //if the triangle isn't textured; use the barycentric coords to weight/interpolate the colours of the verts.
	    if(triangles[triangleID].texture == NULL) {
	      //write the colours to the framebuffer
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = (wv[0] * (float)vert0->colour[0]) + (wv[1] * (float)vert1->colour[0]) + (wv[2] * (float)vert2->colour[0]);
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = (wv[0] * (float)vert0->colour[1]) + (wv[1] * (float)vert1->colour[1]) + (wv[2] * (float)vert2->colour[1]);
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = (wv[0] * (float)vert0->colour[2]) + (wv[1] * (float)vert1->colour[2]) + (wv[2] * (float)vert2->colour[2]);
	    }
	    else {//use the barycentric coords to interpolate the texture coords
	      Vec2 texCords;
	      int u = (int)(wv[0] * vert0->texCords[0]) + (wv[1] * vert1->texCords[0]) + (wv[2] * vert2->texCords[0]);
	      int v = (int)(wv[0] * vert0->texCords[1]) + (wv[1] * vert1->texCords[1]) + (wv[2] * vert2->texCords[1]);
	      //sample the texture at the point of intersection
	      unsigned char* image = triangles[triangleID].texture->image;
	      //copy the sample to the framebuffer
	      //see if the text coord is wrapping past the end of the texture and if so we will modulo with the size
	      unsigned int texSize = triangles[triangleID].texture->sizeBytes;
	      unsigned char texChannels = triangles[triangleID].texture->channels;
	      unsigned int texSampleAddr = ((v * triangles[triangleID].texture->width) + u) * texChannels;

	      if(texSampleAddr > texSize)
		texSampleAddr = texSampleAddr % texSize;//if were running past the end of the tex, do texture wrapping

	      memcpy(
		     &frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL]],
		     &image[texSampleAddr],//The 3's here are the colours per pixel
		     texChannels * sizeof(unsigned char));
	    }
	  }
	}
    }

}
