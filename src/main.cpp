#include "Chip8.h"
#include <SDL.h>
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    Chip8 chip8;

    if (!chip8.loadROM("C:/Users/alexp/WORKSPACES/InProProjects/CHIP-8-Emulator/roms/IBM_Logo.ch8")) {
        return 1;
    }

    // Print the first 16 bytes loaded at 0x200
    // (If memory is private, temporarily make it public or add a debug getter)
    std::cout << "First 16 bytes at 0x200:\n";
    for (int i = 0x200; i < 0x210; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(chip8.memory[i]) << " ";
    }
    std::cout << std::dec << "\n";

    return 0;
}