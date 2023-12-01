#include "../include/Mesh.h"

Mesh meshGen(unsigned int numGeometries)
{

    Mesh m;
    //allocate space
    m.nodeToGeomBkPtr = malloc(numGeometries * sizeof(BvhNode*));
    m.geometries = malloc(numGeometries * sizeof(Geometry));
    m.numGeometries = 0; m.maxGeometries = 0;
   return m;
}

unsigned int meshGenGeometry(Mesh* mesh, unsigned int numTriangles, Vec3 pos)
{
    if(mesh)
    {
        //check if we have space for the geom
         //check we have enough space for more triangles
        if(!(mesh->numGeometries < mesh->maxGeometries))
        {
            //need to make more space
            unsigned int moreToAdd = (mesh->maxGeometries / 5) + 1;//how many additional entries to allocate. make sure we will always allocate at least one more space
            Geometry* geomTemp = malloc((mesh->maxGeometries + moreToAdd) * sizeof(Geometry));
            if(geomTemp)
            {
                //copy the old buffers contents to the new buffers
                memcpy(geomTemp, mesh->geometries, (mesh->maxGeometries + moreToAdd) * sizeof(Geometry));
                //clear the old buffer
                free(mesh->geometries);
                //swap the pointers to the new buffers
                mesh->geometries = geomTemp;
                //update the max tris ctr
                mesh->maxGeometries += moreToAdd;
            }
            else
            {
                #ifdef DEBUG
                printf("Error: unable to allocate space for geometries\n");
                #endif
                return 0;
            }
        }
        //add the geometry to the mesh
        mesh->geometries[mesh->numGeometries] = genGeometry(numTriangles, pos);
        mesh->nodeToGeomBkPtr[mesh->numGeometries] = NULL;
        mesh->numGeometries++;
        return 1;
    }
    #ifdef DEBUG
    printf("Error: Invalid mesh to add Geometry to.\n");
    #endif
    return 0;
}

unsigned int meshGeomAddTri(Mesh* mesh, unsigned int geomIdx, Triangle triangle)
{
    if(mesh)
    {
        if(geomIdx <= mesh->numGeometries)
            return geomAddTriangle(&mesh->geometries[geomIdx], triangle);
    }

    return 0;
}

void meshInsertToBvh(Mesh* mesh, BvhNode* bvh)
{
    if(mesh && bvh)
    {
        //pNode = bvhAddGeometry(rootNode, &g1);
        for(unsigned int idx = 0; idx < mesh->numGeometries; idx++)
            mesh->nodeToGeomBkPtr[idx] = bvhAddGeometry(bvh, &mesh->geometries[idx]);
    }
}

Geometry* meshGetGeometry(Mesh* mesh, unsigned int meshIdx)
{
    if(mesh)
    {
        if(meshIdx <= mesh->numGeometries)
            return &mesh->geometries[meshIdx];
    }
    return NULL;
}

BvhNode* mGetBvhPtr(Mesh* mesh, unsigned int meshIdx)
{
    if(mesh)
    {
        if(meshIdx <= mesh->numGeometries)
            return mesh->nodeToGeomBkPtr[meshIdx];
    }
    return NULL;
}