#include "Chip8.h"
#include "Platform.h"
#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <string>

int main(int argc, char* argv[]) {
    if (argc <  2) {
        std::cerr << "Usage: " << argv[0] << " <ROM path> [clock speed in Hz]\n";
        std::cerr << "Example: " << argv[0] << " roms/pong.ch8 700\n";
        return 1;
    }

    const char* romFilename = argv[1];

    // Default to 600 Hz 
    int clockSpeed = 600;

    if (argc >= 3) {
        try {
            clockSpeed = std::stoi(argv[2]);
            if (clockSpeed <= 0) {
                std::cerr << "Warning: Speed must be positive. Falling back to default (600 Hz).\n";
                clockSpeed = 600;
            }
        } catch (const std::exception&) {
            std::cerr << "Warning: Invalid speed argument. Falling back to default (600 Hz).\n";
            clockSpeed = 600;
        }
    }

    // Decouple CPU speed from timers: determine cycles to run per 60Hz frame, ensures at least 1 cycle exceutes per frame
    int cyclesPerFrame = std::max(1, clockSpeed / 60);

    std::cout << "Running at " << clockSpeed << " Hz (~" << cyclesPerFrame << " cycles/frame)\n";

    Chip8 chip8;
    if (!chip8.loadROM(romFilename)) {
        std::cerr << "Failed to load ROM: " << romFilename << "\n";
        return 1;
    }

    Platform platform("CHIP-8 Emulator", 640, 320, 64, 32); // Window Title, Weight, Height, Scale

    const std::chrono::duration<double, std::milli> frameDuration(1000.0 / 60.0); // ~16.67 ms
    bool running = true;

    while (running) {
        auto frameStart = std::chrono::steady_clock::now();

        // 1. Process Input
        running = platform.ProcessInput(chip8.keypad);

        // 2. Run CPU Cycles for this frame
        for (int i = 0; i < cyclesPerFrame; ++i) {
            chip8.Cycle();
        }

        // 3. Update Timers at 60Hz
        if (chip8.delayTimer > 0) {
            --chip8.delayTimer;
        }
        if (chip8.soundTimer > 0) {
            --chip8.soundTimer;
        }

        // 4. Render Display
        platform.Update(chip8.video, sizeof(chip8.video[0]) * 64);

        // 5. Frame Delay
        auto frameEnd = std::chrono::steady_clock::now();
        std::chrono::duration<double, std::milli> elapsed = frameEnd - frameStart;

        if (elapsed < frameDuration) {
            std::this_thread::sleep_for(frameDuration - elapsed);
        }
    }

    return 0;
}