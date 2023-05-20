#include "../include/Window.h"
#include "../include/FrameBuffer.h"
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
int numTextures = 10;
int numTriangles = 3;
int fbDescriptor[3] = {400, 400, 3};//Descriptor for the FB, [0] = WIDTH, [1] = HEIGHT, [2] = #colours per pixel

//Buffers
Texture* textures;//list of textures
Triangle* triangles;//list of triangles
unsigned char* triangleHitBuffer;//what triangle the ray intersected
Vec3* triangleHitPointBuffer;//where on the triangle we hit

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

  //Gen the window
  Window* window = createWindow(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], "Raytracer", fullscreen, primMonitor, NULL);

  //init the triangle hit buffer and distance buffer
  triangleHitBuffer = malloc(sizeof(unsigned char) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  triangleHitPointBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  textures = malloc(sizeof(Texture) * numTextures);

  Vec3 red = {255,128,128};
  Vec3 green = {128,255,128};
  Vec3 blue = {128,128,255};
  Vec3 grey = {100,100,100};
  Vec3 white = {255,255,255};
  Vec3 dark = {20,20,20};
  Vec3 norm = {1,1,1};

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
      Vec3 v0 = {0, 0, 100};
      Vec3 v1 = {400, 0, 100};
      Vec3 v2 = {400, 400, 100};
      verts[0] = vertexGen(v0, norm, red, (Vec2){0, 0});
      verts[1] = vertexGen(v1, norm, green, (Vec2){textures[0].width, 0});
      verts[2] = vertexGen(v2, norm, blue, (Vec2){textures[0].width, textures[0].height});
      triangles[0] = triangleGen(verts, (Vec3){0, 0, 0}, &textures[0]);


      verts[0] = vertexGen((Vec3){0, 0, 50}, norm, green, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){0, 400, 50}, norm, red, (Vec2){0, textures[1].height});
      verts[2] = vertexGen((Vec3){400, 400, 50}, norm, blue, (Vec2){textures[1].width, textures[1].height});
      triangles[1] = triangleGen(verts, (Vec3){0, 0, 0}, &textures[1]);

      verts[0] = vertexGen((Vec3){100, 50, 5}, norm, blue, (Vec2){150-25, 100});
      verts[1] = vertexGen((Vec3){300, 50, 5}, norm, red, (Vec2){225, 100});
      verts[2] = vertexGen((Vec3){200, 323, 25}, norm, green, (Vec2){175,150});
      triangles[2] = triangleGen(verts, (Vec3){0, 0, 0}, NULL);
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
  FrameBuffer fb = createFB(fbDescriptor);

  printf("Running OpenGL %s\n", glGetString(GL_VERSION));

  //getting the inverse of the height and width -1 outside the loop so we can perform multiplies with the invers (= to / of normal. multiplies are cheaper than division and this is happening lots
  float invHeightMinus1 = 1.0f / (fbDescriptor[HEIGHT] - 1.0f);
  float invWidthMinus1 = 1.0f / (fbDescriptor[WIDTH] - 1.0f);
  while(!glfwWindowShouldClose(window->window))
    {
      clock_t start = clock(), diff;
      ///Trace some rays and see what we hit
      //Clear the pixel's triangle hit buffer
      memset(triangleHitBuffer, 0xFF, fbDescriptor[WIDTH] * fbDescriptor[HEIGHT] * sizeof(unsigned char));
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
		  if(triangleIntersect(triangles[tID].verts[0].position, triangles[tID].verts[1].position, triangles[tID].verts[2].position, &ray, intersectionPoint))
		    {
		      if(ray.distance < distance)
			{//we hit a triangle thats closer to whatever we have hit before; if anything
			  distance = ray.distance;
			  triangleHitBuffer[((j * fbDescriptor[WIDTH]) + i)] = tID;//mark the hit buffer for this pixel with the tri that we hit
			  memcpy(triangleHitPointBuffer[((j * fbDescriptor[WIDTH]) + i)], intersectionPoint, 3 * sizeof(float));
			}
		    }
		}
	    }
	}

      ///Shading
      //now we know what we hit, colour it to the fb
      for(int j = 0; j <fbDescriptor[HEIGHT]; j++)
	{
	  for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	    {
	      //Read from the buffer what triangle the ray intersected
	      unsigned char triangleID = triangleHitBuffer[((j * fbDescriptor[WIDTH]) + i)];
	      if(triangleID == 0xFF){//if we didn't hit a triangle, colour the pixel "dark"
		fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = dark[0];
		fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = dark[1];
		fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = dark[2];
	      }
	      else {//else lets colour the pixel whatever the triangle's colour is at this postiion
		//read back from the buffer where we hit the triangle
		Vec3 hitpoint;
		memcpy(hitpoint,  triangleHitPointBuffer[((j * fbDescriptor[WIDTH]) + i)], 3 * sizeof(float));

		//just unpack things to be more readable and get at the vertices
		Vertex* vert0 = &triangles[triangleID].verts[0]; Vertex* vert1 = &triangles[triangleID].verts[1]; Vertex* vert2 = &triangles[triangleID].verts[2];

		//calculate barycentric coords in the triangle
		float wv[3];
		barycentricCoords(wv, &triangles[triangleID], hitpoint);

		///Colouring the triangle
		//if the triangle isn't textured; use the barycentric coords to weight/interpolate the colours of the verts.
		if(triangles[triangleID].texture == NULL) {
		  //write the colours to the framebuffer
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = (wv[0] * (float)vert0->colour[0]) + (wv[1] * (float)vert1->colour[0]) + (wv[2] * (float)vert2->colour[0]);
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = (wv[0] * (float)vert0->colour[1]) + (wv[1] * (float)vert1->colour[1]) + (wv[2] * (float)vert2->colour[1]);
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = (wv[0] * (float)vert0->colour[2]) + (wv[1] * (float)vert1->colour[2]) + (wv[2] * (float)vert2->colour[2]);
		}
		else {//use the barycentric coords to interpolate the texture coords
		  Vec2 texCords;
		  int u = (int)(wv[0] * vert0->texCords[0]) + (wv[1] * vert1->texCords[0]) + (wv[2] * vert2->texCords[0]);
		  int v = (int)(wv[0] * vert0->texCords[1]) + (wv[1] * vert1->texCords[1]) + (wv[2] * vert2->texCords[1]);
		  //sample the texture at the point of intersection
		  unsigned char* image = triangles[triangleID].texture->image;
		  //copy the sample to the framebuffer
		  memcpy(
			 &fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL]],
			 &image[((v * triangles[triangleID].texture->width) + u) * 3],//The 3's here are the colours per pixel
			 3 * sizeof(unsigned char));
		}
	      }
	    }
	}


      glClear(GL_COLOR_BUFFER_BIT);
      glDrawPixels(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], GL_RGB, GL_UNSIGNED_BYTE, fb);

      //swap buffers
      glfwSwapBuffers(window->window);

      //poll and process events
      glfwPollEvents();
      diff = clock() - start;
      int msec = diff * 1000 /CLOCKS_PER_SEC;
      printf("Frametime: %d\n", msec%1000);
    }


  freeFB(fb);
  freeWindow(window);
  free(triangles);
  free(triangleHitBuffer);
  glfwTerminate();

  return 0;
}
