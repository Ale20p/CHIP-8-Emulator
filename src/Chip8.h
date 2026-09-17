#pragma once
#include <cstdint> 

class Chip8 {
    public:
        uint8_t memory[4096]{};     // 4KB main memory
        uint8_t V[16]{};            // 16 general-purpose 8-bit registers (V0 - V7)
        uint16_t I{0};              // 16-bit index register
        uint16_t pc{0x200};          // Program counter starting at 0x200
        uint16_t stack[16]{};        // 16-level 16-bit index stack
        uint8_t sp{0};              // 8-bit stack pointer
        uint8_t delayTimer{0};      // 60Hz delay timer
        uint8_t soundTimer{0};      // 60Hz sound timer
        uint32_t video[64 * 32]{};  // Monochrome display buffer (64x32 pixels)
        uint8_t keypad[16]{};       // 16-key hex keypad state (0x0 - 0x7)
        uint16_t opcode{0};         // Current 2-byte instruction

        Chip8();
        void initialize();
};