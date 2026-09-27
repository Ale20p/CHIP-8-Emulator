#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

class Platform {
public:
    Platform(const char* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();

    void Update(const void* buffer, int pitch);

private:
    SDL_Window* window{nullptr};
    SDL_Renderer* renderer{nullptr};
    SDL_Texture* texture{nullptr};
};