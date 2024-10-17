#include <stdio.h>
#include <stdint.h>
#include "3DRenderer.h"
#include "OBJReader.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("ERROR: EmbeddedOBJConverter {obj filepath} {output file} {model name}\n");
        return -1;
    }

    ModelData data = ReadOBJ(argv[1]);
    FILE *f = fopen(argv[2], "w");

    fprintf(f, "#include <stdint.h>\n"
               "#include \"3DRenderer.h\"\n"
               "Vec3f %s_verts[] = {\n", argv[3]);
    for (uint32_t i = 0; i < data.numVerts; i++) {
        fprintf(f, "{%e, %e, %e},", data.verts[i].x, data.verts[i].y, data.verts[i].z); 
    }

    fprintf(f, "\n};\n"
               "uint32_t %s_primatives[] = {\n", argv[3]);
    for (uint32_t i = 0; i < data.primativesSize; i++) {
        fprintf(f, "%d,", data.primatives[i]); 
    } 
    fprintf(f, "\n};\n"
               "ModelData %s = {%d, %s_verts, %d, %d, %s_primatives, {0}, {0}, {1, 1, 1}};",
               argv[3], data.numVerts, argv[3], data.numPrimatives, data.primativesSize, argv[3]);

    fclose(f);
    return 0;
}