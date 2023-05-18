#include "../include/Vertex.h"


Vertex vertexGen(Vec3 position, Vec3 normal, Vec3 colour, Vec2 texCords)
{
  Vertex v;

    memcpy(v.position, position, sizeof(Vec3));
    memcpy(v.normal, normal, sizeof(Vec3));
    memcpy(v.colour, colour, sizeof(Vec3));
    memcpy(v.texCords, texCords, sizeof(Vec2));
  return v;
}
