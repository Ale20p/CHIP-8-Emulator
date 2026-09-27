#include "Chip8.h"
#include <cstring>
#include <iostream>
#include <fstream>

constexpr unsigned int FONTSET_START_ADDRESS = 0x50;
constexpr unsigned int FONTSET_SIZE = 80;

const uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
    initialize();
}

void Chip8::initialize() {
    pc = 0x200;     // Programs load and start at address 0x200
    opcode = 0;
    I = 0;
    sp = 0;
    delayTimer = 0;
    soundTimer = 0;

    // Clear display, stack, registers, and memory
    std::memset(video, 0, sizeof(video));
    std::memset(stack, 0, sizeof(stack));
    std::memset(V, 0, sizeof(V));
    std::memset(memory, 0, sizeof(memory));
    std::memset(keypad, 0, sizeof(keypad));

    // Load font set into reserved memory 0x50 - 0x9F
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

bool Chip8::loadROM(const char* filename) {
    // Open in binary mode and position the pointer at the end to get file size
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open ROM file " << filename << std::endl;
        return false; 
    }

    std::streampos size = file.tellg();
    constexpr size_t max_size = 4096 - 0x200; // 3584 bytes available

    // Prevent buffer overflow into memory
    if (size > max_size) {
        std::cerr << "Error: ROM size (" << size << " bytes) exceeds available space (" << max_size <<  " bytes)." << std::endl;
        return false; 
    }

    // Seek back to the beginning and read into memory starting at 0x200
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(&memory[0x200]), size);
    file.close();

    return true;
}

void Chip8::Cycle() {
    // 1. Fetch 
    // high byte from memory[pc] and low byte from memory[pc + 1]
    // '<< 8' shifts the bitwise left by 8 positions
    // the `|` combines the upper and lower bits into one 16-bit instruction
    opcode = (memory[pc] << 8) | memory[pc + 1];

    // advance the program counter past the current 2-byte opcode
    pc += 2;

    uint8_t x = (opcode & 0x0F00u) >> 8; // register Vx
    uint8_t y = (opcode & 0x00F0u) >> 4; // register Vy
    uint8_t n = opcode & 0x000Fu; // 4-bit nible
    uint8_t nn = opcode & 0x00FFu; // 8-bit immediate byte
    uint16_t nnn = opcode & 0x0FFFu; // 12-bit memory address


    // 2. Decode & 3. Execute
    // look at the high nibble (0x0 to 0xF) to see the instruction family
    switch(opcode & 0xF000u) {
        case 0x0000:
            switch(opcode & 0x00FFu) {
                case 0x00E0: // 00E0: clear screen
                    // TODO
                    break;
                case 0x00EE: // 00EE: return from subroutine 
                    // TODO
                    break;
                default:
                    std::cerr << "Unknown 0x0000 opcode 0x:" << std::hex << opcode << "\n";
                break;
            }
            break;
        case 0x1000: // 1NNN: Jump to address NNN
            // TODO
            break;
        case 0x2000: // 2NNN: Call subroutine at NNN
            // TODO
            break;
        case 0x3000: // 3XNN: Skipnext instruction if Vx == NN
            // TODO
            break;
        case 0x4000: // 4XNN: Skip next instruction if Vx != NN
            // TODO
            break;
        case 0x5000: // 5XY0: Skip next instruction if Vx == Vy
            // TODO
            break;
        case 0x6000: // 6XNN: Set Vx == NN
            // TODO
            break;
        case 0x7000: // 7XNN: Set Vx = Vx + NN (no carry)
            // TODO
            break;
        case 0x8000: // Arithmetic and logical operations
            switch (opcode & 0x000Fu) {
                case 0x0: // 8XY0: Set Vx = Vy
                    break;
                case 0x1: // 8XY1: Set Vx = Vx OR Vy
                    break;
                case 0x2: // 8XY2: Set Vx = Vx AND Vy
                    break;
                case 0x3: // 8XY3: Set Vx = Vx XOR Vy
                    break;
                case 0x4: // 8XY4: Set Vx = Vx + Vy, set VF = carry
                    break;
                case 0x5: // 8XY5: Set Vx = Vx - Vy, set VF = NOT borrow
                    break;
                case 0x6: // 8XY6: Shift right
                    break;
                case 0x7: // 8XY7: Set Vx = Vy - Vx, set VF = NOT borrow
                    break;
                case 0x8: // 8XYE: Shift left
                    break;
                default:
                std::cerr << "Unknown 0x8000 opcode 0x" << std::hex << opcode << "\n";
                    break;
                }   
                break;
        case 0x9000: // 9XY0: Skip next instruction if Vx != Vy
            break;
        case 0xA000: // ANNN: Set I = NNN
            // TODO
            break;
        case 0xB000: // BNNN: Jump to location NNN + V0
            break;
        case 0xC000: // CXNN: Set Vx = random byte AND NN
            break;
        case 0xD000: // DXYN: Draw sprite at (Vx, Vy) with height N
            break;
        case 0xE000: // Key input skips
            switch(opcode & 0x00FFu) {
                case 0x9E: // EX9E: Skip if jey Vx is pressed
                    break;
                case 0xA1: // EXA1: Skip if key Vx is not pressed
                    break;
                default:
                    std::cerr << "Unknown 0xE000 opcode 0x" << std::hex << opcode << "\n";
                    break;
                }
                break;
        case 0xF000: // Timers, memory, and BCD operations
            switch(opcode & 0x00FFu) {
                case 0x07: // FX07: Set Vx = delay timer
                    break;
                case 0x0A: // FX0A: Wait for a key press, store in Vx
                    break;
                case 0x15: // FX15: Set delay timer = Vx
                    break;
                case 0x18: // FX18: Set sound timer = Vx
                    break;
                case 0x1E: // FX1E: Set I = I + Vx
                    break;
                case 0x29: // FX29: Set I = location of sprite for digit Vx
                    break;
                case 0x33: // FX33: Store BCD representation of Vx in I, I + 1, I + 2
                    break;
                case 0x55: // FX55: Store registers V0 through Vx in memory starting at I
                    break;
                case 0x65: // FX65: Read registers V0 through Vx from memory starting at I
                    break;
                default:
                    std::cerr << "Unknown 0xF000 opcode 0x" << std::hex << opcode << "\n";
                    break;
            }
            break;

        default:
            std::cerr << "Unrecogned opcode: 0x" << std::hex << opcode << "\n";
            break;
    }
    

    // 4. Timers
    if (delayTimer > 0) {
        --delayTimer;
    }
    if (soundTimer > 0) {
        --soundTimer;
    }
}