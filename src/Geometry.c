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


Triangle* geomGetTriangle(Geometry* geometry, unsigned int index)
{
    if(index <= geometry->numTriangles)
        return &geometry->triangles[index];

    return NULL;
}

BBox* geomGenAABB(Geometry* geometry)
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

void GeometryFree(Geometry* geom)
{
    if(geom)
    {
        if(geom->numTriangles > 0){
	      for(unsigned int i = 0; i < geom->numTriangles; i++)
	        freeTriangle(geom->triangles);
        }
    }
}
