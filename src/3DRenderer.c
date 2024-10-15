#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "3DRenderer.h"
#include "OBJReader.h"
#include "Matrix.h"

#define swap(x, y) do { typeof(x) SWAP = x; x = y; y = SWAP; } while (0)

#define INVALID_SCREEN_VERT ((Vec3i) {-1, -1, 0})

static inline int32_t max(int32_t a, int32_t b) {
    return (a > b) ? a : b;
}

static inline int32_t min(int32_t a, int32_t b) {
    return (a < b) ? a : b;
}

static Vec3f GetBarycentric(Vec3i a, Vec3i b, Vec3i c, Vec2i p);

static TformMatrix LookAtTform(Vec3f eye, Vec3f center, Vec3f up);
static TformMatrix ViewportTform(int32_t x, int32_t y, int32_t w, int32_t h);
static TformMatrix PerspectiveTform(float fov, float aspect, float zNear, float zFar);
static TformMatrix ScaleTform(Vec3f s);
static TformMatrix TranslateTform(Vec3f t);
static TformMatrix RotationXTform(float rot);
static TformMatrix RotationYTform(float rot);
static TformMatrix RotationZTform(float rot);

View * CreateView(uint32_t width, uint32_t height) {
    View *v = (View *) malloc(sizeof(View));
    v->w = width;
    v->h = height;
    v->look_pos = (Vec3f) {0, 0, 0};
    v->eye_pos = (Vec3f) {0, 0, 1};
    v->framebuf = (RGBData *) malloc(sizeof(RGBData) * width * height);
    v->zbuffer = (uint32_t *) malloc(sizeof(uint32_t) * width * height);

    ClearViewBuffers(v);

    return v;
}

void ClearViewBuffers(View *v) {
    memset(v->framebuf, 0, v->w * v->h * sizeof(RGBData));
    for (uint32_t i = 0; i < v->w * v->h; i++) {
        v->zbuffer[i] = INT32_MAX;
    }
}

void RenderModel(View *views, uint32_t view_count, ModelData m) {
    // Apply Model to World Transformations
    Vec3f *world_verts = (Vec3f *) malloc(sizeof(Vec3f) * m.numVerts);
    TformMatrix world_tform = ScaleTform(m.scale);
    world_tform = TformMatrix_Multiply(RotationXTform(m.world_rot.x), world_tform);
    world_tform = TformMatrix_Multiply(RotationYTform(m.world_rot.y), world_tform);
    world_tform = TformMatrix_Multiply(RotationZTform(m.world_rot.z), world_tform);
    world_tform = TformMatrix_Multiply(TranslateTform(m.world_loc), world_tform);
    for (uint32_t i = 0; i < m.numVerts; i++) {
        world_verts[i] = TformPoint_to_Vec3f(TformMatrix_Apply(world_tform, Vec3f_to_TformPoint(m.verts[i])));
    }

    for (uint32_t i = 0; i < view_count; i++) {
        // Build World to Clip Transformation
        View *v = views+i;
        Vec3i *screen_verts = (Vec3i *) malloc(sizeof(Vec3i) * m.numVerts);
        TformMatrix clip_tform = LookAtTform(v->eye_pos, v->look_pos, (Vec3f) {0, 1, 0});
        clip_tform = TformMatrix_Multiply(PerspectiveTform(v->fov, ((float) v->w)/v->h, 0.5, 200), clip_tform);
        TformMatrix screen_tform = ViewportTform(0, 0, v->w, v->h);

        // Clip verts and convert to screen space
        for (uint32_t i = 0; i < m.numVerts; i++) {
            Vec3f clip_vert = TformPoint_to_Vec3f(TformMatrix_Apply(clip_tform, Vec3f_to_TformPoint(world_verts[i])));
            if (clip_vert.x+1 >= 0 && clip_vert.y+1 >= 0 && clip_vert.z+1 >= 0) {
                Vec3f screen_vert = TformPoint_to_Vec3f(TformMatrix_Apply(screen_tform, Vec3f_to_TformPoint(clip_vert)));
                screen_verts[i] = (Vec3i) {screen_vert.x, screen_vert.y, screen_vert.z};
            } else { 
                screen_verts[i] = INVALID_SCREEN_VERT;
            }
        }

        // Display Primatives
        uint32_t *prim_idx = m.primatives;
        ClearViewBuffers(v);
        for (uint32_t i = 0; i < m.numPrimatives; i++) {
            PrimativeType prim_type = *(prim_idx++);
            bool draw_prim = true;
            for (uint32_t vi = 0; vi < prim_type; vi++) {
                if (screen_verts[prim_idx[vi]].x == -1) {
                    draw_prim = false;
                    break;
                }
            }
            if (!draw_prim) continue;            

            switch (prim_type) {
                case PRIMATIVE_TRIANGLE: {
                    Vec3f n = Vec3f_CrossProduct(Vec3f_Subtract(world_verts[prim_idx[2]], world_verts[prim_idx[0]]),
                                                 Vec3f_Subtract(world_verts[prim_idx[1]], world_verts[prim_idx[0]]));
                    n = Vec3f_Normalize(n);
                    float inten = Vec3f_DotProduct(n, Vec3f_Normalize(Vec3f_Subtract(v->look_pos, v->eye_pos)));
                    if (inten > 0) {
                        uint32_t color = 205*inten + 50;
                        DrawTriangle(v, screen_verts[prim_idx[0]],
                                        screen_verts[prim_idx[1]],
                                        screen_verts[prim_idx[2]],
                                        (RGBData) {color, color, color});
                    }
                } break;

                default: break;
            }

            prim_idx += prim_type;
        }

        free(screen_verts);
    }   

    free(world_verts);
}

void DrawLine(View *v, int32_t x0, int32_t y0, int32_t x1, int32_t y1, RGBData color) {
    bool steep = false;
    if (abs(x0-x1) < abs(y0-y1)) {
        swap(x0, y0);
        swap(x1, y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int dx = x1-x0;
    int dy = y1-y0;
    int derror = abs(dy)*2;
    int error = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            SetPixel(v, y, x, color);
        } else {
            SetPixel(v, x, y, color);
        }
        error += derror;
        if (error > dx) {
            y += (y1 > y0) ? 1 : -1;
            error -= dx*2;
        }
    }
}

static Vec3f GetBarycentric(Vec3i a, Vec3i b, Vec3i c, Vec2i p) {
    Vec3f vx = {b.x-a.x, c.x-a.x, a.x-p.x};
    Vec3f vy = {b.y-a.y, c.y-a.y, a.y-p.y};
    Vec3f u = Vec3f_CrossProduct(vx, vy);

    if (abs(u.z) < 1) {
        return (Vec3f) {-1, 1, 1};
    }
    return (Vec3f) {1.0f-(u.x+u.y)/u.z, u.x/u.z, u.y/u.z};
}

// TODO: Test other forms of rasterization on HW
// Assume Vecs are inside 
void DrawTriangle(View *v, Vec3i v0, Vec3i v1, Vec3i v2, RGBData color) {
    if (v0.y == v1.y && v0.y == v2.y) return;
    if (v0.y > v1.y) swap(v0, v1);
    if (v0.y > v2.y) swap(v0, v2);
    if (v1.y > v2.y) swap(v1, v2);
    int32_t tri_height = v2.y - v0.y;
    for (uint32_t y = v0.y; y <= v2.y; y++) {
        Vec3i s0, s1;
        if (y > v1.y || v1.y == v0.y) {
            // In 2nd Segment 
            s0 = v1;
            s1 = v2;
        } else {
            // In 1st Segment 
            s0 = v0;
            s1 = v1;
        }

        int32_t seg_height = s1.y - s0.y;
        int32_t alpha_num = y - v0.y;
        int32_t beta_num = y - s0.y;
        int32_t Ax = v0.x + ((v2.x - v0.x)*alpha_num)/tri_height;
        int32_t Bx = s0.x + ((s1.x - s0.x)*beta_num)/seg_height;
        int32_t Az = v0.z + ((v2.z - v0.z)*alpha_num)/tri_height;
        int32_t Bz = s0.z + ((s1.z - s0.z)*beta_num)/seg_height;
        if (Ax > Bx) {
            swap(Ax, Bx);
            swap(Az, Bz);
        }

        // Protect for div by 0
        int32_t z_div = (Ax == Bx) ? 1 : (Bx - Ax);
        for (int32_t i = Ax; i <= Bx; i++) {
            int32_t z = Az + ((Bz - Az)*(i - Ax))/z_div;
            //Vec3f bay = GetBarycentric(v0, v1, v2, (Vec2i) {i, y});
            //int32_t z = v0.z*bay.x + v1.z*bay.y + v1.z*bay.z;
            //color = (RGBData) {(uint32_t)z, (uint32_t)z, (uint32_t)z};
            SetPixelZ(v, i, y, z, color);
        }
    }
}
/*void DrawTriangle(View *r, Vec2i v0, Vec2i v1, Vec2i v2, RGBData color) {
    Vec2i boxmin = {max(0, min(v0.x, min(v1.x, min(v2.x, r->w-1)))),
                    max(0, min(v0.y, min(v1.y, min(v2.y, r->h-1))))};
    Vec2i boxmax = {min(r->w-1, max(v0.x, max(v1.x, max(v2.x, 0)))),
                    min(r->h-1, max(v0.y, max(v1.y, max(v2.y, 0))))};
    Vec2i p;
    for (p.x = boxmin.x; p.x <= boxmax.x; p.x++) {
        for (p.y = boxmin.y; p.y <= boxmax.y; p.y++) {
            Vec3f bc_cord = GetBarycentric(v0, v1, v2, p);
            if (bc_cord.x >= 0 && bc_cord.y >= 0 && bc_cord.z >= 0) {
                SetPixel(r, p.x, p.y, color);
            }
        }
    }
}*/

static TformMatrix LookAtTform(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = Vec3f_Normalize(Vec3f_Subtract(eye, center));
    Vec3f x = Vec3f_Normalize(Vec3f_CrossProduct(up, z));
    Vec3f y = Vec3f_Normalize(Vec3f_CrossProduct(z, x));
    TformMatrix Minv = {{{x.x, x.y, x.z, 0},
                         {y.x, y.y, y.z, 0},
                         {z.x, z.y, z.z, 0},
                         {0,   0,   0,   1}}};
    return TformMatrix_Multiply(Minv, TranslateTform(Vec3f_Negate(eye)));
}

static TformMatrix ViewportTform(int32_t x, int32_t y, int32_t w, int32_t h) {
    TformMatrix out = {{{w/2,    0,            0, x + w/2},
                        {  0, -h/2,            0, y + h/2},
                        {  0,    0,    INT32_MAX,      -1},
                        {  0,    0,            0,       1}}};
    return out;
}

static TformMatrix PerspectiveTform(float fov, float aspect, float zNear, float zFar) {
    float f = 1/tanf(fov);
    float zscale = (zFar+zNear)/(zNear-zFar);
    float ztrans = 2*zFar*zNear/(zNear-zFar);
    TformMatrix out = {{{f/aspect, 0,      0,      0},
                        {       0, f,      0,      0},
                        {       0, 0, zscale, ztrans},
                        {       0, 0,     -1,      0}}};
    return out;
}

static TformMatrix ScaleTform(Vec3f s) {
    TformMatrix out = {{{s.x,   0,   0, 0},
                        {  0, s.y,   0, 0},
                        {  0,   0, s.z, 0},
                        {  0,   0,   0, 1}}};
    return out; 
}

static TformMatrix TranslateTform(Vec3f t) {
    TformMatrix out = {{{ 1, 0, 0, t.x},
                        { 0, 1, 0, t.y},
                        { 0, 0, 1, t.z},
                        { 0, 0, 1,   1}}};
    return out; 
}

static TformMatrix RotationXTform(float rot) {
    TformMatrix out = {{{1,         0,          0, 0},
                        {0, cosf(rot), -sinf(rot), 0},
                        {0, sinf(rot),  cosf(rot), 0},
                        {0,         0,          0, 1}}};
    return out;
}

static TformMatrix RotationYTform(float rot) {
    TformMatrix out = {{{ cosf(rot), 0, sinf(rot), 0},
                        {         0, 1,         0, 0},
                        {-sinf(rot), 0, cosf(rot), 0},
                        {         0, 0,         0, 1}}};
    return out;
}

static TformMatrix RotationZTform(float rot) {
    TformMatrix out = {{{cosf(rot), -sinf(rot), 0, 0},
                        {sinf(rot),  cosf(rot), 1, 0},
                        {        0,          0, 1, 0},
                        {        0,          0, 0, 1}}};
    return out;
}