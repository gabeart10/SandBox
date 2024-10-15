#include <stdio.h>
#include <stdlib.h>
#include "OBJReader.h"

ModelData ReadOBJ(const char * const fileName) {
    ModelData obj_data = DEFAULT_MODEL;
    FILE *f = fopen(fileName, "r");
    if (f == NULL) {
        printf("OBJReader: Failed to open file...\n");
        return obj_data;
    }

    // Get count of types for buffers
    char line[128];
    while (fgets(line, sizeof(line), f) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            obj_data.numVerts++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            obj_data.numPrimatives++;
        }
    }

    // Build buffs and read file data
    obj_data.verts = malloc(sizeof(Vec3f)*obj_data.numVerts);
    obj_data.primatives = malloc(sizeof(uint32_t)*4*obj_data.numPrimatives);
    uint32_t curr_vert = 0;
    uint32_t curr_face = 0;
    rewind(f);
    while (fgets(line, sizeof(line), f) != NULL) {
        if (line[0] == 'v' && line[1] == ' ') {
            if (sscanf(line + 2, "%f %f %f",
                &obj_data.verts[curr_vert].x,
                &obj_data.verts[curr_vert].y,
                &obj_data.verts[curr_vert].z) < 3) {
                printf("OBJReader: Error Reading Vert %d...\n", curr_vert); 
            }
            curr_vert++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            // TODO: Make parsing work for all types of f lines
            uint32_t *face_loc = obj_data.primatives+(curr_face*4);
            *face_loc = PRIMATIVE_TRIANGLE;
            if (sscanf(line + 2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",
                face_loc+1,
                face_loc+2,
                face_loc+3) < 3) {
                printf("OBJReader: Error Reading Face %d...\n", curr_face); 
            }
            face_loc[1]--;
            face_loc[2]--;
            face_loc[3]--;
            curr_face++;
        }
    }

    fclose(f);
    printf("OBJReader: Loaded OBJ with %d verts and %d faces...\n", obj_data.numVerts, obj_data.numPrimatives);
    return obj_data;
}