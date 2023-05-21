#include "../include/Window.h"
#include "../include/FrameBuffer.h"
#include "../include/Raytracer.h"
#include "../include/Triangle.h"
#include "../include/BvhNode.h"
#include "../include/Box.h"
#include "../include/Ray.h"
#include "../include/Texture.h"
#include "../include/Camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

//some flags for the window
int fullscreen = 0;
int stereoscopic = 0;//creates a segfault
int doublebuffer = 1;

//Buffer sizes - how many entries we will allocate
unsigned int numTextures = 10;
unsigned int numTriangles = 1;
FbDescriptor fbDescriptor = {800, 800, 3};//Descriptor for the FB, [0] = WIDTH, [1] = HEIGHT, [2] = #colours per pixel

//how many are allocated
//Buffers
Texture* textures;//list of textures
Triangle* triangles;//list of triangles
RayHitBuffer rayHitBuffer;//what triangle the ray intersected
RayHitpointBuffer rayHitpointBuffer;//where on the triangle we hit
Vertex* verices;

int main()
{
  //Init glfw and setup monitor/window
  int initCode = glfwInit();
  if(!initCode)
    return -1;
  GLFWmonitor* primMonitor = glfwGetPrimaryMonitor();
  if(!primMonitor)
    return -1;

  if(stereoscopic)
    windowHint(GLFW_STEREO, GLFW_TRUE);
  if(doublebuffer)
    windowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

  printf("FB: %d, %d, %d\n", fbDescriptor[0], fbDescriptor[1], fbDescriptor[2]);
  //Gen the window
  //TODO: Change the params out to just pass a ptr to the fbDescriptor
  Window* window = createWindow(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], "Raytracer", fullscreen, primMonitor, NULL);

  //init the triangle hit buffer and distance buffer
  rayHitBuffer = malloc(sizeof(RayHitBuffer) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  rayHitpointBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  textures = malloc(sizeof(Texture) * numTextures);

  Vec3 red = {255,128,128};
  Vec3 green = {128,255,128};
  Vec3 blue = {128,128,255};
  Vec3 grey = {100,100,100};
  Vec3 white = {255,255,255};
  Vec3 dark = {20,20,20};

  /* //Matrix operation example
  Mat3 m3_1 = {{1,0,0}, {2,1,0}, {0,0,3}};
  Mat3 m3_2 = {{5,0,1}, {0,1,0}, {1,7,0}};
  Mat3 m3_3;
  mat3Mul(m3_1, m3_2, m3_3);
  printMat3(m3_3);

  Mat4 m4_1 = {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16}};
  Mat4 m4_2 = {{20,21,22,23}, {24,25,26,27}, {28,29,30,31}, {32,33,34,35}};
  Mat4 m4_3;
  mat4Mul(m4_1, m4_2, m4_3);
  printMat4(m4_3);
  */

  Vec3 normal = {1,1,1};
  //Generate the vertices list
  const unsigned int vLen = 20;//num verts
  Vertices vertices = verticesGen(vLen);
  if(vertices.vertices != NULL)
    printf("Vertices buffer generated\n");
  else{
    printf("Vertices buffer failed to gen\n");
    return -1;}


  verticesAddVert(&vertices, vertexGen((Vec3){-400, 0, 0}, normal, red, (Vec2){-600, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){400, 0, 0}, normal, green, (Vec2){400, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){0, 400, 0}, normal, blue, (Vec2){0, 800}));

  //load some textures
  if(!textures)
    {
      printf("Error initialising texture list\n");
      return -1;
    }
  int numChannels = 3;
  textures[0] = loadTexture("res/scrot.png", numChannels);
  textures[1] = loadTexture("res/tex1.jpg", numChannels);


  //init triangles
  triangles = malloc(numTriangles * sizeof(Triangle));

  Vertex verts[3];
  if(triangles)
    {
      //verts[0] = vertexGen(v0, norm, red, (Vec2){-640, 0});
      //verts[1] = vertexGen(v1, norm, green, (Vec2){textures[0].width/2, 0});
      //verts[2] = vertexGen(v2, norm, blue, (Vec2){0, textures[0].height});
      verts[0] = *verticesGetVert(&vertices, 0);
      verts[1] = *verticesGetVert(&vertices, 1);
      verts[2] = *verticesGetVert(&vertices, 2);

      triangles[0] = triangleGen(verts, (Vec3){fbDescriptor[WIDTH]/2, fbDescriptor[HEIGHT]/2, 100}, &textures[0]);
      /*
      verts[0] = vertexGen((Vec3){-50, 0, 0}, norm, green, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){50, 0, 0}, norm, red, (Vec2){textures[1].width, 0});
      verts[2] = vertexGen((Vec3){0, 50, 0}, norm, blue, (Vec2){textures[1].width/2, textures[1].height});
      triangles[1] = triangleGen(verts, (Vec3){100, 100, 5}, &textures[1]);


      verts[0] = vertexGen((Vec3){100, 50, 5}, norm, blue, (Vec2){150-25, 100});
      verts[1] = vertexGen((Vec3){300, 100, 5}, norm, red, (Vec2){225, 100});
      verts[2] = vertexGen((Vec3){200, 125, 25}, norm, green, (Vec2){175,150});
      triangles[2] = triangleGen(verts, (Vec3){0, 0, 0}, NULL);
      */

    }
  else
    return -1;
  /*
  //Setup the bvh node
  BBox* rootBox = genBox((Vec3){0,0,0},(Vec3){10,10,10});
  BvhNode* rootNode = bvhNodeGen(8, 0, *rootBox);
  if(rootNode)
    printf("BVH root inited\n");
  */




  Camera camera = cameraGen((Vec3){0,0,0}, (Vec3){0,0,100}, (Vec3){0,1,0}, 30,
			    fbDescriptor[WIDTH]/fbDescriptor[HEIGHT]);

  //setup framebuffer
  FrameBuffer frameBuffer = createFB(fbDescriptor);

  printf("Running OpenGL %s\n", glGetString(GL_VERSION));

  //getting the inverse of the height and width -1 outside the loop so we can perform multiplies with the invers (= to / of normal. multiplies are cheaper than division and this is happening lots
  float invHeightMinus1 = 1.0f / (fbDescriptor[HEIGHT] - 1.0f);
  float invWidthMinus1 = 1.0f / (fbDescriptor[WIDTH] - 1.0f);
  while(!glfwWindowShouldClose(window->window))
    {
      clock_t start = clock(), diff;
      ///Trace some rays and see what we hit
      //Clear the pixel's triangle hit buffer
      memset(rayHitBuffer, 0xFF, fbDescriptor[WIDTH] * fbDescriptor[HEIGHT] * sizeof(unsigned char));

      printf("hello\n");
      traceRays(triangles, numTriangles, frameBuffer, &camera, rayHitBuffer,
		rayHitpointBuffer, fbDescriptor, invHeightMinus1, invWidthMinus1);
      printf("Goodbye\n");

      /*
      for(int j = 0; j < fbDescriptor[HEIGHT]; j++)
	{
	  float jj = (2.0f * j) * invHeightMinus1;//normalised screen cord in the vertical
	  Vec3 ja;
	  vec3ScalarMult(camera.up, camera.dimensions[1], ja);
	  Vec3 jb;
	  vec3ScalarMult(ja, jj, jb);
	  int i;
	  //	  printf("camera up * cam height: ");
	  //	  printVec3(ja);

	  for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	    {
	      float distance = 0xFFFF;

	      float ii = (2.0f * i) * invWidthMinus1;//normalised screen cord in the horisontal
	      Vec3 ia;
	      vec3ScalarMult(camera.right, camera.dimensions[0], ia);
	      Vec3 ib;
	      vec3ScalarMult(ia, ii, ib);


	      Vec3 ija;
	      vec3Add(ib, jb, ija);
	      Vec3 ijza;
	      vec3Add(camera.forwards, ija, ijza);
	      vec3Normalise(ijza, ijza);

	      Ray ray = rayInit((Vec3){i,j,camera.origin[2]}, ijza, 10000);
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
			  memcpy(rayHitPointBuffer[((j * fbDescriptor[WIDTH]) + i)], intersectionPoint, 3 * sizeof(float));
			}
		    }
		}
	    }
	}
      */

      ///Shading
      //now we know what we hit, colour it to the fb
      for(int j = 0; j < fbDescriptor[HEIGHT]; j++)
	{
	  for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	    {
	      //Read from the buffer what triangle the ray intersected
	      unsigned char triangleID = rayHitBuffer[((j * fbDescriptor[WIDTH]) + i)];
	      if(triangleID == 0xFF){//if we didn't hit a triangle, colour the pixel "dark"
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
		barycentricCoords(wv, vert0.position, vert1.position, vert2.position, hitpoint);
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


      glClear(GL_COLOR_BUFFER_BIT);
      glDrawPixels(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);

      //swap buffers
      glfwSwapBuffers(window->window);

      //poll and process events
      glfwPollEvents();
      diff = clock() - start;
      int msec = diff * 1000 /CLOCKS_PER_SEC;
      //      printf("Frametime: %d\n", msec%1000);
    }


  freeFB(frameBuffer);
  freeWindow(window);
  free(triangles);
  free(rayHitBuffer);
  glfwTerminate();

  return 0;
}
