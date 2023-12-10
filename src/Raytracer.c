#include "../include/Raytracer.h"
void traceRays(BvhNode* bvhNode, Camera* camera, RayHitBuffer rayHitBuffer, RayHitpointBuffer rayHitpointBuffer, RayHitNormalBuffer rayHitnormalBuffer, RayHitDirectionBuffer rayHitDirectionBuffer, FbDescriptor fbDescriptor, float invHeightMinus1, float invWidthMinus1){
	long pxCount = 0;
	
  	for(int j = 0; j < fbDescriptor[HEIGHT]; j++)
	{
		float jj = (2.0f * j) * invHeightMinus1;//normalised screen cord in the vertical
		Vec3 ja;
		vec3ScalarMult(camera->up, camera->dimensions[1], ja);
		Vec3 jb;
		vec3ScalarMult(ja, jj, jb);

		for(int i = 0; i < fbDescriptor[WIDTH]; i++)
		{			
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
			float distance = 500000;//distance limit something big. Nothing beond here will be rendered
			//Ray ray = rayInit((Vec3){i,j,camera->origin[2]}, ijza, distance);
			Ray ray = rayInit((Vec3){i,j,0}, (Vec3){0,0,1}, distance);
			//Test the ray against the BvhNode
			Vec3 intersectionPoint;//This is the place in space where the ray intersects with the triangle
			
			T* hitTriangle = NULL;
			hitTriangle = testBVH(&ray, bvhNode, intersectionPoint);
			
			
			if(hitTriangle){ 
				//ray hit something inside the Bvh.

				rayHitBuffer[((j * fbDescriptor[WIDTH]) + i)] = hitTriangle;
				memcpy(rayHitpointBuffer[((j * fbDescriptor[WIDTH]) + i)], intersectionPoint, sizeof(Vec3));
				memcpy(rayHitnormalBuffer[((j * fbDescriptor[WIDTH]) + i)], &hitTriangle->vertNormal, sizeof(Vec3));
				memcpy(rayHitDirectionBuffer[((j * fbDescriptor[WIDTH]) + i)], ray.direction, sizeof(Vec3));
			}
			pxCount++;
		}
	}
}

inline void traceSecondaryRays(BvhNode* rootNode, RayHitBuffer rayHitBuffer, RayHitpointBuffer rayHitpointBuffer, RayHitNormalBuffer rayHitnormalBuffer, RayHitDirectionBuffer rayHitDirectionBuffer, FbDescriptor fbDescriptor)
{
	for(int i = 0; i < fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]; i++)
	{
		//if we didn't hit anything then just return
		if(rayHitBuffer[i] == NULL)
	    	continue;
			

		//Now we know we hit something, copy the details out of the buffer
		Vec3 point;
		Vec3 normal;
		Vec3 direction;
		float distance = 50000;

		memcpy(point,  rayHitpointBuffer[i], sizeof(Vec3));
		memcpy(normal,  rayHitnormalBuffer[i], sizeof(Vec3));
		memcpy(direction,  rayHitDirectionBuffer[i], sizeof(Vec3));

		//TODO:
		//Adjust the ray direction to be about the normal
		//Move the ray just above the surfice

		//trace the rays
		Ray ray = rayInit(point, direction, distance);
		//Test the ray against the BvhNode
		Vec3 intersectionPoint;//This is the place in space where the ray intersects with the triangle
			
		T* hitTriangle = NULL;
		hitTriangle = testBVH(&ray, rootNode, intersectionPoint);
	}
}


inline void shading(FrameBuffer frameBuffer,  RayHitBuffer rayHitBuffer, RayHitpointBuffer rayHitpointBuffer, RayHitNormalBuffer rayHitnormalBuffer, RayHitDirectionBuffer rayHitDirectionBuffer, FbDescriptor fbDescriptor)
{

  //now we know what we hit, colour it to the fb
  for(int j = 0; j < fbDescriptor[HEIGHT]; j++)
    {
      for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	{

	  //Read from the buffer what triangle the ray intersected
	  //	  unsigned short triangleID = rayHitBuffer[((j * fbDescriptor[WIDTH]) + i)];
	  T* triangle = NULL;
	  if(rayHitBuffer[((j * fbDescriptor[WIDTH]) + i)])
	    triangle = rayHitBuffer[((j * fbDescriptor[WIDTH]) + i)];

	  if(!triangle){//if we didn't hit a triangle, colour the pixel "dark"
	    Vec3 dark = {40,40,40};//TODO: Pass in the colours through a colour array
	    frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = dark[0];
	    frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = dark[1];
	    frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = dark[2];
	  }
	  else {//else lets colour the pixel whatever the triangle's colour is at this postiion
		//read back from the buffer where we hit the triangle


	    Vec3 hitpoint;
		Vec3 hitNormal;
		Vec3 hitDirection;




	    memcpy(hitpoint,  rayHitpointBuffer[((j * fbDescriptor[WIDTH]) + i)], 3 * sizeof(float));
		memcpy(hitNormal,  rayHitnormalBuffer[((j * fbDescriptor[WIDTH]) + i)], 3 * sizeof(float));
		memcpy(hitDirection,  rayHitDirectionBuffer[((j * fbDescriptor[WIDTH]) + i)], 3 * sizeof(float));

	    //just unpack things to be more readable and get at the vertices
	    //Vertex* vert0 = &triangle->verts[0]; Vertex* vert1 = &triangle->verts[1]; Vertex* vert2 = &triangle->verts[2];
	    Vec3 wv;//barrycentric cords

#ifdef RELATIVE_VERTS    

#ifndef RUNTIME_VERT_TRANSFORM
		///Use when tranformed pos is generated at update time
		/////////////////
		///Do the transform at triangle gen time
	    barycentricCoords(wv, triangle->vertPosition[0], triangle->vertPosition[1], triangle->vertPosition[2], hitpoint);//calculate barycentric coords in the triangle

		///Use when the transform is done at render time
#else
	    Vec3 tVert0Pos, tVert1Pos, tVert2Pos;
	    vec3Add(vert0->position, triangle->position, tVert0Pos);
	    vec3Add(vert1->position, triangle->position, tVert1Pos);
	    vec3Add(vert2->position, triangle->position, tVert2Pos);
		barycentricCoords(wv, tVert0Pos, tVert1Pos, tVert2Pos, hitpoint);//calculate barycentric coords in the triangle
#endif
#else
	    barycentricCoords(wv, vert0->position, vert1->position, vert2->position, hitpoint);
#endif
	    ///Colouring the triangle
	    //if the triangle isn't textured; use the barycentric coords to weight/interpolate the colours of the verts.
		/*
	    if(triangle->texture == NULL) {
	      //write the colours to the framebuffer
		  unsigned char redPx = ((wv[0] * vert0->colour[0]) + (wv[1] * vert1->colour[0]) + (wv[2] * vert2->colour[0]));
		  unsigned char greenPx = ((wv[0] * vert0->colour[1]) + (wv[1] * vert1->colour[1]) + (wv[2] * vert2->colour[1]));
		  unsigned char bluePx = ((wv[0] * vert0->colour[2]) + (wv[1] * vert1->colour[2]) + (wv[2] * vert2->colour[2]));
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = redPx;
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = greenPx;
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = bluePx;
	    }
	    else
		{
			///use the barycentric coords to interpolate the texture coords
			//get denormalised tex coords
			int texWidth = triangle->texture->width;
			int texHeight = triangle->texture->height;
			int u = (int)((wv[0] * vert0->texCords[0] * texWidth) + (wv[1] * vert1->texCords[0] * texWidth) + (wv[2] * vert2->texCords[0] * texWidth));
			int v = (int)((wv[0] * vert0->texCords[1] * texHeight) + (wv[1] * vert1->texCords[1] * texHeight) + (wv[2] * vert2->texCords[1] * texHeight));
			//sample the texture at the point of intersection
			unsigned char* image = triangle->texture->image;
			//copy the sample to the framebuffer
			//see if the text coord is wrapping past the end of the texture and if so we will modulo with the size
			unsigned int texSize = triangle->texture->sizeBytes;
			unsigned char texChannels = triangle->texture->channels;
			unsigned int texSampleAddr = ((v * triangle->texture->width) + u) * texChannels;

			if(texSampleAddr > texSize)
				texSampleAddr = texSampleAddr % texSize;//if were running past the end of the tex, do texture wrapping

			memcpy(
			&frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL]],
			&image[texSampleAddr],//The 3's here are the colours per pixel
			texChannels * sizeof(unsigned char));
	    }
		*/
		unsigned char redPx = 128;
		  unsigned char greenPx = 128;
		  unsigned char bluePx = 128;
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = redPx;
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = greenPx;
	      frameBuffer[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = bluePx;
	  }
	}
    }

}
