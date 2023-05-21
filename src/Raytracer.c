#include "../include/Raytracer.h"

//TODO: Replace the Triangle list with a BvhNode
void traceRays(Triangle* triangles, unsigned int numTriangles, FrameBuffer frameBuffer, Camera* camera, RayHitBuffer rayHitBuffer, RayHitpointBuffer rayHitPointBuffer, FbDescriptor fbDescriptor, float invHeightMinus1, float invWidthMinus1){

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
	  for(int tID = 0; tID < numTriangles; tID++)//go through the triangles
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
