#include <stdio.h>
#include <stdlib.h>
#include "OBJReader.h"

OBJData ReadOBJ(const char * const fileName) {
    OBJData obj_data = {.numFaces = 0, .numVerts = 0,
                        .faces = NULL, .verts = NULL};
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
            obj_data.numFaces++;
        }
    }

    // Build buffs and read file data
    obj_data.verts = malloc(sizeof(Vec3f)*obj_data.numVerts);
    obj_data.faces = malloc(sizeof(OBJFace)*obj_data.numFaces);
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
            if (sscanf(line + 2, "%d/%*d/%*d %d/%*d/%*d %d/%*d/%*d",
                obj_data.faces[curr_face].vertIdxs,
                obj_data.faces[curr_face].vertIdxs+1,
                obj_data.faces[curr_face].vertIdxs+2) < 3) {
                printf("OBJReader: Error Reading Face %d...\n", curr_face); 
            }
            obj_data.faces[curr_face].vertIdxs[0]--;
            obj_data.faces[curr_face].vertIdxs[1]--;
            obj_data.faces[curr_face].vertIdxs[2]--;
            curr_face++;
        }
    }

    fclose(f);
    printf("OBJReader: Loaded OBJ with %d verts and %d faces...\n", obj_data.numVerts, obj_data.numFaces);
    return obj_data;
}