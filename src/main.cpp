#include "Chip8.h"
#include <SDL.h>
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    Chip8 chip8;

    if (!chip8.loadROM("C:/Users/alexp/WORKSPACES/InProProjects/CHIP-8-Emulator/roms/IBM_Logo.ch8")) {
        return 1;
    }

    for (int i = 0; i < 40; ++i) {
        chip8.Cycle();
    }

    chip8.PrintDisplay();
    

    return 0;
}