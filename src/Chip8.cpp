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
                    std::memset(video, 0, sizeof(video));
                    break;
                case 0x00EE: // 00EE: return from subroutine 
                    --sp;
                    pc = stack[sp];
                    break;
                default:
                    std::cerr << "Unknown 0x0000 opcode 0x:" << std::hex << opcode << "\n";
                break;
            }
            break;
        case 0x1000: // 1NNN: Jump to address NNN
            pc = nnn;
            break;
        case 0x2000: // 2NNN: Call subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = nnn;
            break;
        case 0x3000: // 3XNN: Skipnext instruction if Vx == NN
            if (V[x] == nn) {
                pc += 2;
            }
            break;
        case 0x4000: // 4XNN: Skip next instruction if Vx != NN
            if (V[x] != nn) {
                pc += 2;
            }
            break;
        case 0x5000: // 5XY0: Skip next instruction if Vx == Vy
            if ( n == 0x0) {
                if (V[x] == V[y]) {
                    pc += 2;
                } else {
                    std::cerr << "Unknown opcode [0x5000]: 0x" << std::hex << opcode << "\n";
                }
            }
            break;
        case 0x6000: // 6XNN: Set Vx = NN
            V[x] = nn;
            break;
        case 0x7000: // 7XNN: Set Vx = Vx + NN (no carry)
            V[x] += nn;
            break;
        case 0x8000: // Arithmetic and logical operations
            switch (opcode & 0x000Fu) {
                case 0x0: // 8XY0: Set Vx = Vy
                    V[x] = V[y];
                    break;
                case 0x1: // 8XY1: Set Vx = Vx OR Vy
                    V[x] |= V[y];
                    break;
                case 0x2: // 8XY2: Set Vx = Vx AND Vy
                    V[x] &= V[y];
                    break;
                case 0x3: // 8XY3: Set Vx = Vx XOR Vy
                    V[x] ^= V[y];
                    break;
                case 0x4: // 8XY4: Set Vx = Vx + Vy, set VF = carry
                    uint16_t sum = static_cast<uint16_t>(V[x]) + static_cast<uint16_t>(V[y]);
                    V[x] = sum & 0xFF;
                    V[0xF] = (sum > 0xFF) ? 1 : 0; // 1 if overflow (> 255), 0 otherwise
                    break;
                case 0x5: // 8XY5: Set Vx = Vx - Vy, set VF = NOT borrow
                    // 1 if there is no borrow, and 0 if there is borrow
                    uint8_t flag = (V[x] >= V[y]) ? 1 : 0;
                    V[x] = V[x] - V[y];
                    V[0xF] = flag;
                    break;
                case 0x6: // 8XY6: Shift right
                    uint8_t lsb = V[x] & 0x01; // least significant bit
                    V[x] >>= 1;
                    V[0xF] = lsb; // store shifted-out bit into VF
                    break;
                case 0x7: // 8XY7: Set Vx = Vy - Vx, set VF = NOT borrow
                    uint8_t flag = (V[y] >= V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    V[0xF] = flag;
                    break;
                case 0xE: // 8XYE: Shift left
                    uint8_t msb = (V[x] & 0x80) >> 7; // most significant bit
                    V[x] <<= 1;
                    V[0xF] = msb;
                    break;
                default:
                    std::cerr << "Unknown 0x8000 opcode 0x" << std::hex << opcode << "\n";
                    break;
                }   
                break;
        case 0x9000: // 9XY0: Skip next instruction if Vx != Vy
                if (n == 0) {
                    if (V[x] != V[y]) {
                        pc += 2;
                    } else {
                        std::cerr << "Unknow opcode [0x9000]: 0x" << std::hex << opcode << "\n";
                    }
                }
            break;
        case 0xA000: // ANNN: Set I = NNN
            I = nnn;
            break;
        case 0xB000: // BNNN: Jump to location NNN + V0
            pc = nnn + V[0];
            break;
        case 0xC000: // CXNN: Set Vx = random byte AND NN
                V[x] = (rand() % 256) & nn;
            break;
        case 0xD000: // DXYN: Draw sprite at (Vx, Vy) with height N
                uint8_t xCoord = V[x] % 64;
                uint8_t yCoord = V[y] % 32;

                // reset collision register before drawing
                V[0xF] = 0;

                for (unsigned int row = 0; row < n; ++row) {
                    uint8_t spriteByte = memory[I + row];

                    for (unsigned int col = 0; col < 8; ++col) {
                        // check if the current bit of the sprite is set
                        if ((spriteByte & (0x80 >> col)) != 0) {
                            // wrap sprite pixels around screen boundaries
                            unsigned int pixelX = (xCoord + col) % 64;
                            unsigned int pixelY = (yCoord + row) % 32;
                            unsigned int index = pixelY * 64 + pixelX;

                            // check for collision: if the screen pixel is already turned on
                            if (video[index] == 0xFFFFFFFF) {
                                V[0xF] = 1;
                            }

                            // XOR the screen pixel (toggle between 0x00000000 and 0xFFFFFFFF)
                            video[index] ^= 0xFFFFFFFF;
                        }
                    }
                }
            break;
        case 0xE000: // Key input skips
            switch(opcode & 0x00FFu) {
                case 0x9E: // EX9E: Skip if key Vx is pressed
                    uint8_t key = V[x];
                    if (key < 16 && keypad[key] != 0) {
                        pc += 2;
                    }
                    break;
                case 0xA1: // EXA1: Skip if key Vx is not pressed
                    uint8_t key = V[x];
                    if (key >= 16 || keypad[key] == 0) {
                        pc += 2;
                    }
                    break;
                default:
                    std::cerr << "Unknown 0xE000 opcode 0x" << std::hex << opcode << "\n";
                    break;
                }
                break;
        case 0xF000: // Timers, memory, and BCD operations
            switch(opcode & 0x00FFu) {
                case 0x07: // FX07: Set Vx = delay timer
                    V[x] = delayTimer;
                    break;
                case 0x0A: // FX0A: Wait for a key press, store in Vx
                    bool keyPressed = false;

                    for (uint8_t i = 0; i < 16; i++) {
                        if (keypad[i] != 0) {
                            V[x] = i;
                            keyPressed = true;
                            break;
                        }
                    }

                    // if no key is down, rewind PC by 2 so this opcode repeats next cycle
                    if (!keyPressed) {
                        pc -= 2;
                    }
                    break;
                case 0x15: // FX15: Set delay timer = Vx
                    delayTimer = V[x];
                    break;
                case 0x18: // FX18: Set sound timer = Vx
                    soundTimer = V[x];
                    break;
                case 0x1E: // FX1E: Set I = I + Vx
                    I += V[x];
                    break;
                case 0x29: // FX29: Set I = location of sprite for digit Vx
                    // standard font characcters are 5 bytes tall, loaded at 0x50
                    I = 0x50 + (V[x] * 5);
                    break;
                case 0x33: // FX33: Store BCD representation of Vx in I, I + 1, I + 2
                    uint8_t value = V[x];
                    memory[I + 2] = value % 10;
                    value /= 10;
                    memory[I + 1] = value % 10;
                    value /= 10;
                    memory[I] = value % 10;
                    break;
                case 0x55: // FX55: Store registers V0 through Vx in memory starting at I
                    for (uint8_t i = 0; i <= x; ++i) {
                        memory[I + i] = V[i];
                    }
                    break;
                case 0x65: // FX65: Read registers V0 through Vx from memory starting at I
                    for (uint8_t i = 0; i <= x; i++) {
                        V[i] = memory[I + i];
                    }
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