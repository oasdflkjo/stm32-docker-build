#include "graphics.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

// 3D geometry types
typedef struct { float x, y, z; } Vec3;
typedef Vec3 Point3D;

// Graphics state
static struct {
    DisplayConfig* display;
    size_t frame;
} state;

// Cube geometry definition
static const Point3D VERTICES[8] = {
    {-1,-1,-1}, {1,-1,-1}, {1,1,-1}, {-1,1,-1},
    {-1,-1,1},  {1,-1,1},  {1,1,1},  {-1,1,1}
};

static const uint8_t EDGES[][2] = {
    {0,1}, {1,2}, {2,3}, {3,0},  // Bottom
    {4,5}, {5,6}, {6,7}, {7,4},  // Top
    {0,4}, {1,5}, {2,6}, {3,7}   // Sides
};

// Animation parameters
static const Vec3 ROTATION = { .03f, .08f, .01f };
static const float SCALE = 128;
static const float PROJ_DIST = 7.0f;

// Forward declarations of helper functions
static void clear_buffer(void);
static void draw_cube(void);
static void draw_line(int x0, int y0, int x1, int y1);
static Point3D rotate(Point3D p, Vec3 angles);
static Point3D project(Point3D p);

void Graphics_Init(DisplayConfig* display) {
    state.display = display;
    state.frame = 0;
    clear_buffer();
}

void Graphics_Update(void) {
    clear_buffer();
    draw_cube();
    state.frame++;
}

static void clear_buffer(void) {
    size_t size = (state.display->width * state.display->height) / 8;
    memset(state.display->buffer, 0, size);
}

static void draw_cube(void) {
    // Calculate current rotation
    Vec3 rot = {
        state.frame * ROTATION.x,
        state.frame * ROTATION.y,
        state.frame * ROTATION.z
    };

    // Transform and project all vertices
    Point3D points[8];
    for (int i = 0; i < 8; i++) {
        points[i] = project(rotate(VERTICES[i], rot));
    }

    // Draw all edges
    for (int i = 0; i < sizeof(EDGES)/sizeof(EDGES[0]); i++) {
        const Point3D* v1 = &points[EDGES[i][0]];
        const Point3D* v2 = &points[EDGES[i][1]];
        draw_line(v1->x, v1->y, v2->x, v2->y);
    }
}

static Point3D rotate(Point3D p, Vec3 a) {
    // Optimize by pre-calculating sin/cos
    float sx = sinf(a.x), cx = cosf(a.x);
    float sy = sinf(a.y), cy = cosf(a.y);
    float sz = sinf(a.z), cz = cosf(a.z);
    
    // X rotation
    float y = p.y * cx - p.z * sx;
    float z = p.y * sx + p.z * cx;
    p.y = y; p.z = z;

    // Y rotation
    float x = z * sy + p.x * cy;
    z = z * cy - p.x * sy;
    p.x = x; p.z = z;

    // Z rotation
    x = p.x * cz - p.y * sz;
    y = p.x * sz + p.y * cz;
    p.x = x; p.y = y;

    return p;
}

static Point3D project(Point3D p) {
    float scale = SCALE / (PROJ_DIST - p.z);
    return (Point3D){
        .x = p.x * scale + state.display->width/2,
        .y = p.y * scale + state.display->height/2,
        .z = p.z
    };
}

static void draw_line(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy)/2;

    while (1) {
        if (x0 >= 0 && x0 < state.display->width && 
            y0 >= 0 && y0 < state.display->height) {
            int idx = x0 + (y0/8) * state.display->width;
            state.display->buffer[idx] |= 1 << (y0 % 8);
        }

        if (x0 == x1 && y0 == y1) break;
        
        int e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}