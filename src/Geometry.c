#include "../include/Geometry.h"

Geometry genGeometry(unsigned int numTriangles, Vec3 position)
{
    Geometry g;
    //allocate space for the triangles
    g.triangles = malloc(numTriangles * sizeof(Triangle));
    //g.backPtr = NULL;

    //set the triangle ranges
    g.numTriangles = 0; g.maxTriangles = numTriangles;

    //copy the geometrys position to the geom
    memcpy(g.position, position, sizeof(Vec3));

    //set the unique geomID for the geom and increment the global ctr
    //g.geometryID = geomCtr; geomCtr++;

    return g;
}

G* genG(Vec3 *pPositions, Vec3 *pNormals, Vec2 *pTexCords, unsigned int maxTriangles)
{
    if(pPositions && pNormals && pTexCords)
    {
        G* g = malloc(sizeof(G));
        if(g)
        {
            g->positions = pPositions;
            g->normals = pNormals;
            g->texCords = pTexCords;
            g->triangles = malloc(maxTriangles * sizeof(T));
            if(!g->triangles)
            {
                #ifdef DEBUG
                printf("Error triangles failed to allocate\n");
                #endif
                free(g);
                return 0;
            }
            g->maxTriangles = maxTriangles;
            g->numTriangles = 0;
        }
        #ifdef DEBUG
        printf("Error allocating geometry. Uninitialised buffers supplied\n");
        #endif
        return 0;
    }
    #ifdef DEBUG
    printf("Error geometry failed to allocate\n");
    #endif
    return 0;

}

unsigned int Ginit(G* geom, Vec3 *pPositions, Vec3 *pNormals, Vec2 *pTexCords, unsigned int maxTriangles)//when we already have a G allocated
{
    if(pPositions && pNormals && pTexCords && geom)
    {
        geom->positions = pPositions;
        geom->normals = pNormals;
        geom->texCords = pTexCords;
        geom->triangles = malloc(maxTriangles * sizeof(T));
        if(!geom->triangles)
        {
            #ifdef DEBUG
            printf("Error triangles failed to allocate\n");
            #endif
            return 0;
        }
        geom->maxTriangles = maxTriangles;
        geom->numTriangles = 0;
        return 1;
    }
    #ifdef DEBUG
    printf("Error allocating geometry. Uninitialised buffers supplied\n");
    #endif
    return 0;
}

unsigned int geomAddTriangle(Geometry* geometry, Triangle triangle)
{
    if(geometry)
    {
        
        //check we have enough space for more triangles
        if(!(geometry->numTriangles < geometry->maxTriangles))
        {
            //need to make more space
            unsigned int moreToAdd = (geometry->maxTriangles / 5) + 1;//how many additional entries to allocate. make sure we will always allocate at least one more space
            Triangle* triTemp = malloc((geometry->maxTriangles + moreToAdd) * sizeof(Triangle));

            if(triTemp)
            {
                //copy the old buffers contents to the new buffers
                memcpy(triTemp, geometry->triangles, (geometry->maxTriangles + moreToAdd) * sizeof(Triangle));
                //clear the old buffer
                free(geometry->triangles);
                //swap the pointers to the new buffers
                geometry->triangles = triTemp;
                //update the max tris ctr
                geometry->maxTriangles += moreToAdd;
            }
            else
            {
                #ifdef DEBUG
                printf("Error: unable to allocate space for triangles.\n");
                #endif
                return 0;
            }
        }

        //add the triangle and increment the counter
        geometry->triangles[geometry->numTriangles] = triangle;       
        geometry->numTriangles++;
        return 1;
    }
    #ifdef DEBUG
    printf("Error: Invalid geometry to add triangle to\n");
    #endif
    return 0;
}

unsigned int GAddTriangle(G* geometry, T* triangle)
{
    if(geometry)
    {
        
        //check we have enough space for more triangles
        if(!(geometry->numTriangles < geometry->maxTriangles))
        {
            //need to make more space
            unsigned int moreToAdd = (geometry->maxTriangles / 5) + 1;//how many additional entries to allocate. make sure we will always allocate at least one more space
            unsigned int numToAllocate = geometry->maxTriangles + moreToAdd;
            T* triTemp = malloc(numToAllocate * sizeof(T));//allocate new tri buffer

            if(triTemp)
            {
                //copy the old buffers contents to the new buffers
                memcpy(triTemp, geometry->triangles, numToAllocate * sizeof(T));
                //clear the old buffer
                free(geometry->triangles);
                //swap the pointers to the new buffers
                geometry->triangles = triTemp;
                //update the max tris ctr
                geometry->maxTriangles += moreToAdd;
            }
            else
            {
                #ifdef DEBUG
                printf("Error: unable to allocate space for triangles.\n");
                #endif
                return 0;
            }
        }

        ///add the triangle and increment the counter
        //vertex indices
        geometry->triangles[geometry->numTriangles].vertIndex[0] = triangle->vertIndex[0];
        geometry->triangles[geometry->numTriangles].vertIndex[1] = triangle->vertIndex[1];
        geometry->triangles[geometry->numTriangles].vertIndex[2] = triangle->vertIndex[2];
        geometry->triangles[geometry->numTriangles].normalIndex[0] = triangle->normalIndex[0];
        geometry->triangles[geometry->numTriangles].normalIndex[1] = triangle->normalIndex[1];
        geometry->triangles[geometry->numTriangles].normalIndex[2] = triangle->normalIndex[2];
        geometry->triangles[geometry->numTriangles].textureIndex[0] = triangle->textureIndex[0];
        geometry->triangles[geometry->numTriangles].textureIndex[1] = triangle->textureIndex[1];
        geometry->triangles[geometry->numTriangles].textureIndex[2] = triangle->textureIndex[2];

        //copy the positions
        memcpy(geometry->triangles[geometry->numTriangles].vertPosition[0], triangle->vertPosition[0], sizeof(Vec3));
        memcpy(geometry->triangles[geometry->numTriangles].vertPosition[1], triangle->vertPosition[1], sizeof(Vec3));
        memcpy(geometry->triangles[geometry->numTriangles].vertPosition[2], triangle->vertPosition[2], sizeof(Vec3));
        //copy the normals
        memcpy(geometry->triangles[geometry->numTriangles].vertNormal[0], triangle->vertNormal[0], sizeof(Vec3));
        memcpy(geometry->triangles[geometry->numTriangles].vertNormal[1], triangle->vertNormal[1], sizeof(Vec3));
        memcpy(geometry->triangles[geometry->numTriangles].vertNormal[2], triangle->vertNormal[2], sizeof(Vec3));
        //copy the tex cords
        memcpy(geometry->triangles[geometry->numTriangles].vertTexture[0], triangle->vertTexture[0], sizeof(Vec2));
        memcpy(geometry->triangles[geometry->numTriangles].vertTexture[1], triangle->vertTexture[1], sizeof(Vec2));
        memcpy(geometry->triangles[geometry->numTriangles].vertTexture[2], triangle->vertTexture[2], sizeof(Vec2));
        
        geometry->numTriangles++;
        return 1;
    }
    #ifdef DEBUG
    printf("Error: Invalid geometry to add triangle to\n");
    #endif
    return 0;
}



inline Triangle* geomGetTriangle(Geometry* geometry, unsigned int index)
{
    if(index <= geometry->numTriangles)
        return &geometry->triangles[index];

    return NULL;
}
/*
inline BBox* geomGenAABB(Geometry* geometry)
{ 
    if(geometry)
    {
        BBox* b = malloc(sizeof(BBox));
        b->min[0] = 0; b->min[1] = 0; b->min[2] = 0;
        b->max[0] = 0; b->max[1] = 0; b->max[2] = 0;
        for(unsigned int t = 0; t < geometry->numTriangles; t++)//iterate through each tri
        {
            for(unsigned int v = 0; v < 3; v++)
            {
                //find the max x
                b->max[0] = geometry->triangles[t].verts[v].transformedPosition[0] > b->max[0] ? geometry->triangles[t].verts[v].transformedPosition[0] : b->max[0];
                b->min[0] = geometry->triangles[t].verts[v].transformedPosition[0] < b->min[0] ? geometry->triangles[t].verts[v].transformedPosition[0] : b->min[0];
                //find the max y
                b->max[1] = geometry->triangles[t].verts[v].transformedPosition[1] > b->max[1] ? geometry->triangles[t].verts[v].transformedPosition[1] : b->max[1];
                b->min[1] = geometry->triangles[t].verts[v].transformedPosition[1] < b->min[1] ? geometry->triangles[t].verts[v].transformedPosition[1] : b->min[1];
                //find the max z
                b->max[2] = geometry->triangles[t].verts[v].transformedPosition[2] > b->max[2] ? geometry->triangles[t].verts[v].transformedPosition[2] : b->max[2];
                b->min[2] = geometry->triangles[t].verts[v].transformedPosition[2] < b->min[2] ? geometry->triangles[t].verts[v].transformedPosition[2] : b->min[2];
            }
        }
        #ifdef DEBUG
        printf("AABB min: ");
        printVec3(b->min);
        printf("AABB max: ");
        printVec3(b->max);
        #endif
        return b;
    }
    return NULL;
}
*/

inline BBox* geomGenAABB(G* geometry)
{
    if(geometry)
    {
        BBox* b = malloc(sizeof(BBox));
        b->min[0] = 0; b->min[1] = 0; b->min[2] = 0;
        b->max[0] = 0; b->max[1] = 0; b->max[2] = 0;
        for(unsigned int t = 0; t < geometry->numTriangles; t++)//iterate through each tri
        {
            T* triangle = &geometry->triangles[t];
            Vec3* positions = geometry->positions;
            for(unsigned int v = 0; v < 3; v++)
            {
                //find the max x
                Vec3 vertPos;
                vertPos[X] = positions[triangle->vertIndex[v]][X];
                vertPos[Y] = positions[triangle->vertIndex[v]][Y];
                vertPos[Z] = positions[triangle->vertIndex[v]][Z];
                b->max[0] = vertPos[X] > b->max[0] ? vertPos[X] : b->max[0];
                b->min[0] = vertPos[X] < b->min[0] ? vertPos[X] : b->min[0];
                //find the max y
                b->max[1] = vertPos[Y] > b->max[1] ? vertPos[Y] : b->max[1];
                b->min[1] = vertPos[Y] < b->min[1] ? vertPos[Y] : b->min[1];
                //find the max z
                b->max[2] = vertPos[Z] > b->max[2] ? vertPos[Z] : b->max[2];
                b->min[2] = vertPos[Z] < b->min[2] ? vertPos[Z] : b->min[2];
            }
        }
        #ifdef DEBUG
        printf("AABB min: ");
        printVec3(b->min);
        printf("AABB max: ");
        printVec3(b->max);
        #endif
        return b;
    }
    return NULL;
}


void GeometryFree(G* geometry)
{
    if(geometry)
    {
        if(geometry->positions)
            free(geometry->positions);
        if(geometry->normals)
            free(geometry->normals);
        if(geometry->texCords)
            free(geometry->texCords);
        if(geometry->triangles)
            free(geometry->triangles);
        
    }
}
