#include "graphics.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

static DisplayConfig* display = NULL;

#define SCALE_FACTOR 128
#define TRANSLATE_X (display->width / 2)
#define TRANSLATE_Y (display->height / 2)

#define X_ROTATE_SPEED 0.03f
#define Y_ROTATE_SPEED 0.08f
#define Z_ROTATE_SPEED 0.01f
#define PROJECTION_DISTANCE 7.0f
#define MIN_Z_DISTANCE 2.0f

static uint8_t* display_buffer = NULL;
static uint32_t frame_count = 0;

typedef struct {
    float x;
    float y;
    float z;
} Point3D;

static const Point3D cube_vertices[8] = {
    {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
    {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
};

static const uint8_t cube_edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

static void ClearBuffer(void);
static void UpdateSpinningCube(void);
static void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

void Graphics_Init(DisplayConfig* config) {
    display = config;
    ClearBuffer();
}

void Graphics_Update(void) {
    ClearBuffer();
    UpdateSpinningCube();
    frame_count++;
}

static void ClearBuffer(void) {
    uint32_t buffer_size = (display->width * display->height) / 8;
    memset(display->buffer, 0, buffer_size);
}

static void UpdateSpinningCube(void) {
    float angle_x = frame_count * X_ROTATE_SPEED;
    float angle_y = frame_count * Y_ROTATE_SPEED;
    float angle_z = frame_count * Z_ROTATE_SPEED;

    Point3D rotated_vertices[8];

    for (int i = 0; i < 8; i++) {
        float x = cube_vertices[i].x;
        float y = cube_vertices[i].y;
        float z = cube_vertices[i].z;

        // Rotate around x axis
        float rotated_y = y * cosf(angle_x) - z * sinf(angle_x);
        float rotated_z = y * sinf(angle_x) + z * cosf(angle_x);
        y = rotated_y;
        z = rotated_z;

        // Rotate around y axis
        float rotated_x = z * sinf(angle_y) + x * cosf(angle_y);
        rotated_z = z * cosf(angle_y) - x * sinf(angle_y);
        x = rotated_x;
        z = rotated_z;

        // Rotate around z axis
        rotated_x = x * cosf(angle_z) - y * sinf(angle_z);
        rotated_y = x * sinf(angle_z) + y * cosf(angle_z);
        x = rotated_x;
        y = rotated_y;

        // Project 3D point to 2D space
        float scale = SCALE_FACTOR / (PROJECTION_DISTANCE - z);
        rotated_vertices[i].x = (int16_t)(x * scale + TRANSLATE_X);
        rotated_vertices[i].y = (int16_t)(y * scale + TRANSLATE_Y);
    }

    // Draw the edges
    for (int i = 0; i < 12; i++) {
        DrawLine(rotated_vertices[cube_edges[i][0]].x, rotated_vertices[cube_edges[i][0]].y,
                 rotated_vertices[cube_edges[i][1]].x, rotated_vertices[cube_edges[i][1]].y);
    }
}

static void DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    int16_t dx = abs(x1 - x0);
    int16_t dy = abs(y1 - y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = (dx > dy ? dx : -dy) / 2;
    int16_t e2;

    while (1) {
        if (x0 >= 0 && x0 < display->width && y0 >= 0 && y0 < display->height) {
            uint16_t byte_index = x0 + (y0 / 8) * display->width;
            uint8_t bit_position = y0 % 8;
            display->buffer[byte_index] |= (1 << bit_position);
        }

        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}