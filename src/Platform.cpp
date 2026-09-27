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