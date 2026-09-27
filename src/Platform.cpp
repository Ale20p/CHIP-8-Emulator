#include "Platform.h"
#include <iostream>


Platform::Platform(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    // 1. Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize. SDL_Error:" << SDL_GetError() << "\n";
        return;
    }

    // 2. create scaled window (640x320)
    window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return;
    }

    // 3. Create a hardware-accelerated renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
    }

    // 4. Create the native-resolution texture (64x32)
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,   // Matches 32-bit pixel data
        SDL_TEXTUREACCESS_STREAMING,    // Crucial for textures updated every frame
        textureWidth,   // 64
        textureHeight // 32
    );

    if (!texture) {
        std::cerr << "Texture could not be created. SDL_Error: " << SDL_GetError() << "\n";
    }
}

void Platform::Update(const void* buffer, int pitch) {
    // push the raw pixel array into the GPU texture
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);

    // clear the existing render target
    SDL_RenderClear(renderer);

    // copy the 64x32 texture onto the entire window (stretches automatically)
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // present the back-buffer to the display
    SDL_RenderPresent(renderer);
} 

Platform::~Platform() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Platform::ProcessInput(uint8_t* keypad) {
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN: {
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE: quit = true; break;
                    case SDLK_x: keypad[0x0] = 1; break;
                    case SDLK_1: keypad[0x1] = 1; break;
                    case SDLK_2: keypad[0x2] = 1; break;
                    case SDLK_3: keypad[0x3] = 1; break;
                    case SDLK_q: keypad[0x4] = 1; break;
                    case SDLK_w: keypad[0x5] = 1; break;
                    case SDLK_e: keypad[0x6] = 1; break;
                    case SDLK_a: keypad[0x7] = 1; break;
                    case SDLK_s: keypad[0x8] = 1; break;
                    case SDLK_d: keypad[0x9] = 1; break;
                    case SDLK_z: keypad[0xA] = 1; break;
                    case SDLK_c: keypad[0xB] = 1; break;
                    case SDLK_4: keypad[0xC] = 1; break;
                    case SDLK_r: keypad[0xD] = 1; break;
                    case SDLK_f: keypad[0xE] = 1; break;
                    case SDLK_v: keypad[0xF] = 1; break;
                    default: break;
                }
                break;
            }
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                    case SDLK_x: keypad[0x0] = 0; break;
                    case SDLK_1: keypad[0x1] = 0; break;
                    case SDLK_2: keypad[0x2] = 0; break;
                    case SDLK_3: keypad[0x3] = 0; break;
                    case SDLK_q: keypad[0x4] = 0; break;
                    case SDLK_w: keypad[0x5] = 0; break;
                    case SDLK_e: keypad[0x6] = 0; break;
                    case SDLK_a: keypad[0x7] = 0; break;
                    case SDLK_s: keypad[0x8] = 0; break;
                    case SDLK_d: keypad[0x9] = 0; break;
                    case SDLK_z: keypad[0xA] = 0; break;
                    case SDLK_c: keypad[0xB] = 0; break;
                    case SDLK_4: keypad[0xC] = 0; break;
                    case SDLK_r: keypad[0xD] = 0; break;
                    case SDLK_f: keypad[0xE] = 0; break;
                    case SDLK_v: keypad[0xF] = 0; break;
                    default: break;
                }
                break;
            default:
                break;
        }
    }

    return !quit;
}