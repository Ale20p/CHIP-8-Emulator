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

    chip8.delayTimer = 10;

    std::cout << "--- Testing CPU Cycles ---\n";

    // Execute 8 cyclese to consume the first 16 bytes (2 bytes per opcode)
    for (int cycle = 1; cycle <= 8; ++cycle) {
        uint16_t pcBefore = chip8.pc;

        // run one fetch-decode-execute step
        chip8.Cycle();

        // setw(), stands for set width 
        std::cout << "Cycle " << std::setw(2) << std::dec << cycle << " | "
            << "PC: 0x" << std::hex << std::setw(3) << std::setfill('0') << pcBefore << " | "
            << "Opcode : 0x" << std::setw(4) << std::setfill('0') << chip8.opcode << " | "
            << "Next PC: 0x" << std::setw(3) << std::setfill('0') << chip8.pc << " | "
            << "Delay Timer: " << std::dec << static_cast<int>(chip8.delayTimer) << "\n";
    }

    return 0;
}