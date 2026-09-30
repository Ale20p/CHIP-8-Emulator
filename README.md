# CHIP-8 Emulator

A modern, cycle-accurate CHIP-8 interpreter and virtual machine implemented in C++17 with hardware-accelerated rendering and input handling powered by SDL2.

![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![Build](https://img.shields.io/badge/Build-CMake%203.16+-brightgreen.svg)
![Library](https://img.shields.io/badge/Library-SDL2-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)

---

## Table of Contents

- [Overview](#overview)
- [Key Features](#key-features)
- [CHIP-8 System Specifications](#chip-8-system-specifications)
- [Project Architecture](#project-architecture)
- [Controls & Keypad Mapping](#controls--keypad-mapping)
- [Instruction Set (Opcode Table)](#instruction-set-opcode-table)
- [Prerequisites](#prerequisites)
- [Building the Project](#building-the-project)
- [Usage & Execution](#usage--execution)
- [Bundled ROMs](#bundled-roms)
- [Implementation Details](#implementation-details)

---

## Overview

The CHIP-8 is an interpreted programming language originally developed by Joseph Weisbecker in the mid-1970s for 8-bit microcomputers like the COSMAC VIP and Telmac 1800. It was created to facilitate video game programming on resource-constrained systems.

This project implements a complete, modern CHIP-8 emulator from scratch in C++17. It accurately reproduces the CHIP-8 execution environment—including memory layout, general-purpose and index registers, call stack, 60 Hz delay and sound timers, monochrome graphics with collision detection, and hexadecimal keypad inputs.

---

## Key Features

- **Full Opcode Coverage**: Implements all 35 standard CHIP-8 instructions (arithmetic, logic, control flow, memory manipulation, binary-coded decimal, timers, and drawing).
- **Hardware-Accelerated SDL2 Graphics**: Renders the native 64x32 monochrome display upscaled to a crisp 640x320 window using streaming textures and hardware acceleration.
- **Configurable Clock Speed**: Decouples CPU execution frequency from the 60 Hz display refresh and timer countdowns. Run games at any desired clock speed (default: 600 Hz).
- **Smooth 60 FPS Frame Limiting**: High-precision frame pacing using `std::chrono::steady_clock` to prevent screen tearing and jitter.
- **Hexadecimal Keypad Emulation**: Maps the classic 4x4 matrix keypad to intuitive QWERTY keyboard bindings with both down and up event states.
- **Safe ROM Ingestion**: Validates file size to prevent buffer overflows into interpreter memory, correctly loading binaries at offset `0x200`.
- **Debugging & Diagnostics**: Built-in ASCII display renderer (`PrintDisplay`) and full CPU state dumper (`dumpState`) reporting registers, stack depth, and program counter.

---

## CHIP-8 System Specifications

| Component | Specification | Description |
| :--- | :--- | :--- |
| **RAM** | 4,096 bytes (4 KB) | 8-bit byte addressed (`0x000` - `0xFFF`) |
| **Reserved Memory** | `0x000` - `0x1FF` | Historically reserved for the interpreter; stores the 80-byte fontset at `0x050` - `0x09F` |
| **ROM Space** | `0x200` - `0xFFF` | Programs load and begin execution at address `0x200` (up to 3,584 bytes) |
| **General Registers** | 16 registers (`V0` - `VF`) | 8-bit data registers. `VF` is used as a flag for carry, borrow, and collision detection |
| **Index Register (`I`)** | 16-bit register | Points to memory addresses for reading and writing data |
| **Program Counter (`PC`)** | 16-bit register | Stores the address of the currently executing opcode (starts at `0x200`) |
| **Call Stack** | 16 levels | Stores 16-bit return addresses during subroutine calls |
| **Stack Pointer (`SP`)** | 8-bit register | Points to the topmost position of the call stack |
| **Delay Timer** | 8-bit register | Decrements at 60 Hz down to 0; used for software timing |
| **Sound Timer** | 8-bit register | Decrements at 60 Hz down to 0; produces an audio beep while non-zero |
| **Display** | 64 x 32 monochrome pixels | 1-bit monochrome pixel array with XOR drawing and sprite collision detection |
| **Keypad** | 16 keys (`0x0` - `0xF`) | Hexadecimal matrix keypad |

---

## Project Architecture

```
CHIP-8-Emulator/
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # Project documentation
├── .vscode/                    # VS Code editor and CMake settings
│   ├── c_cpp_properties.json
│   └── settings.json
├── roms/                       # Test and playable CHIP-8 ROM files
│   ├── IBM_Logo.ch8            # Standard graphical test ROM
│   └── invaders.ch8            # Full Space Invaders game ROM
└── src/
    ├── Chip8.h                 # CHIP-8 virtual machine definition
    ├── Chip8.cpp               # CPU execution, memory, fontset & opcodes
    ├── Platform.h              # SDL2 platform abstraction definition
    ├── Platform.cpp            # SDL2 window, rendering, and input handling
    └── main.cpp                # Application entry point, timing & game loop
```

### Module Responsibilities

- **`Chip8` (`Chip8.h` / `Chip8.cpp`)**: Contains the core virtual machine state. Manages the 4 KB RAM, registers, stack, timers, font data, and executes the Fetch-Decode-Execute cycle inside `Cycle()`.
- **`Platform` (`Platform.h` / `Platform.cpp`)**: Wraps SDL2 subsystems. Creates the window and hardware renderer, manages the 64x32 streaming texture, updates the frame buffer, and polls keyboard events.
- **`main` (`main.cpp`)**: Coordinates initialization, CLI argument parsing, ROM loading, frame pacing (16.67 ms / 60 Hz), CPU cycle budgeting per frame, and timer updates.

---

## Controls & Keypad Mapping

The original CHIP-8 systems utilized a 16-key hexadecimal keypad (arranged `0x0` through `0xF`). This emulator maps those keys directly to modern keyboard layouts for ergonomic gameplay:

```
  Original CHIP-8 Keypad             Modern QWERTY Keypad
    +-+-+-+-+                         +-+-+-+-+
    | 1 | 2 | 3 | C |                 | 1 | 2 | 3 | 4 |
    +-+-+-+-+                         +-+-+-+-+
    | 4 | 5 | 6 | D |       --->      | Q | W | E | R |
    +-+-+-+-+                         +-+-+-+-+
    | 7 | 8 | 9 | E |                 | A | S | D | F |
    +-+-+-+-+                         +-+-+-+-+
    | A | 0 | B | F |                 | Z | X | C | V |
    +-+-+-+-+                         +-+-+-+-+
```

### Keybinding Table

| CHIP-8 Key | Keyboard Key | Description |
| :---: | :---: | :--- |
| `1` | `1` | Keypad 1 |
| `2` | `2` | Keypad 2 |
| `3` | `3` | Keypad 3 |
| `C` | `4` | Keypad C |
| `4` | `Q` | Keypad 4 |
| `5` | `W` | Keypad 5 |
| `6` | `E` | Keypad 6 |
| `D` | `R` | Keypad D |
| `7` | `A` | Keypad 7 |
| `8` | `S` | Keypad 8 |
| `9` | `D` | Keypad 9 |
| `E` | `F` | Keypad E |
| `A` | `Z` | Keypad A |
| `0` | `X` | Keypad 0 |
| `B` | `C` | Keypad B |
| `F` | `V` | Keypad F |
| **Exit** | `Escape` | Close emulator and exit |

---

## Instruction Set (Opcode Table)

Each CHIP-8 instruction is 2 bytes (16 bits) stored big-endian. The emulator fetches two consecutive bytes, shifts and masks them into an opcode, and executes the designated operation.

| Opcode | Mnemonic | Description |
| :--- | :--- | :--- |
| `00E0` | `CLS` | Clear the video display buffer |
| `00EE` | `RET` | Return from a subroutine (pops address from stack) |
| `1NNN` | `JP addr` | Jump to memory address `NNN` |
| `2NNN` | `CALL addr` | Call subroutine at `NNN` (pushes current `PC` to stack) |
| `3XNN` | `SE Vx, byte` | Skip next instruction if `Vx == NN` |
| `4XNN` | `SNE Vx, byte` | Skip next instruction if `Vx != NN` |
| `5XY0` | `SE Vx, Vy` | Skip next instruction if `Vx == Vy` |
| `6XNN` | `LD Vx, byte` | Set register `Vx = NN` |
| `7XNN` | `ADD Vx, byte` | Set `Vx = Vx + NN` (carry flag `VF` unaffected) |
| `8XY0` | `LD Vx, Vy` | Set `Vx = Vy` |
| `8XY1` | `OR Vx, Vy` | Set `Vx = Vx | Vy` |
| `8XY2` | `AND Vx, Vy` | Set `Vx = Vx & Vy` |
| `8XY3` | `XOR Vx, Vy` | Set `Vx = Vx ^ Vy` |
| `8XY4` | `ADD Vx, Vy` | Set `Vx = Vx + Vy`, set `VF = 1` on carry, `0` otherwise |
| `8XY5` | `SUB Vx, Vy` | Set `Vx = Vx - Vy`, set `VF = NOT borrow` (`1` if `Vx >= Vy`) |
| `8XY6` | `SHR Vx` | Store least-significant bit of `Vx` in `VF`, then divide `Vx` by 2 (shift right) |
| `8XY7` | `SUBN Vx, Vy` | Set `Vx = Vy - Vx`, set `VF = NOT borrow` (`1` if `Vy >= Vx`) |
| `8XYE` | `SHL Vx` | Store most-significant bit of `Vx` in `VF`, then multiply `Vx` by 2 (shift left) |
| `9XY0` | `SNE Vx, Vy` | Skip next instruction if `Vx != Vy` |
| `ANNN` | `LD I, addr` | Set index register `I = NNN` |
| `BNNN` | `JP V0, addr` | Jump to target address `NNN + V0` |
| `CXNN` | `RND Vx, byte` | Set `Vx = (random byte) & NN` |
| `DXYN` | `DRW Vx, Vy, nibble` | Draw `N`-byte sprite at coordinates `(Vx, Vy)`. XORs pixels; sets `VF = 1` if collision occurs |
| `EX9E` | `SKP Vx` | Skip next instruction if the key stored in `Vx` is pressed |
| `EXA1` | `SKNP Vx` | Skip next instruction if the key stored in `Vx` is not pressed |
| `FX07` | `LD Vx, DT` | Set `Vx = delayTimer` |
| `FX0A` | `LD Vx, K` | Wait for key press and store the pressed key in `Vx` (blocking) |
| `FX15` | `LD DT, Vx` | Set `delayTimer = Vx` |
| `FX18` | `LD ST, Vx` | Set `soundTimer = Vx` |
| `FX1E` | `ADD I, Vx` | Set `I = I + Vx` |
| `FX29` | `LD F, Vx` | Set `I = location of font sprite for digit Vx` (character in `0x50` - `0x9F`) |
| `FX33` | `LD B, Vx` | Store Binary-Coded Decimal (BCD) of `Vx` in memory at `I`, `I+1`, `I+2` |
| `FX55` | `LD [I], Vx` | Store registers `V0` through `Vx` into memory starting at address `I` |
| `FX65` | `LD Vx, [I]` | Read registers `V0` through `Vx` from memory starting at address `I` |

---

## Prerequisites

To build and run this emulator, ensure you have the following installed:

- **C++ Compiler**: A C++17 compatible compiler (e.g., MSVC on Windows, GCC 9+, or Clang 10+).
- **CMake**: Version 3.16 or higher.
- **SDL2 Library**: Used for window creation, hardware graphics presentation, and keyboard polling.
- *(Recommended on Windows)* **vcpkg**: Microsoft's C++ library package manager.

---

## Building the Project

### Windows (MSVC & vcpkg)

1. **Install SDL2 using vcpkg**:
   ```powershell
   vcpkg install sdl2:x64-windows
   ```

2. **Generate build files with CMake**:
   ```powershell
   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
   ```

3. **Compile the executable**:
   ```powershell
   cmake --build build --config Release
   ```
   *(Or build Debug with `--config Debug`)*

The generated binary will be located at `build/Release/chip8.exe` (or `build/Debug/chip8.exe`).

### Linux / macOS

1. **Install dependencies**:
   ```bash
   # Debian / Ubuntu
   sudo apt-get install build-essential cmake libsdl2-dev

   # macOS (Homebrew)
   brew install cmake sdl2
   ```

2. **Configure and build**:
   ```bash
   cmake -B build -S .
   cmake --build build --config Release
   ```

---

## Usage & Execution

Run the emulator by passing the path to a `.ch8` ROM file. You can also specify an optional clock speed in Hz:

```bash
./chip8 <ROM path> [clock speed in Hz]
```

### Arguments

- `<ROM path>` *(Required)*: Relative or absolute path to the CHIP-8 ROM binary.
- `[clock speed in Hz]` *(Optional)*: Desired execution frequency of the CPU. Defaults to **600 Hz**.

### Examples

```powershell
# Run the IBM Logo test at default speed (600 Hz)
.\build\Debug\chip8.exe roms/IBM_Logo.ch8

# Run Space Invaders at 700 Hz
.\build\Debug\chip8.exe roms/invaders.ch8 700

# Run high-speed games at 1000 Hz
.\build\Debug\chip8.exe roms/pong.ch8 1000
```

---

## Bundled ROMs

The repository includes sample ROMs in the [`roms/`](roms/) directory:

- **`IBM_Logo.ch8`**: A classic demonstration ROM that displays the IBM logo on screen. Useful for verifying basic opcode execution and graphics rendering.
- **`invaders.ch8`**: A full, playable Space Invaders clone showcasing sprite rendering, collision detection, keypad input handling, and 60 Hz timer decrementing.
  - Controls: `Q` (Left), `E` (Right), `W` (Shoot/Start).

---

## Implementation Details

### 1. Decoupled CPU Timing
Rather than executing 1 instruction per 60 Hz frame (which would make games unplayably slow at 60 Hz), the main loop calculates:
$$\text{cyclesPerFrame} = \max\left(1, \frac{\text{clockSpeed}}{60}\right)$$
For each 60 Hz frame (~16.67 ms):
1. User input events are processed.
2. The CPU runs `cyclesPerFrame` cycles (e.g., 10 cycles for 600 Hz).
3. The `delayTimer` and `soundTimer` decrement by 1 if greater than 0.
4. The display texture is updated and rendered to the screen.
5. The remaining frame duration is slept using `std::this_thread::sleep_for`.

### 2. Coordinate Wrapping & Sprite Collision
The `DXYN` instruction draws sprites of width 8 and variable height $N$ bytes:
- Sprites wrap cleanly around the horizontal (64) and vertical (32) boundaries using modulo arithmetic (`(xCoord + col) % 64` and `(yCoord + row) % 32`).
- Pixel collision is flagged (`VF = 1`) whenever a sprite pixel attempts to draw over an already active pixel (`0xFFFFFFFF`), which is critical for collision physics in games like Space Invaders and Pong.

### 3. Blocking Keypad Input (`FX0A`)
The `FX0A` opcode requires the processor to halt execution until a key is pressed. This emulator implements it non-blockingly for the operating system: if no key is currently held down in the keypad buffer, the program counter is decremented by 2 (`pc -= 2`), causing the instruction to re-execute on the subsequent cycle without freezing the SDL2 event loop or operating system window.
