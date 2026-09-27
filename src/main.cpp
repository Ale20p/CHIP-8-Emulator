#include "Chip8.h"
#include "Platform.h"
#include <SDL.h>
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    const int CHIP8_WIDTH = 64;
    const int CHIP8_HEIGHT = 32;
    const int SCALE = 10;

    Platform platform(
        "CHIP-8 Phase 6 test",
        CHIP8_WIDTH * SCALE,
        CHIP8_HEIGHT * SCALE,
        CHIP8_WIDTH,
        CHIP8_HEIGHT
    );

    uint32_t video[CHIP8_WIDTH * CHIP8_HEIGHT];

    const uint32_t COLOR_WHITE = 0xFFFFFFFF;
    const uint32_t COLOR_BLACK = 0x000000FF;

    // 2. Generate a checkerboard test pattern
    for (int y = 0; y < CHIP8_HEIGHT; ++y) {
        for (int x = 0; x < CHIP8_WIDTH; ++x) {
            // Draw 4x4 pixel checker blocks
            if (((x / 4) + (y / 4)) % 2 == 0) {
                video[y * CHIP8_WIDTH + x] = COLOR_WHITE;
            } else {
                video[y * CHIP8_WIDTH + x] = COLOR_BLACK;
            }
        }
    }

    // Pitch: Byte length of one full row of pixels (64 * 4 = 256 bytes)
    int pitch = sizeof(uint32_t) * CHIP8_WIDTH;

    // 3. Render loop to keep the window open
    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Push buffer to the texture and display
        platform.Update(video, pitch);

        // Cap frame rate slightly (~60 FPS) to prevent 100% CPU usage
        SDL_Delay(16);
    }

    return 0;
}