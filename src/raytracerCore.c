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
  textures = malloc(sizeof(Texture) * numTextures);

  Vec3 red = {255,128,128};
  Vec3 green = {128,255,128};
  Vec3 blue = {128,128,255};
  //Vec3 grey = {100,100,100};
  //Vec3 white = {255,255,255};
  //Vec3 dark = {20,20,20};


  //load some textures
  if(!textures)
    {
      printf("Error initialising texture list\n");
      return -1;
    }

  int numChannels = 3;
  const char* tex1Path = "/home/hakaru/Projects/LinuxRT/C_Linux_Raytracer/res/scrot.png";
  const char* tex2Path = "/home/hakaru/Projects/LinuxRT/C_Linux_Raytracer/res/tex1.jpg";
  textures[0] = loadTexture(tex1Path, numChannels);
  textures[1] = loadTexture(tex2Path, numChannels);



  //////////
  //VERTICES
  //////////
  Vec3 normal = {1,1,1};
  //Generate the vertices list
  const unsigned int vLen = 17;//num verts
  Vertices vertices = verticesGen(vLen);
  if(vertices.vertices != NULL)
    printf("Vertices buffer generated\n");
  else{
    printf("Vertices buffer failed to gen\n");
    return -1;}


  //Each triangle will have 3 verts
  //triangle 1
  verticesAddVert(&vertices, vertexGen((Vec3){-50, 0, 0}, normal, red, (Vec2){0, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 0, 0}, normal, green, (Vec2){100, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){0, 100, 0}, normal, blue, (Vec2){0, 100}));
  //triangle 2
  verticesAddVert(&vertices, vertexGen((Vec3){-50, 0, 0}, normal, green, (Vec2){0, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 0, 0}, normal, red, (Vec2){textures[1].width, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){0, 50, 0}, normal, blue, (Vec2){textures[1].width/2, textures[1].height}));
  //triangle 3
  verticesAddVert(&vertices, vertexGen((Vec3){100, 50, 5}, normal, blue, (Vec2){150-25, 100}));
  verticesAddVert(&vertices, vertexGen((Vec3){300, 100, 5}, normal, red, (Vec2){225, 100}));
  verticesAddVert(&vertices, vertexGen((Vec3){200, 125, 25}, normal, green, (Vec2){175,150}));
  //triangle 4
  verticesAddVert(&vertices, vertexGen((Vec3){0, 0, 0}, normal, green, (Vec2){0, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 0, 0}, normal, red, (Vec2){textures[1].width, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 50, 0}, normal, blue, (Vec2){textures[1].width/2, textures[1].height}));


  //////////////////
  //GEOMETRY
  //////////////////
  ///Init geometry
  initGeomCtr(0);
  //Add geometry
  Vec3 geomPos = {200, 200, 100};
  unsigned int numTriangles = 20;
  Geometry g1 = genGeometry(numTriangles, geomPos);


  ////////////////
  //TRIANGLES
  ////////////////
  //init triangles
  //triangles = malloc(numTriangles * sizeof(Triangle));

  Vertex verts[3];
  verts[0] = *verticesGetVert(&vertices, 0);
  verts[1] = *verticesGetVert(&vertices, 1);
  verts[2] = *verticesGetVert(&vertices, 2);
  if(!geomAddTriangle(&g1, triangleGen(verts, (Vec3){200, 200, 100}, &textures[0])))
    return 0;

  verts[0] = *verticesGetVert(&vertices, 3);
  verts[1] = *verticesGetVert(&vertices, 4);
  verts[2] = *verticesGetVert(&vertices, 5);
  if(!geomAddTriangle(&g1, triangleGen(verts, (Vec3){100, 100, 5}, &textures[1])))
    return 0;

  verts[0] = *verticesGetVert(&vertices, 6);
  verts[1] = *verticesGetVert(&vertices, 7);
  verts[2] = *verticesGetVert(&vertices, 8);
  if(!geomAddTriangle(&g1, triangleGen(verts, (Vec3){0, 0, 0}, NULL)))
    return 0;

  verts[0] = *verticesGetVert(&vertices, 9);
  verts[1] = *verticesGetVert(&vertices, 10);
  verts[2] = *verticesGetVert(&vertices, 11);
  if(!geomAddTriangle(&g1, triangleGen(verts, (Vec3){0, 0, 5}, &textures[1])))
    return 0;


  //////////////////
  //BVH
  //////////////////
  //Setup the bvh node
  Vec3 bmin = {0,0,10};
  Vec3 bmax = {400,400,110};
  BBox* rootBox = genBox(bmin, bmax);
  BvhNode* rootNode = bvhNodeGen(8, 3, *rootBox);

  Vec3 bmin1 = {0,0,10};//{100,100,10};
  Vec3 bmax1 = {400,400,110};//{300,300,110};
  BBox* box1 = genBox(bmin1, bmax1);
  BvhNode* node1 = bvhNodeGen(8, 3, *box1);

  BvhNode* pNode;

  //first tri goes in the root node just to show it can do and will still work
  pNode = bvhAddTriangle(rootNode, g1.geometryID, geomGetTriangle(&g1, 0));
  if(!pNode)
    return 0;
  geomSetBackPtr(&g1, 0, pNode);
  //next 3 triangles go in child node just to see it can, also it's happening in a loop to show it can be automated
  for(int i = 1; i < 4; i++)
  {
      pNode = bvhAddTriangle(rootNode, g1.geometryID, geomGetTriangle(&g1, i));
      if(!pNode)
        return 0;
      geomSetBackPtr(&g1, i, pNode);
  }

  //Set the child node as a child of the root node.
  bvhAddChild(rootNode, node1);
  if(rootNode)
    printf("BVH root inited\n");


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
      traceRays(rootNode, &camera, rayHitBuffer, rayHitpointBuffer, fbDescriptor, invHeightMinus1, invWidthMinus1);

      ///Shading, sample the texture or interpolate vertex colours of where we hit and put it in the framebuffer
      shading(frameBuffer, rayHitBuffer, rayHitpointBuffer, fbDescriptor);

      //Copy framebuffer to GPU
      glDrawPixels(fbDescriptor[WIDTH], fbDescriptor[HEIGHT], GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);

      //swap buffers
      glfwSwapBuffers(window->window);

      //poll and process events
      glfwPollEvents();
      
      #ifdef FRAME_TIMINGS
      diff = clock() - start;
      int msec = diff * 1000 /CLOCKS_PER_SEC;
      printf("Frametime: %d\n", msec%1000);
      #endif
    }


  freeFB(frameBuffer);
  freeWindow(window);
  free(triangles);
  free(rayHitBuffer);
  glfwTerminate();

  return 0;
}
