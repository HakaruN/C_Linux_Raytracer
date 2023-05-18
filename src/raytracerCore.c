#include "../include/Window.h"
#include "../include/FrameBuffer.h"
#include "../include/Triangle.h"
#include "../include/BvhNode.h"
#include "../include/Box.h"
#include "../include/Ray.h"
#include "../include/Texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int fullscreen = 0;
int stereoscopic = 0;//creates a segfault
int doublebuffer = 1;

//Descriptor for the FB, [0] = width, [1] = height, [2] = #colours per pixel
int fbDescriptor[3] = {400, 400, 3};
unsigned char* triangleHitBuffer;
//float* triangleDistBuffer;
Vec3* triangleHitPointBuffer;

unsigned char grey[3] = {100, 100, 100};
Triangle* triangles;
unsigned short numTriangles = 3;

//void preRender(unsigned char* triangleHits, float* triangleDists, vec3* intersectPoints);
void preRender(unsigned char* triangleHits,  Vec3* intersectPoints);

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

  unsigned char* tex1 = loadTexture("res/scrot.png");
  if(tex1)
    printf("Texture loaded\n");
  else
    printf("Texture failed to load\n");


  Window* window = createWindow(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], "Raytracer", fullscreen, primMonitor, NULL);


  //init the triangle hit buffer and distance buffer
  triangleHitBuffer = malloc(sizeof(unsigned char) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  //  triangleDistBuffer = malloc(sizeof(float) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  triangleHitPointBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);


  /*
  printf("Vector sizes:\n");
  printf("vec unsigned char: %d\n", sizeof(vector unsigned char));
  printf("vec signed char: %d\n", sizeof(vector signed char));
  printf("vec bool char: %d\n", sizeof(vector bool char));
  printf("vec unsigned short: %d\n", sizeof(vector unsigned short));
  printf("vec signed short: %d\n", sizeof(vector signed short));
  printf("vec bool short: %d\n", sizeof(vector bool short));
  printf("vec pixel: %d\n", sizeof(vector pixel));
  printf("vec unsigned int: %d\n", sizeof(vector unsigned int));
  printf("vec signed int: %d\n", sizeof(vector signed int));
  printf("vec bool int: %d\n", sizeof(vector bool int));
  printf("vec float: %d\n", sizeof(vector float));
  */


  /*Vector example
  vector float a = (vector float){10, 10, 15, 0};
  vector float b = (vector float){1, 1, 1, 1};
  vector float c = vec_add(a,b);
  printf("%f,%f,%f,%f\n",c[0],c[1],c[2],c[3]);
  */

  //init triangles
  triangles = malloc(numTriangles * sizeof(Triangle));
  Vertex verts[3];

  Vec3 red = {255,128,128};
  Vec3 green = {128,255,128};
  Vec3 blue = {128,128,255};
  Vec3 grey = {100,100,100};
  Vec3 white = {255,255,255};
  Vec3 dark = {20,20,20};
  Vec3 norm = {1,1,1}; Vec2 uv = {0,0};
  if(triangles)
    {

      verts[0] = vertexGen((Vec3){0,0,10}, norm, red, uv);
      verts[1] = vertexGen((Vec3){50, 0, 10}, norm, green, uv);
      verts[2] = vertexGen((Vec3){25, 50, 10}, norm, blue, uv);
      triangles[0] = triangleGen(verts, (Vec3){0, 0, 0});

      verts[0] = vertexGen((Vec3){100, 100, 10}, norm, green, uv);
      verts[1] = vertexGen((Vec3){150, 100, 10}, norm, red, uv);
      verts[2] = vertexGen((Vec3){125, 150, 10}, norm, blue, uv);
      triangles[1] = triangleGen(verts, (Vec3){0, 0, 0});

      verts[0] = vertexGen((Vec3){150-25, 100, 5}, norm, blue, uv);
      verts[1] = vertexGen((Vec3){250-25, 100, 5}, norm, red, uv);
      verts[2] = vertexGen((Vec3){200-25, 150, 5}, norm, green, uv);
      triangles[2] = triangleGen(verts, (Vec3){0, 0, 0});
    }
  else
    return -1;


  for(int i = 0; i < numTriangles; i++)
    triangles[i].tex = tex1;

  vector float a = {10, 10, 10, 10};
  vector float b = {10, 10, 10, 10};
  vector float c = vec_mulo(a,b);
  printf("C: %f,%f,%f,%f\n", c[0], c[1], c[2], c[3]);


  //setup framebuffer
  FrameBuffer fb = createFB(fbDescriptor);

  printf("Running OpenGL %s\n", glGetString(GL_VERSION));
  while(!glfwWindowShouldClose(window->window))
    {
      clock_t start = clock(), diff;
      ///render
      preRender(triangleHitBuffer,  triangleHitPointBuffer);//clears the buffers during rendering so we dont keep last frames data in this frame

      //Trace some rays and see what we hit
      for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	{
	  for(int j = 0; j < fbDescriptor[HEIGHT]; j++)
	    {
	      Ray ray = rayInit((Vec3){i,j,0}, (Vec3){0,0,1}, 10000);
	      for(int ti = 0; ti < numTriangles; ti++)//go through the triangles
		{
		  //trace some rays
		  float distance = 0xFFFF;
		  Vec3 intersectionPoint;//This is the place in space where the ray intersects with the triangle

		  if(triangleIntersect(triangles[ti].verts[0].position, triangles[ti].verts[1].position, triangles[ti].verts[2].position, &ray, intersectionPoint))
		    {
		      //note down what triangle was intersected
		      //		      if(ray.distance < triangleDistBuffer[((j * fbDescriptor[WIDTH]) + i)])
		      if(ray.distance < distance)
			{//we hit a triangle thats closer to whatever we have hit before; if anything
			  distance = ray.distance;
			  triangleHitBuffer[((j * fbDescriptor[WIDTH]) + i)] = ti;//mark the hit buffer for this pixel with the tri that we hit
			  memcpy(triangleHitPointBuffer[((j * fbDescriptor[WIDTH]) + i)], intersectionPoint, 3 * sizeof(float));
			}
		    }
		}
	    }
	}

      //now we know what we hit, colour it to the fb
      for(int i = 0; i < fbDescriptor[WIDTH]; i++)
	{
	  for(int j = 0; j <fbDescriptor[HEIGHT]; j++)
	    {
	      unsigned char triangleID = triangleHitBuffer[((j * fbDescriptor[WIDTH]) + i)];
	      if(triangleID == 255)
		{
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = grey[0];
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = grey[1];
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = grey[2];
		}
	      else
		{
		  //read back from the buffers what triangle we hit and where
		  Vec3 hitpoint;
		  memcpy(hitpoint,  triangleHitPointBuffer[((j * fbDescriptor[WIDTH]) + i)], 3 * sizeof(float));
		  //just unpack things to be more readable and get at the vertices
		  Vertex vert0 = triangles[triangleID].verts[0]; Vertex vert1 = triangles[triangleID].verts[1]; Vertex vert2 = triangles[triangleID].verts[2];
		  //		  vec3* v0pos = &vert0.pos; vec3* v1pos = &vert1.pos; vec3* v2pos = &vert2.pos;

		  //calculate barycentric coords in the triangle
		  float wv[3];
		  barycentricCoords(wv, vert0.position, vert1.position, vert2.position, hitpoint);
		  //use the barycentric coords to weight/interpolate the colours of the verts
		  float col[3];
		  #ifdef VA1
		  //		  Vec3 v0Col =
		  col[0] = (wv[0] * (float)vert0.colour[0]) + (wv[1] * (float)vert1.colour[0]) + (wv[2] * (float)vert2.colour[0]);
		  col[1] = (wv[0] * (float)vert0.colour[1]) + (wv[1] * (float)vert1.colour[1]) + (wv[2] * (float)vert2.colour[1]);
		  col[2] = (wv[0] * (float)vert0.colour[2]) + (wv[1] * (float)vert1.colour[2]) + (wv[2] * (float)vert2.colour[2]);
		  #else
		  col[0] = (wv[0] * (float)vert0.colour[0]) + (wv[1] * (float)vert1.colour[0]) + (wv[2] * (float)vert2.colour[0]);
		  col[1] = (wv[0] * (float)vert0.colour[1]) + (wv[1] * (float)vert1.colour[1]) + (wv[2] * (float)vert2.colour[1]);
		  col[2] = (wv[0] * (float)vert0.colour[2]) + (wv[1] * (float)vert1.colour[2]) + (wv[2] * (float)vert2.colour[2]);
		  #endif
		  //write the colours to the framebuffer
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 0] = (unsigned char)col[0];
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 1] = (unsigned char)col[1];
		  fb[((j * fbDescriptor[WIDTH]) + i) * fbDescriptor[COLOURS_PER_PIXEL] + 2] = (unsigned char)col[2];
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



//This function clears the buffers used during rendering so we dont get last frames data mixed into this frame
void preRender(unsigned char* triangleHits, Vec3* intersectPoints)
{
  //clear the triangle hit buffer and distance buffer
  memset(triangleHits, 0xFF, fbDescriptor[WIDTH] * fbDescriptor[HEIGHT] * sizeof(unsigned char));
  //  for(int i = 0; i < fbDescriptor[WIDTH]; i++){
  //    for(int j = 0; j <fbDescriptor[HEIGHT]; j++){
      //triangleHits[((j * fbDescriptor[WIDTH]) + i)] = 255;
      //      triangleDists[((j * fbDescriptor[WIDTH]) + i)] = 65000;
  //  }
  //  }
}
