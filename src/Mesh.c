#include "../include/Mesh.h"

Mesh meshGen(unsigned int numGeometries)
{

    Mesh m;
    //allocate space
    m.nodeToGeomBkPtr = malloc(numGeometries * sizeof(BvhNode*));
    m.geometries = malloc(numGeometries * sizeof(Geometry));
    m.numGeometries = 0; m.maxGeometries = numGeometries;
   return m;
}

M* mGen(unsigned int numGeometries)
{

    M* m = malloc(sizeof(M));
    //allocate space
    m->nodeToGeomBkPtr = malloc(numGeometries * sizeof(BvhNode*));
    m->geometries = malloc(numGeometries * sizeof(Geometry));
    m->numGeometries = 0; m->maxGeometries = numGeometries;
   return m;
}

inline unsigned int meshGenGeometry(Mesh* mesh, unsigned int numTriangles, Vec3 pos)
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

inline unsigned int meshGeomAddTri(Mesh* mesh, unsigned int geomIdx, Triangle triangle)
{
    if(mesh)
    {
        if(geomIdx <= mesh->numGeometries)
            return geomAddTriangle(&mesh->geometries[geomIdx], triangle);
    }

    return 0;
}

inline M* meshLoadOBJ(const char *filePath)
{
    fastObjMesh* loadMesh = fast_obj_read(filePath);
    if(!loadMesh)
    {
        #ifdef DEBUG
        printf("Error loading mesh from file. Check the file path is correct\n");
        #endif
        return 0;
    }

    ///parse the file into objects
    unsigned int numObjects = loadMesh->object_count;

    
    M* mesh = mGen(loadMesh->object_count);
    if(!mesh)
    {
        #ifdef DEBUG
        printf("Error initialising mesh\n");
        #endif
        return 0;
    }


    //copy the vertex positions
    unsigned int posCount = loadMesh->position_count - 1;
    mesh->positions = malloc(posCount * sizeof(Vec3));
    memcpy(mesh->positions, (loadMesh->positions) + 3, posCount * sizeof(Vec3));

    //copy the vertex normals
    unsigned int normCount = loadMesh->normal_count - 1;
    mesh->normals = malloc(normCount * sizeof(Vec3));
    memcpy(mesh->normals, (loadMesh->normals + 3), normCount * sizeof(Vec3));

    //copy the tex cords
    unsigned int texCordCount = loadMesh->texcoord_count - 1;
    mesh->texCords = malloc(texCordCount * sizeof(Vec2));
    memcpy(mesh->texCords, (loadMesh->texcoords + 2), texCordCount * sizeof(Vec2));

    //parse the geometries out
    for(unsigned int objectID = 0; objectID < numObjects; objectID++)
    {
        fastObjGroup* pObj = &loadMesh->objects[objectID];
        G* g = &mesh->geometries[objectID];

        //initialise the geometry
        printf("Parsing object %u: %s\n", objectID, pObj->name);
        unsigned int numFaces  = pObj->face_count; //Number of faces
        unsigned int faceOffset = pObj->face_offset; //First face in fastObjMesh face_* arrays
        unsigned int indexOffset = pObj->index_offset; //First index in fastObjMesh indices array
        unsigned int numFaceVerts = loadMesh->face_vertices[faceOffset];//get the number of verts per face/tri

        unsigned int numTris = 0;
        if(numFaceVerts == 3)
            numTris = numFaces;
        else
            numTris = numFaces * 2;//assume a face is a quad, it's composed of 2 triangles

        Ginit(&mesh->geometries[objectID], mesh->positions, mesh->normals, mesh->texCords, numTris);//init the geometry
        
        //go through each face and turn them into triangles to be added to g
        for(int faceIndex = 0; faceIndex < numFaces; faceIndex++)
        {
            switch(numFaceVerts)
            {
            case 3:
            {
                printf("3\n");
                T tri;
                //iterate through the vertices of the face. 3 verts per face (the face is a triangle)
                for(unsigned int faceVertexId = 0; faceVertexId < 3; faceVertexId++)
                {
                    fastObjIndex* vert = &loadMesh->indices[indexOffset + (faceIndex * numFaceVerts) + faceVertexId];
                    unsigned int vertPIndex = vert->p;
                    unsigned int vertTIndex = vert->t;
                    unsigned int vertNIndex = vert->n;
                    tri.vertIndex[faceVertexId] = (vertPIndex * FLOATS_PER_POS);
                    tri.textureIndex[faceVertexId] = (vertTIndex * FLOATS_PER_TEX);
                    tri.normalIndex[faceVertexId] = (vertNIndex * FLOATS_PER_NORM);
                }
                GAddTriangle(&mesh->geometries[objectID], &tri);
                break;
            }
            case 4:
            {
                //iterate through the vertices of the face. 4 verts per face (the face is a square)
                T t1, t2;

                //get the 4 verts
                fastObjIndex* vert1 = &loadMesh->indices[indexOffset + (faceIndex * numFaceVerts) + VERT1];
                unsigned int vert1PIndex = vert1->p - 1;// -1 because in the fast_obj lib, the verts are indexed from 1 not 0
                unsigned int vert1TIndex = vert1->t - 1;
                unsigned int vert1NIndex = vert1->n - 1;

                fastObjIndex* vert2 = &loadMesh->indices[indexOffset + (faceIndex * numFaceVerts) + VERT2];
                unsigned int vert2PIndex = vert2->p - 1;
                unsigned int vert2TIndex = vert2->t - 1;
                unsigned int vert2NIndex = vert2->n - 1;

                fastObjIndex* vert3 = &loadMesh->indices[indexOffset + (faceIndex * numFaceVerts) + VERT3];
                unsigned int vert3PIndex = vert3->p - 1;
                unsigned int vert3TIndex = vert3->t - 1;
                unsigned int vert3NIndex = vert3->n - 1;

                fastObjIndex* vert4 = &loadMesh->indices[indexOffset + (faceIndex * numFaceVerts) + VERT4];
                unsigned int vert4PIndex = vert4->p - 1;
                unsigned int vert4TIndex = vert4->t - 1;
                unsigned int vert4NIndex = vert4->n - 1;

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
                //add the tris to the geom
                GAddTriangle(&mesh->geometries[objectID], &t1);
                GAddTriangle(&mesh->geometries[objectID], &t2);
                break;
            }
            default:
            {
                #ifdef DEBUG
                printf("Error loading geometry. Unsupported number of vertices per face\n");
                #endif
                return 0;
            }
            }
        }
        //Add the geom to the mesh
        meshAddGeom(mesh, g);
    }
    return mesh;  
}

unsigned int meshAddGeom(M* mesh, G* geometry)
{
    if(mesh && geometry)
    {
        //check we have enough space for more geoms
        if(!(mesh->numGeometries < mesh->maxGeometries))
        {
            //need to make more space
            unsigned int moreToAdd = (mesh->maxGeometries / 5) + 1;//how many additional entries to allocate. make sure we will always allocate at least one more space
            unsigned int numToAllocate = mesh->maxGeometries + moreToAdd;
            G* gTemp = malloc(numToAllocate * sizeof(G));//allocate new geom buffer

            if(gTemp)
            {
                //copy the old buffers contents to the new buffers
                memcpy(gTemp, mesh->geometries, numToAllocate * sizeof(G));
                //clear the old buffer
                free(mesh->geometries);
                //swap the pointers to the new buffers
                mesh->geometries = gTemp;
                //update the max tris ctr
                mesh->maxGeometries += moreToAdd;
            }
            else
            {
                #ifdef DEBUG
                printf("Error: unable to allocate space for triangles.\n");
                #endif
                return 0;
            }
        }
        //Add the new geom to the mesh
        mesh->geometries[mesh->numGeometries].triangles = geometry->triangles;
        mesh->geometries[mesh->numGeometries].maxTriangles = geometry->maxTriangles;
        mesh->geometries[mesh->numGeometries].numTriangles = geometry->numTriangles;
        mesh->geometries[mesh->numGeometries].positions = geometry->positions;
        mesh->geometries[mesh->numGeometries].normals = geometry->normals;
        mesh->geometries[mesh->numGeometries].texCords = geometry->texCords;
        mesh->numGeometries++;
        return 1;
    }
    else
    {
        #ifdef DEBUG
        printf("Error adding geometry to mesh. Uninitialised mesh or geom supplied.");
        #endif
        return 0;
    }
}

inline void meshInsertToBvh(Mesh* mesh, BvhNode* bvh)
{
    if(mesh && bvh)
    {
        for(unsigned int idx = 0; idx < mesh->numGeometries; idx++)
            mesh->nodeToGeomBkPtr[idx] = bvhAddGeometry(bvh, &mesh->geometries[idx]);
    }
}

inline Geometry* meshGetGeometry(Mesh* mesh, unsigned int meshIdx)
{
    if(mesh)
    {
        if(meshIdx <= mesh->numGeometries)
            return &mesh->geometries[meshIdx];
    }
    return NULL;
}

inline BvhNode* mGetBvhPtr(Mesh* mesh, unsigned int meshIdx)
{
    if(mesh)
    {
        if(meshIdx <= mesh->numGeometries)
            return mesh->nodeToGeomBkPtr[meshIdx];
    }
    return NULL;
}