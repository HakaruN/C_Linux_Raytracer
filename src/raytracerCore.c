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


#define X 0
#define Y 1
#define Z 2
#define VERT1 0
#define VERT2 1
#define VERT3 2
#define VERT4 3
#define FLOATS_PER_POS 3
#define FLOATS_PER_NORM 3
#define FLOATS_PER_TEX 2



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
Vec3 red = {255,128,128};
Vec3 green = {128,255,128};
Vec3 blue = {128,128,255};
Vec3 grey = {100,100,100};
Vec3 white = {255,255,255};
Vec3 dark = {20,20,20};

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

  //load cube
  fastObjMesh* cube = fast_obj_read("/home/hakaru/Projects/C_Linux_Raytracer/res/models/cube.obj");
  ///Create a geometry from the obj file
  G g;

  //copy the vertex positions
  unsigned int posCount = cube->position_count - 1;
  g.positions = malloc(posCount * sizeof(Vec3));
  memcpy(g.positions, (cube->positions) + 3, posCount * sizeof(Vec3));

  //copy the vertex normals
  unsigned int normCount = cube->normal_count - 1;
  g.normals = malloc(normCount * sizeof(Vec3));
  memcpy(g.normals, (cube->normals + 3), normCount * sizeof(Vec3));

  //copy the tex cords
  unsigned int texCordCount = cube->texcoord_count - 1;
  g.texCords = malloc(texCordCount * sizeof(Vec2));
  memcpy(g.texCords, (cube->texcoords + 2), texCordCount * sizeof(Vec2));

  ///parse the file into objects
  unsigned int numObjects = cube->object_count;
  for(unsigned int objectID = 0; objectID < numObjects; objectID++)
  {
    fastObjGroup* pObj = &cube->objects[objectID];
    printf("Parsing object %u: %s\n", objectID, pObj->name);
    unsigned int numFaces  = pObj->face_count; //Number of faces
    unsigned int faceOffset = pObj->face_offset; //First face in fastObjMesh face_* arrays
    unsigned int indexOffset = pObj->index_offset; //First index in fastObjMesh indices array

    //iterate through each face
    for(int faceIndex = 0; faceIndex < numFaces; faceIndex+= 1)
    {
      
      unsigned int numFaceVerts = cube->face_vertices[faceOffset + faceIndex];
      printf("Face: %u. Num verts: %u\n", faceIndex, numFaceVerts);
      switch(numFaceVerts)
      {
        case 3:
        {
          printf("3\n");
          T tri;
          //iterate through the vertices of the face. 3 verts per face (the face is a triangle)
          for(unsigned int faceVertexId = 0; faceVertexId < 3; faceVertexId++)
          {
              fastObjIndex* vert = &cube->indices[indexOffset + (faceIndex * numFaceVerts) + faceVertexId];
              unsigned int vertPIndex = vert->p;
              unsigned int vertTIndex = vert->t;
              unsigned int vertNIndex = vert->n;
              printf("Vert1 pos index %u, x: %g, y: %g, z: %g\n", vertPIndex, cube->positions[vertPIndex * FLOATS_PER_POS + X], cube->positions[vertPIndex * FLOATS_PER_POS + Y], cube->positions[vertPIndex * FLOATS_PER_POS + Z]);
              printf("Vert1 tex index %u, u: %g, v: %g\n", vertTIndex, cube->texcoords[vertTIndex * FLOATS_PER_TEX + X], cube->texcoords[vertTIndex * FLOATS_PER_TEX + Y]);
              printf("Vert1 normal index %u, x: %g, y: %g, z: %g\n", vertNIndex, cube->normals[vertNIndex * FLOATS_PER_NORM + X], cube->normals[vertNIndex * FLOATS_PER_NORM + Y], cube->normals[vertNIndex * FLOATS_PER_NORM + Z]);
              tri.vertIndex[faceVertexId] = (vertPIndex * FLOATS_PER_POS);
              tri.textureIndex[faceVertexId] = (vertTIndex * FLOATS_PER_TEX);
              tri.normalIndex[faceVertexId] = (vertNIndex * FLOATS_PER_NORM);
          }
          break;
        }
        case 4:
        {
          //iterate through the vertices of the face. 4 verts per face (the face is a square)
          #ifdef DEBUG
          printf("Breaking down quad to tris\n");
          #endif
          T t1, t2;

          //get the 4 verts
          fastObjIndex* vert1 = &cube->indices[indexOffset + (faceIndex * numFaceVerts) + VERT1];
          unsigned int vert1PIndex = vert1->p - 1;// -1 because in the fast_obj lib, the verts are indexed from 1 not 0
          unsigned int vert1TIndex = vert1->t - 1;
          unsigned int vert1NIndex = vert1->n - 1;

          fastObjIndex* vert2 = &cube->indices[indexOffset + (faceIndex * numFaceVerts) + VERT2];
          unsigned int vert2PIndex = vert2->p - 1;
          unsigned int vert2TIndex = vert2->t - 1;
          unsigned int vert2NIndex = vert2->n - 1;

          fastObjIndex* vert3 = &cube->indices[indexOffset + (faceIndex * numFaceVerts) + VERT3];
          unsigned int vert3PIndex = vert3->p - 1;
          unsigned int vert3TIndex = vert3->t - 1;
          unsigned int vert3NIndex = vert3->n - 1;

          fastObjIndex* vert4 = &cube->indices[indexOffset + (faceIndex * numFaceVerts) + VERT4];
          unsigned int vert4PIndex = vert4->p - 1;
          unsigned int vert4TIndex = vert4->t - 1;
          unsigned int vert4NIndex = vert4->n - 1;

          #ifdef DEBUG
          printf("Vert1 pos index %u, x: %g, y: %g, z: %g\n", vert1PIndex, g.positions[vert1PIndex][X], g.positions[vert1PIndex][Y], g.positions[vert1PIndex][Z]);
          printf("Vert2 pos index %u, x: %g, y: %g, z: %g\n", vert2PIndex, g.positions[vert2PIndex][X], g.positions[vert2PIndex][Y], g.positions[vert2PIndex][Z]);
          printf("Vert3 pos index %u, x: %g, y: %g, z: %g\n", vert3PIndex, g.positions[vert3PIndex][X], g.positions[vert3PIndex][Y], g.positions[vert3PIndex][Z]);
          printf("Vert4 pos index %u, x: %g, y: %g, z: %g\n", vert4PIndex, g.positions[vert4PIndex][X], g.positions[vert4PIndex][Y], g.positions[vert4PIndex][Z]);

          printf("Vert1 tex index %u, u: %g, v: %g\n", vert1TIndex, g.texCords[vert1TIndex][X], g.texCords[vert1TIndex][Y]);
          printf("Vert2 tex index %u, u: %g, v: %g\n", vert2TIndex, g.texCords[vert2TIndex][X], g.texCords[vert2TIndex][Y]);
          printf("Vert3 tex index %u, u: %g, v: %g\n", vert3TIndex, g.texCords[vert3TIndex][X], g.texCords[vert3TIndex][Y]);
          printf("Vert4 tex index %u, u: %g, v: %g\n", vert4TIndex, g.texCords[vert4TIndex][X], g.texCords[vert4TIndex][Y]);

          printf("Vert1 normal index %u, x: %g, y: %g, z: %g\n", vert1NIndex, g.normals[vert1NIndex][X], g.normals[vert1NIndex][Y], g.normals[vert1NIndex][Z]);
          printf("Vert2 normal index %u, x: %g, y: %g, z: %g\n", vert2NIndex, g.normals[vert2NIndex][X], g.normals[vert2NIndex][Y], g.normals[vert2NIndex][Z]);
          printf("Vert3 normal index %u, x: %g, y: %g, z: %g\n", vert3NIndex, g.normals[vert3NIndex][X], g.normals[vert3NIndex][Y], g.normals[vert3NIndex][Z]);
          printf("Vert4 normal index %u, x: %g, y: %g, z: %g\n", vert4NIndex, g.normals[vert4NIndex][X], g.normals[vert4NIndex][Y], g.normals[vert4NIndex][Z]);
          #endif

          //Get vertex indices
          //triangle 1
          t1.vertIndex[0] = vert1PIndex;
          t1.vertIndex[1] = vert2PIndex;
          t1.vertIndex[2] = vert3PIndex;
          t1.textureIndex[0] = vert1TIndex;
          t1.textureIndex[1] = vert2TIndex;
          t1.textureIndex[2] = vert3TIndex;
          t1.normalIndex[0] = vert1NIndex;
          t1.normalIndex[1] = vert2NIndex;
          t1.normalIndex[2] = vert3NIndex;

          //triangle 2
          t2.vertIndex[0] = vert3PIndex;
          t2.vertIndex[1] = vert4PIndex;
          t2.vertIndex[2] = vert1PIndex;
          t2.textureIndex[0] = vert3TIndex;
          t2.textureIndex[1] = vert4TIndex;
          t2.textureIndex[2] = vert1TIndex;
          t2.normalIndex[0] = vert3NIndex;
          t2.normalIndex[1] = vert4NIndex;
          t2.normalIndex[2] = vert1NIndex;

          #ifdef DEBUG
          ///print tri 1
          //vert pos
          printf("T1 v0 pos x: %g, y: %g, z: %g\n", g.positions[t1.vertIndex[0]][X],  g.positions[t1.vertIndex[0]][Y], g.positions[t1.vertIndex[0]][Z]);
          printf("T1 v1 pos x: %g, y: %g, z: %g\n", g.positions[t1.vertIndex[1]][X],  g.positions[t1.vertIndex[1]][Y], g.positions[t1.vertIndex[1]][Z]);
          printf("T1 v2 pos x: %g, y: %g, z: %g\n", g.positions[t1.vertIndex[2]][X],  g.positions[t1.vertIndex[2]][Y], g.positions[t1.vertIndex[2]][Z]);
          //vert tex
          printf("T1 v0 tex u: %g, v: %g\n", g.texCords[t1.textureIndex[0]][X],  g.texCords[t1.textureIndex[0]][Y]);
          printf("T1 v1 tex u: %g, v: %g\n", g.texCords[t1.textureIndex[1]][X],  g.texCords[t1.textureIndex[1]][Y]);
          printf("T1 v2 tex u: %g, v: %g\n", g.texCords[t1.textureIndex[2]][X],  g.texCords[t1.textureIndex[2]][Y]);
          //vert norms
          printf("T1 v0 normal x: %g, y: %g, z: %g\n", g.normals[t1.normalIndex[0]][X],  g.normals[t1.normalIndex[0]][Y], g.normals[t1.normalIndex[0]][Z]);
          printf("T1 v1 normal x: %g, y: %g, z: %g\n", g.normals[t1.normalIndex[1]][X],  g.normals[t1.normalIndex[1]][Y], g.normals[t1.normalIndex[1]][Z]);
          printf("T1 v2 normal x: %g, y: %g, z: %g\n", g.normals[t1.normalIndex[2]][X],  g.normals[t1.normalIndex[2]][Y], g.normals[t1.normalIndex[2]][Z]);

          ///print tri 2
          //vert pos
          printf("T2 v0 pos x: %g, y: %g, z: %g\n", g.positions[t2.vertIndex[0]][X],  g.positions[t2.vertIndex[0]][Y], g.positions[t2.vertIndex[0]][Z]);
          printf("T2 v1 pos x: %g, y: %g, z: %g\n", g.positions[t2.vertIndex[1]][X],  g.positions[t2.vertIndex[1]][Y], g.positions[t2.vertIndex[1]][Z]);
          printf("T2 v2 pos x: %g, y: %g, z: %g\n", g.positions[t2.vertIndex[2]][X],  g.positions[t2.vertIndex[2]][Y], g.positions[t2.vertIndex[2]][Z]);
          //vert tex
          printf("T2 v0 tex u: %g, v: %g\n", g.texCords[t2.textureIndex[0]][X],  g.texCords[t2.textureIndex[0]][Y]);
          printf("T2 v1 tex u: %g, v: %g\n", g.texCords[t2.textureIndex[1]][X],  g.texCords[t2.textureIndex[1]][Y]);
          printf("T2 v2 tex u: %g, v: %g\n", g.texCords[t2.textureIndex[2]][X],  g.texCords[t2.textureIndex[2]][Y]);
          //vert norms
          printf("T2 v0 normal x: %g, y: %g, z: %g\n", g.normals[t2.normalIndex[0]][X],  g.normals[t2.normalIndex[0]][Y], g.normals[t2.normalIndex[0]][Z]);
          printf("T2 v1 normal x: %g, y: %g, z: %g\n", g.normals[t2.normalIndex[1]][X],  g.normals[t2.normalIndex[1]][Y], g.normals[t2.normalIndex[1]][Z]);
          printf("T2 v2 normal x: %g, y: %g, z: %g\n", g.normals[t2.normalIndex[2]][X],  g.normals[t2.normalIndex[2]][Y], g.normals[t2.normalIndex[2]][Z]);
          #endif
          break;
        }
        default:
        {
          printf("Error loading geometry. Unsupported number of vertices per face\n");
          return 0;
        }
      }
    }

  }

  

  //Vert pos
   unsigned int                position_count = cube->position_count;
   float*                      positions = cube->positions;
   /*
   printf("Vert positions\n");
    for(unsigned int i = 3; i < position_count * 3; i += 3)
    {
      printf("%g %g %g\n", positions[i+0], positions[i+1], positions[i+2]);
    }
    */

    //Tex cords
    unsigned int                texcoord_count = cube->texcoord_count;
    float*                      texcoords = cube->texcoords;
    /*
    printf("Vert texcoords\n");
    for(unsigned int i = 2; i < texcoord_count * 2; i += 2)
      printf("%g %g\n", texcoords[i+0], texcoords[i+1]);
    */
    unsigned int                normal_count = cube->normal_count;
    float*                      normals = cube->normals;
    /*
    printf("Vert normals\n");
    for(unsigned int i = 3; i < normal_count * 3; i += 3)
      printf("%g %g %g\n", normals[i+0], normals[i+1], normals[i+2]);
    */

    //unsigned int                color_count = cube->color_count;
    //float*                      colors = cube->colors;
    /* Face data: one element for each face */
    unsigned int                face_count = cube->face_count;
    unsigned int*               face_vertices = cube->face_vertices;
    //unsigned int*               face_materials = cube->face_materials;

    unsigned int                object_count = cube->object_count;
    //printf("object count: %u\n", object_count);

    /* Index data: one element for each face vertex */
    unsigned int                index_count = cube->index_count;
    fastObjIndex*               indices = cube->indices;
    /*
    printf("Indices\n");
    for(unsigned int i = 0; i < index_count; i++)
      printf("Vert: %d pos idx: %d, tex idx: %d, nrm idx: %d\n", i, indices[i].p, indices[i].t, indices[i].n);
      */

    
  //printf("objects: %u\n", cube->object_count);



/*
  for(unsigned int i = 0; i < posCount; i++)
  {
    printf("Vert %u: ", i);
    printVec3(g.positions[i]);
  }
  for(unsigned int i = 0; i < normCount; i++)
  {
    printf("Normal %u: ", i);
    printVec3(g.normals[i]);
  }
  for(unsigned int i = 0; i < texCordCount; i++)
  {
    printf("Tex cord %u: ", i);
    printVec2(g.texCords[i]);
    //printf("GPositions %g %g %g\n", g.positions[i][0], g.positions[i][1], g.positions[i][2]);
  }
  */

  //find how many verts per face
  //unsigned int vertsPerFace = cube->face_vertices[0];
  //printf("object has %u vertices per face and %u faces\n", vertsPerFace, face_count);

  //parse the verts into triangles. If we have 3 verts per face then it's easy. If not we have to turn the faces into multiple triangles
  //for 4 verts per face the algo is tri1 = v0, v1, v2. tri2 = v2, v3, v0.
  /*
  typedef struct T{
  //Vertex data for the 3 verts
  unsigned int vertIndex[3];
  unsigned int normalIndex[3];
  unsigned int textureIndex[3];
  Vec3 position;
  Texture* texture;
} T;
*/





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
  verticesAddVert(&vertices, vertexGen((Vec3){50, 0, 0}, normal, green, (Vec2){1, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){0, 100, 0}, normal, blue, (Vec2){0.5, 1}));
  
  //triangle 2
  verticesAddVert(&vertices, vertexGen((Vec3){-50, 0, 0}, normal, grey, (Vec2){0, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 0, 0}, normal, white, (Vec2){1, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){0, 50, 0}, normal, dark, (Vec2){0.5, 1}));
  
  //triangle 3
  verticesAddVert(&vertices, vertexGen((Vec3){100, 50, 5}, normal, red, (Vec2){0.25, 0.1}));
  verticesAddVert(&vertices, vertexGen((Vec3){300, 100, 5}, normal, green, (Vec2){0.75, 0.1}));
  verticesAddVert(&vertices, vertexGen((Vec3){200, 125, 25}, normal, blue, (Vec2){0.5,0.75}));
  //triangle 4
  verticesAddVert(&vertices, vertexGen((Vec3){0, 0, 0}, normal, green, (Vec2){0, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 0, 0}, normal, red, (Vec2){1, 0}));
  verticesAddVert(&vertices, vertexGen((Vec3){50, 50, 0}, normal, blue, (Vec2){1, 1}));


  //////////////////
  //MESH && GEOMETRY
  //////////////////
  Mesh mesh = meshGen(1);
  Vec3 geomPos = {200, 200, 100};
  unsigned int numTriangles = 2;
  meshGenGeometry(&mesh, numTriangles, geomPos);//Adding geom 1 to the mesh
  meshGenGeometry(&mesh, numTriangles, geomPos);//Adding geom 2 to the mesh

  ////////////////
  //TRIANGLES
  ////////////////  
  Vertex verts[3];
  unsigned int geomId = 0;//add the triangles to geom 0
  verts[0] = *verticesGetVert(&vertices, 0);
  verts[1] = *verticesGetVert(&vertices, 1);
  verts[2] = *verticesGetVert(&vertices, 2);
  if(!meshGeomAddTri(&mesh, geomId, triangleGen(verts, (Vec3){200, 200, 100}, textures[0].image == NULL ? NULL: &textures[0])))
    return 0;

  verts[0] = *verticesGetVert(&vertices, 3);
  verts[1] = *verticesGetVert(&vertices, 4);
  verts[2] = *verticesGetVert(&vertices, 5);
  if(!meshGeomAddTri(&mesh, geomId, triangleGen(verts, (Vec3){100, 100, 5}, textures[1].image == NULL ? NULL: &textures[1])))
    return 0;

  geomId = 1;//add the triangles to geom 1
  verts[0] = *verticesGetVert(&vertices, 6);
  verts[1] = *verticesGetVert(&vertices, 7);
  verts[2] = *verticesGetVert(&vertices, 8);
  if(!meshGeomAddTri(&mesh, geomId, triangleGen(verts, (Vec3){0, 0, 0}, NULL)))
    return 0;

  verts[0] = *verticesGetVert(&vertices, 9);
  verts[1] = *verticesGetVert(&vertices, 10);
  verts[2] = *verticesGetVert(&vertices, 11);
  if(!meshGeomAddTri(&mesh, geomId, triangleGen(verts, (Vec3){0, 0, 5}, textures[1].image == NULL ? NULL: &textures[1])))
    return 0;


  //////////////////
  //BVH
  //////////////////
  //Setup the bvh node
  Vec3 bmin = {0,0,10};
  Vec3 bmax = {400,400,110};
  BBox* rootBox = genBox(bmin, bmax);
  BvhNode* rootNode = bvhNodeGen(8, 1, rootBox);

  //first tri goes in the root node just to show it can do and will still work
  meshInsertToBvh(&mesh, rootNode);



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
      traceSecondaryRays(rootNode, rayHitBuffer, rayHitpointBuffer, rayHitnormalBuffer, rayHitDirectionBuffer, fbDescriptor);

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

