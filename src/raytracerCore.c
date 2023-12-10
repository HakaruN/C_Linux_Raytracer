#include "../include/Window.h"
#include "../include/FrameBuffer.h"
#include "../include/Raytracer.h"
#include "../include/Triangle.h"
#include "../include/BvhNode.h"
#include "../include/Box.h"
#include "../include/Ray.h"
#include "../include/Texture.h"
#include "../include/Camera.h"
#include "../include/Geometry.h"
#include "../include/Vertex.h"
#include "../include/Mesh.h"
#include "../include/Maths.h"
#include "../vendor/fast_obj/fast_obj.h"
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
FbDescriptor fbDescriptor = {400, 400, 3};//Descriptor for the FB, [0] = WIDTH, [1] = HEIGHT, [2] = #colours per pixel

//how many are allocated
//Buffers
Texture* textures;//list of textures
Triangle* triangles;//list of triangles
RayHitBuffer rayHitBuffer;//what triangle the ray intersected
RayHitpointBuffer rayHitpointBuffer;//where on the triangle we hit
RayHitNormalBuffer rayHitnormalBuffer;//The normal of the geom that was hit
RayHitDirectionBuffer rayHitDirectionBuffer;//The direction of the ray


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


  //////SETUP RENDER BUFFERS
  //init the triangle hit buffer and distance buffer
  rayHitBuffer = malloc(sizeof(Triangle*) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  rayHitpointBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  rayHitnormalBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  rayHitDirectionBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  textures = malloc(sizeof(Texture) * numTextures);


//colours
/*
Vec3 red = {255,128,128};
Vec3 green = {128,255,128};
Vec3 blue = {128,128,255};
Vec3 grey = {100,100,100};
Vec3 white = {255,255,255};
Vec3 dark = {20,20,20};
*/

  //load some textures
  if(!textures)
    {
      printf("Error initialising texture list\n");
      return -1;
    }

  int numChannels = 3;
  const char* tex1Path = "//home/hakaru/Projects/C_Linux_Raytracer/res/scrot.png";
  const char* tex2Path = "/home/hakaru/Projects/C_Linux_Raytracer/res/tex1.jpg";
  textures[0] = loadTexture(tex1Path, numChannels);
  textures[1] = loadTexture(tex2Path, numChannels);

  M* m1 = meshLoadOBJ("/home/hakaru/Projects/C_Linux_Raytracer/res/models/cube.obj");


  ///Print the loaded model
  
  for(unsigned int geomId = 0; geomId < m1->numGeometries; geomId++)
  {
    printf("Geometry %u: \n", geomId);
    G* geom = &m1->geometries[geomId];
    for(unsigned int triIdx = 0; triIdx < geom->numTriangles; triIdx++)
    {
      printf("\tTriangle %u: \n", triIdx);
      T* t = &geom->triangles[triIdx];

      geom->triangles[triIdx].texture = &textures[0];
      for(unsigned int vertIdx = 0; vertIdx < 3; vertIdx++)
      {
        printf("\t\tVertex %u\n", vertIdx);
        Vec3 vPos;
        Vec2 vTex;
        Vec3 vNrm;
        memcpy(vPos, t->vertPosition[vertIdx], sizeof(Vec3));
        memcpy(vTex, t->vertTexture[vertIdx], sizeof(Vec2));
        memcpy(vNrm, t->vertNormal[vertIdx], sizeof(Vec3));
        printf("\t\t\t Pos: %g, %g, %g\n", vPos[0], vPos[1], vPos[2]);       
        printf("\t\t\t Tex: %g, %g\n", vTex[X], vTex[Y]);        
        printf("\t\t\t Nrm: %g, %g, %g\n", vNrm[X], vNrm[Y], vNrm[Z]);        


      }
    }
  }
  
   //////////////////
  //BVH
  //////////////////
  //Setup the bvh node
  Vec3 bmin = {0,0,10};
  Vec3 bmax = {400,400,110};
  BBox* rootBox = genBox(bmin, bmax);
  BvhNode* rootNode = bvhNodeGen(8, 1, rootBox);

  //first tri goes in the root node just to show it can do and will still work
  //meshInsertToBvh(&mesh, rootNode);
  mInsertToBvh(m1, rootNode);



  Camera camera = cameraGen((Vec3){0,0,0}, (Vec3){0,0,100}, (Vec3){0,1,0}, 30, fbDescriptor[WIDTH]/fbDescriptor[HEIGHT]);

  //setup framebuffer
  FrameBuffer frameBuffer = createFB(fbDescriptor);

  printf("Running OpenGL %s\n", glGetString(GL_VERSION));

  //getting the inverse of the height and width -1 outside the loop so we can perform multiplies with the invers (= to / of normal. multiplies are cheaper than division and this is happening lots
  float invHeightMinus1 = 1.0f / (fbDescriptor[HEIGHT] - 1.0f);
  float invWidthMinus1 = 1.0f / (fbDescriptor[WIDTH] - 1.0f);
  while(!glfwWindowShouldClose(window->window))
    {
      #ifdef FRAME_TIMINGS
      clock_t start = clock(), diff;
      #endif

      ////Rendering pipeline
      //Clear the screen
      glClear(GL_COLOR_BUFFER_BIT);

      //Clear the pixel's triangle hit buffer
      memset(rayHitBuffer, 0x00000000, fbDescriptor[WIDTH] * fbDescriptor[HEIGHT] * sizeof(Triangle*));

      ///Fire rays through each pixel and see what we hit (marked in the rayHitBuffer previously cleared)
      traceRays(rootNode, &camera, rayHitBuffer, rayHitpointBuffer, rayHitnormalBuffer, rayHitDirectionBuffer, fbDescriptor, invHeightMinus1, invWidthMinus1);

      ///Secondary rays
      //traceSecondaryRays(rootNode, rayHitBuffer, rayHitpointBuffer, rayHitnormalBuffer, rayHitDirectionBuffer, fbDescriptor);

      ///Shading, sample the texture or interpolate vertex colours of where we hit and put it in the framebuffer
      shading(frameBuffer, rayHitBuffer, rayHitpointBuffer, rayHitnormalBuffer, rayHitDirectionBuffer, fbDescriptor);

      //Copy framebuffer to GPU
      glDrawPixels(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);

      //swap buffers
      glfwSwapBuffers(window->window);

      //poll and process events
      glfwPollEvents();
      
      #ifdef FRAME_TIMINGS
      diff = clock() - start;
      int msec = diff * 1000 /CLOCKS_PER_SEC;
      //printf("Frametime: %d\n", msec%1000);
      #endif
    }


  freeFB(frameBuffer);
  freeWindow(window);
  free(triangles);
  free(rayHitBuffer);
  glfwTerminate();

  return 0;
}

