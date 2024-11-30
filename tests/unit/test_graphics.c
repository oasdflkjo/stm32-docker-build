#include "unity.h"
#include "graphics.h"
#include <stdlib.h>
#include <string.h>

static DisplayConfig display;
static const uint16_t DISPLAY_WIDTH = 128;
static const uint16_t DISPLAY_HEIGHT = 64;
static const size_t BUFFER_SIZE = (DISPLAY_WIDTH * DISPLAY_HEIGHT) / 8;

void setUp(void) {
    // Create our test display for each test
    display.width = DISPLAY_WIDTH;
    display.height = DISPLAY_HEIGHT;
    display.buffer = (uint8_t*)calloc(BUFFER_SIZE, 1);
    Graphics_Init(&display);
}

void tearDown(void) {
    free(display.buffer);
}

void test_buffer_starts_clear(void) {
    for(size_t i = 0; i < BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_UINT8(0, display.buffer[i]);
    }
}

void test_graphics_update_modifies_buffer(void) {
    // Given - start with a clear buffer
    Graphics_Update();  // This clears and draws
    
    // When - store buffer state and update again
    uint8_t first_frame[BUFFER_SIZE];
    memcpy(first_frame, display.buffer, BUFFER_SIZE);
    
    Graphics_Update();  // Should draw cube in new position
    
    // Then - buffer should be different (cube moved)
    int differences = 0;
    for(size_t i = 0; i < BUFFER_SIZE; i++) {
        if(first_frame[i] != display.buffer[i]) differences++;
    }
    
    TEST_ASSERT_TRUE(differences > 0);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_buffer_starts_clear);
    RUN_TEST(test_graphics_update_modifies_buffer);
    return UNITY_END();
} 