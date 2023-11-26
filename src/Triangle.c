#include "../include/Triangle.h"

Triangle triangleGen(Vertex* verts, Vec3 pos, Texture* texture)
{
  Triangle t;
  if(texture)
    t.texture = texture;
  else
    t.texture = NULL;
  if(verts){
    memcpy(t.verts, verts, 3 * sizeof(Vertex));//copy the verts
    ////Transform the triangle's vert pos now so doesn't happen at render-time
    //Transform the verts pos with the triangle pos and store them in the verts transformedPos
    Vec3 tVert0Pos, tVert1Pos, tVert2Pos;
    vec3Add(t.verts[0].position, pos, tVert0Pos);
    vec3Add(t.verts[1].position, pos, tVert1Pos);
    vec3Add(t.verts[2].position, pos, tVert2Pos);
    memcpy(t.verts[0].transformedPosition, tVert0Pos, sizeof(Vec3));
    memcpy(t.verts[1].transformedPosition, tVert1Pos, sizeof(Vec3));
    memcpy(t.verts[2].transformedPosition, tVert2Pos, sizeof(Vec3));

    memcpy(t.position, pos, sizeof(Vec3));
    }
  return t;
}


void freeTriangle(Triangle* triangle)
{
  if(triangle){
    free(triangle);
  }

  return;
}

inline void barycentricCoords(Vec3 out, Vec3 vert0, Vec3 vert1, Vec3 vert2, Vec3 point)
{
  ///calculate barycentric coords in the triangle
  //  float* vert0 = t->verts[0].position;
  //  float* vert1 = t->verts[1].position;
  //  float* vert2 = t->verts[2].position;
 #ifdef ALTIVEC
  vector float leftSub = { vert1[1], vert0[0], vert2[0], vert0[1]};
  vector float rightSub = {vert2[1], vert2[0], vert1[0], vert2[1]};
  vector float wv1LeftSub = {vert1[1], point[0], vert2[0], point[1]};
  vector float wv1RightSub = {vert2[1], vert2[0], vert1[0], vert2[1]};
  vector float wv2LeftSub = {vert2[1], point[0], vert0[0], point[1]};
  vector float wv2RightSub = {vert0[1], vert2[0], vert2[0], vert2[1]};
  vector float subed = vec_sub(leftSub, rightSub);
  vector float wv1Subed = vec_sub(wv1LeftSub, wv1RightSub);
  vector float wv2Subed = vec_sub(wv2LeftSub, wv2RightSub);
  float leftAdd = subed[0] * subed[1]; float rightAdd = subed[2] * subed[3];
  float wv1LeftAdd = wv1Subed[0] * wv1Subed[1]; float wv1RightAdd = wv1Subed[2] * wv1Subed[3];
  float wv2LeftAdd = wv2Subed[0] * wv2Subed[1]; float wv2RightAdd = wv2Subed[2] * wv2Subed[3];
  float denominator = leftAdd + rightAdd;
  out[0] = (wv1LeftAdd + wv1RightAdd)/denominator;
  out[1] = (wv2LeftAdd + wv2RightAdd)/denominator;
  out[2] = 1 - out[0] - out[1];
  #else
  out[0] = (((vert1[1] - vert2[1]) * (point[0] - vert2[0])) + ((vert2[0] - vert1[0]) * (point[1] - vert2[1]))) / (((vert1[1] - vert2[1]) * (vert0[0] - vert2[0])) + ((vert2[0] - vert1[0]) * (vert0[1] - vert2[1])));
  out[1] = (((vert2[1] - vert0[1]) * (point[0] - vert2[0])) + ((vert0[0] - vert2[0]) * (point[1] - vert2[1]))) / (((vert1[1] - vert2[1]) * (vert0[0] - vert2[0])) + ((vert2[0] - vert1[0]) * (vert0[1] - vert2[1])));
  out[2] = 1 - out[0] - out[1];
  #endif

  ///This is the algorithm to provide the barycentric coords
  //  float wv1 = (((v1pos.y - v2pos.y) * (hitpoint.x - v2pos.x)) + ((v2pos.x - v1pos.x) * (hitpoint.y - v2pos.y))) / (((v1pos.y - v2pos.y) * (v0pos.x - v2pos.x)) + ((v2pos.x - v1pos.x) * (v0pos.y - v2pos.y)));
  //  float wv2 = (((v2pos.y - v0pos.y) * (hitpoint.x - v2pos.x)) + ((v0pos.x - v2pos.x) * (hitpoint.y - v2pos.y))) / (((v1pos.y - v2pos.y) * (v0pos.x - v2pos.x)) + ((v2pos.x - v1pos.x) * (v0pos.y - v2pos.y)));
  //  float wv3 = 1 - wv1 - wv2;
}

int triangleIntersect(Vec3 v0, Vec3 v1, Vec3 v2, Ray* ray, Vec3 intersectionPoint)
{
  const float EPSILON = 0.000001;
  intersectionPoint[0] = 0; intersectionPoint[1] = 0; intersectionPoint[2] = 0;
  Vec3  h, q;
  float a, f, u, v;

  Vec3 edge1 = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]};
  Vec3 edge2 = {v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]};

  cross(ray->direction, edge2, h);
  a = dot(edge1, h);
  if(a > -EPSILON && a < EPSILON)
    return 0;

  f = 1.0 / a;
  //  Vec3 s = {ray->origin[0] - v0[0], ray->origin[1] - v0[1], ray->origin[2] - v0[2]};
  Vec3 s;
  vec3Sub(ray->origin, v0, s);
  u = f * dot(s,h);
  if(u<0.0 || u >1.0)
    return 0;

  cross(s, edge1, q);
  v = f * dot(ray->direction, q);
  if(v<0.0 || u + v > 1.0)
    return 0;
  ray->distance = f * dot(edge2, q);

  Vec3 scaled;
  vec3ScalarMult(ray->direction, ray->distance, scaled);
  vec3Add(ray->origin, scaled, intersectionPoint);

  return 1;
}
