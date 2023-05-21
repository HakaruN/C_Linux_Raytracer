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
unsigned int numTriangles = 3;
FbDescriptor fbDescriptor = {400, 400, 3};//Descriptor for the FB, [0] = WIDTH, [1] = HEIGHT, [2] = #colours per pixel

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

  //Gen the window
  Window* window = createWindow(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], "Raytracer", fullscreen, primMonitor, NULL);

  //init the triangle hit buffer and distance buffer
  rayHitBuffer = malloc(sizeof(unsigned short) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  rayHitpointBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  textures = malloc(sizeof(Texture) * numTextures);

  Vec3 red = {255,128,128};
  Vec3 green = {128,255,128};
  Vec3 blue = {128,128,255};
  Vec3 grey = {100,100,100};
  Vec3 white = {255,255,255};
  Vec3 dark = {20,20,20};

  Vec3 normal = {1,1,1};
  //Generate the vertices list
  const unsigned int vLen = 20;//num verts
  Vertices vertices = verticesGen(vLen);
  if(vertices.vertices != NULL)
    printf("Vertices buffer generated\n");
  else{
    printf("Vertices buffer failed to gen\n");
    return -1;}


  verticesAddVert(&vertices, vertexGen((Vec3){-50, 0, 0}, normal, red, (Vec2){0, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 0, 0}, normal, green, (Vec2){100, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){0, 100, 0}, normal, blue, (Vec2){0, 100}));

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
      verts[0] = *verticesGetVert(&vertices, 0);
      verts[1] = *verticesGetVert(&vertices, 1);
      verts[2] = *verticesGetVert(&vertices, 2);
      triangles[0] = triangleGen(verts, (Vec3){200, 200, 100}, &textures[0]);

      verts[0] = vertexGen((Vec3){-50, 0, 0}, normal, green, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){50, 0, 0}, normal, red, (Vec2){textures[1].width, 0});
      verts[2] = vertexGen((Vec3){0, 50, 0}, normal, blue, (Vec2){textures[1].width/2, textures[1].height});
      triangles[1] = triangleGen(verts, (Vec3){100, 100, 5}, &textures[1]);

      verts[0] = vertexGen((Vec3){100, 50, 5}, normal, blue, (Vec2){150-25, 100});
      verts[1] = vertexGen((Vec3){300, 100, 5}, normal, red, (Vec2){225, 100});
      verts[2] = vertexGen((Vec3){200, 125, 25}, normal, green, (Vec2){175,150});
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

  Camera camera = cameraGen((Vec3){0,0,0}, (Vec3){0,0,100}, (Vec3){0,1,0}, 30, fbDescriptor[WIDTH]/fbDescriptor[HEIGHT]);

  //setup framebuffer
  FrameBuffer frameBuffer = createFB(fbDescriptor);

  printf("Running OpenGL %s\n", glGetString(GL_VERSION));

  //getting the inverse of the height and width -1 outside the loop so we can perform multiplies with the invers (= to / of normal. multiplies are cheaper than division and this is happening lots
  float invHeightMinus1 = 1.0f / (fbDescriptor[HEIGHT] - 1.0f);
  float invWidthMinus1 = 1.0f / (fbDescriptor[WIDTH] - 1.0f);
  while(!glfwWindowShouldClose(window->window))
    {
      clock_t start = clock(), diff;

      ////Rendering pipeline
      //Clear the screen
      glClear(GL_COLOR_BUFFER_BIT);

      //Clear the pixel's triangle hit buffer
      memset(rayHitBuffer, 0xFFFF, fbDescriptor[WIDTH] * fbDescriptor[HEIGHT] * sizeof(unsigned short));

      ///Fire rays through each pixel and see what we hit (marked in the rayHitBuffer previously cleared)
      traceRays(triangles, numTriangles, &camera, rayHitBuffer, rayHitpointBuffer, fbDescriptor, invHeightMinus1, invWidthMinus1);

      ///Shading, sample the texture or interpolate vertex colours of where we hit and put it in the framebuffer
      shading(triangles, frameBuffer, rayHitBuffer, rayHitpointBuffer, fbDescriptor);

      //Copy framebuffer to GPU
      glDrawPixels(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);

      //swap buffers
      glfwSwapBuffers(window->window);

      //poll and process events
      glfwPollEvents();
      diff = clock() - start;
      int msec = diff * 1000 /CLOCKS_PER_SEC;
      printf("Frametime: %d\n", msec%1000);
    }


  freeFB(frameBuffer);
  freeWindow(window);
  free(triangles);
  free(rayHitBuffer);
  glfwTerminate();

  return 0;
}
