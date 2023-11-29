#include "../include/Window.h"
#include "../include/FrameBuffer.h"
#include "../include/Raytracer.h"
#include "../include/Triangle.h"
#include "../include/BvhNode.h"
#include "../include/Box.h"
#include "../include/Ray.h"
#include "../include/Texture.h"
#include "../include/Camera.h"
#include "../include/FP.h"
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
unsigned int numTriangles = 4;
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

/*
    UFX16_16 ua = ufloatToFixed(0.000001);
    UFX16_16 ub = ufloatToFixed(1.05f);
    float ui = ufixedToFloat(ua);
    float uj = ufloatToFixed(ub);

    UFX16_16 uc = uFXMul(ua, ub);
    float uk = ufixedToFloat(uc);

    UFX16_16 ud = uFXDiv(ua, ub);
    float ul = ufixedToFloat(ud);

    SFX16_16 sa = sfloatToFixed(60000.f);
    SFX16_16 sb = sfloatToFixed(1.05f);
    float si = sfixedToFloat(sa);
    float sj = sfloatToFixed(sb);

    SFX16_16 sc = sFXMul(sa, sb);
    float sk = sfixedToFloat(sc);

    SFX16_16 sd = sFXDiv(sa, sb);
    float sl = sfixedToFloat(sd);

    float uab = ufixedToFloat(ua + sb);

    UFX16_16 i = 5 << 16;
    */




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
  rayHitBuffer = malloc(sizeof(Triangle*) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  rayHitpointBuffer = malloc(sizeof(Vec3) * fbDescriptor[WIDTH] * fbDescriptor[HEIGHT]);
  textures = malloc(sizeof(Texture) * numTextures);

  Vec3 red = {255,128,128};
  Vec3 green = {128,255,128};
  Vec3 blue = {128,128,255};
  //Vec3 grey = {100,100,100};
  //Vec3 white = {255,255,255};
  //Vec3 dark = {20,20,20};

  Vec3 normal = {1,1,1};
  //Generate the vertices list -- unused atm
  const unsigned int vLen = 20;//num verts
  Vertices vertices = verticesGen(vLen);

  if(vertices.vertices != NULL)
    printf("Vertices buffer generated\n");
  else{
    printf("Vertices buffer failed to gen\n");
    return -1;}


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


  //init triangles
  triangles = malloc(numTriangles * sizeof(Triangle));

  Vertex verts[3];
  if(triangles)
    {
      #ifdef FX
      verts[0] = vertexGen((Vec3){ItoFX(-50), 0, 0}, normal, red, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){ItoFX(50), 0, 0}, normal, green, (Vec2){ItoFX(100), 0});
      verts[2] = vertexGen((Vec3){0, ItoFX(100), 0}, normal, blue, (Vec2){0, ItoFX(100)});
      triangles[0] = triangleGen(verts, (Vec3){200, 200, 100}, &textures[0]);

      verts[0] = vertexGen((Vec3){ItoFX(-50), 0, 0}, normal, green, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){ItoFX(50), 0, 0}, normal, red, (Vec2){ItoFX(textures[1].width), 0});
      verts[2] = vertexGen((Vec3){0, ItoFX(50), 0}, normal, blue, (Vec2){ItoFX(textures[1].width/2), ItoFX(textures[1].height)});
      triangles[1] = triangleGen(verts, (Vec3){ItoFX(100), ItoFX(100), ItoFX(5)}, &textures[1]);

      verts[0] = vertexGen((Vec3){ItoFX(100), ItoFX(50), ItoFX(5)}, normal, blue, (Vec2){ItoFX(125), ItoFX(100)});
      verts[1] = vertexGen((Vec3){ItoFX(300), ItoFX(100), ItoFX(5)}, normal, red, (Vec2){ItoFX(225), ItoFX(100)});
      verts[2] = vertexGen((Vec3){ItoFX(200), ItoFX(125), ItoFX(25)}, normal, green, (Vec2){ItoFX(175),ItoFX(150)});
      triangles[2] = triangleGen(verts, (Vec3){0, 0, 0}, NULL);

      verts[0] = vertexGen((Vec3){0, 0, 0}, normal, green, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){ItoFX(50), 0, 0}, normal, red, (Vec2){ItoFX(textures[1].width), 0});
      verts[2] = vertexGen((Vec3){ItoFX(50), ItoFX(50), 0}, normal, blue, (Vec2){ItoFX(textures[1].width/2), ItoFX(textures[1].height)});
      triangles[3] = triangleGen(verts, (Vec3){0, 0, ItoFX(5)}, &textures[1]);
      #else
      verts[0] = vertexGen((Vec3){-50, 0, 0}, normal, red, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){50, 0, 0}, normal, green, (Vec2){100, 0});
      verts[2] = vertexGen((Vec3){0, 100, 0}, normal, blue, (Vec2){0, 100});
      triangles[0] = triangleGen(verts, (Vec3){200, 200, 100}, &textures[0]);

      verts[0] = vertexGen((Vec3){-50, 0, 0}, normal, green, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){50, 0, 0}, normal, red, (Vec2){textures[1].width, 0});
      verts[2] = vertexGen((Vec3){0, 50, 0}, normal, blue, (Vec2){textures[1].width/2, textures[1].height});
      triangles[1] = triangleGen(verts, (Vec3){100, 100, 5}, &textures[1]);

      verts[0] = vertexGen((Vec3){100, 50, 5}, normal, blue, (Vec2){150-25, 100});
      verts[1] = vertexGen((Vec3){300, 100, 5}, normal, red, (Vec2){225, 100});
      verts[2] = vertexGen((Vec3){200, 125, 25}, normal, green, (Vec2){175,150});
      triangles[2] = triangleGen(verts, (Vec3){0, 0, 0}, NULL);

      verts[0] = vertexGen((Vec3){0, 0, 0}, normal, green, (Vec2){0, 0});
      verts[1] = vertexGen((Vec3){50, 0, 0}, normal, red, (Vec2){textures[1].width, 0});
      verts[2] = vertexGen((Vec3){50, 50, 0}, normal, blue, (Vec2){textures[1].width/2, textures[1].height});
      triangles[3] = triangleGen(verts, (Vec3){0, 0, 5}, &textures[1]);
      #endif
    }
  else
    return -1;

  //Setup the bvh node
  #ifdef FX
  Vec3 bmin = {ItoFX(0),ItoFX(0),ItoFX(10)};
  Vec3 bmax = {ItoFX(400),ItoFX(400),ItoFX(110)};
  BBox* rootBox = genBox(bmin, bmax);
  BvhNode* rootNode = bvhNodeGen(8, 3, *rootBox);

  Vec3 bmin1 = {ItoFX(0),ItoFX(0),ItoFX(10)};//{100,100,10};
  Vec3 bmax1 = {ItoFX(400),ItoFX(400),ItoFX(110)};//{300,300,110};
  #else
  Vec3 bmin = {0,0,10};
  Vec3 bmax = {400,400,110};
  BBox* rootBox = genBox(bmin, bmax);
  BvhNode* rootNode = bvhNodeGen(8, 3, *rootBox);

  Vec3 bmin1 = {0,0,10};//{100,100,10};
  Vec3 bmax1 = {400,400,110};//{300,300,110};
  #endif



  BBox* box1 = genBox(bmin1, bmax1);
  BvhNode* node1 = bvhNodeGen(8, 3, *box1);
  bvhAddTriangle(rootNode, triangles[0]);
  bvhAddTriangle(rootNode, triangles[1]);
  bvhAddTriangle(node1, triangles[2]);
  bvhAddTriangle(node1, triangles[3]);


  bvhAddChild(rootNode, node1);
  if(rootNode)
    printf("BVH root inited\n");
  #ifdef FX
  Camera camera = cameraGen((Vec3){0,0,0}, (Vec3){0,0,ItoFX(100)}, (Vec3){0,ItoFX(1),0}, ItoFX(30), fbDescriptor[WIDTH]/fbDescriptor[HEIGHT]);
  #else
  Camera camera = cameraGen((Vec3){0,0,0}, (Vec3){0,0,100}, (Vec3){0,1,0}, 30, fbDescriptor[WIDTH]/fbDescriptor[HEIGHT]);
  #endif

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
