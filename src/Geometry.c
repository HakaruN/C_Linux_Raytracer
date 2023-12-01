#include "../include/Geometry.h"

Geometry genGeometry(unsigned int numTriangles, Vec3 position)
{
    Geometry g;
    //allocate space for the triangles
    g.triangles = malloc(numTriangles * sizeof(Triangle));
    g.backPtrs = malloc(numTriangles * sizeof(BvhNode*));

    //set the triangle ranges
    g.numTriangles = 0; g.maxTriangles = numTriangles;

    //set the back ptrs to NULL
    memset(g.backPtrs, (int)NULL, numTriangles * sizeof(BvhNode**));//NULL out the back pointer buffer

    //copy the geometrys position to the geom
    memcpy(g.position, position, sizeof(Vec3));

    //set the unique geomID for the geom and increment the global ctr
    g.geometryID = geomCtr; geomCtr++;

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
            BvhNode** bkPtTemp = malloc((geometry->maxTriangles + moreToAdd) * sizeof(BvhNode*));

            if(triTemp && bkPtTemp)
            {
                //copy the old buffers contents to the new buffers
                memcpy(triTemp, geometry->triangles, (geometry->maxTriangles + moreToAdd) * sizeof(Triangle));
                memset(bkPtTemp, (int)NULL, (geometry->maxTriangles + moreToAdd) * sizeof(BvhNode*));//NULL out the back pointer buffer
                memcpy(bkPtTemp, geometry->backPtrs, (geometry->maxTriangles + moreToAdd) * sizeof(BvhNode*));//copy the back pointer buffer up to the current num tris. This mean all the new spaces are still nulled out
                //clear the old buffer
                free(geometry->triangles); free(geometry->backPtrs);
                //swap the pointers to the new buffers
                geometry->triangles = triTemp; geometry->backPtrs = bkPtTemp;
                //update the max tris ctr
                geometry->maxTriangles += moreToAdd;
            }
            else
            {
                #ifdef DEBUG
                printf("Error: unable to allocate space for triangles and/or back pointers\n");
                #endif
                return 0;
            }
        }

        //add the triangle and increment the counter
        geometry->triangles[geometry->numTriangles] = triangle;
        geometry->backPtrs[geometry->numTriangles] = NULL;//shouldn't need this anyway because the bckPtr buffer is always nulled out                  
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

unsigned int geomSetBackPtr(Geometry* geometry, unsigned int index, BvhNode* backPtr)
{
    if(geometry && backPtr)
    {
        if(index <= geometry->numTriangles)
        {
            geometry->backPtrs[index] = backPtr;
        }
    }
    return 0;
}


//Geom Ctr
unsigned long getGeomCtr(void)
{
    return geomCtr;
}
void initGeomCtr(unsigned long ctrVal)
{
    geomCtr = ctrVal;
}