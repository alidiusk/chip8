#include <stdio.h>
#include <iostream>
#include <cmath>
#include <SDL2/SDL.h>

#include "Chip8.h"

// Chip8 graphics are 64 x 32
const int DISPLAY_WIDTH = 64;
const int DISPLAY_HEIGHT = 32;
// Use a multiplier to get larger window
const int DISPLAY_MULTIPLIER = 10;

// Calculate actual screen width and height
const int SCREEN_WIDTH = DISPLAY_WIDTH * DISPLAY_MULTIPLIER;
const int SCREEN_HEIGHT = DISPLAY_HEIGHT * DISPLAY_MULTIPLIER;

void draw_graphics(SDL_Renderer* renderer, Chip8& chip8);
bool handle_input(Chip8& chip8);

void draw_graphics(SDL_Renderer* renderer, Chip8& chip8) {
    // Clear screen to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // Set drawing color to white
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    SDL_Rect r;
    for (unsigned long i = 0; i < chip8.gfx.size(); i++) {
        if (chip8.gfx[i] == 0) {
            continue;
        }

        int row = static_cast<int>(i / 64);
        int col = i % 64;

        r.x = DISPLAY_MULTIPLIER * col;
        r.y = DISPLAY_MULTIPLIER * row;
        r.w = DISPLAY_MULTIPLIER;
        r.h = DISPLAY_MULTIPLIER;

        SDL_RenderFillRect(renderer, &r);
    }

    SDL_RenderPresent(renderer);
}

// Returns true if we are quitting, false otherwise.
bool handle_input(Chip8& chip8) {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            return true;
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_1:
                    chip8.keys[0] = 1;
                    break;
                case SDLK_2:
                    chip8.keys[1] = 1;
                    break;
                case SDLK_3:
                    chip8.keys[2] = 1;
                    break;
                case SDLK_4:
                    chip8.keys[3] = 1;
                    break;
                case SDLK_q:
                    chip8.keys[4] = 1;
                    break;
                case SDLK_w:
                    chip8.keys[5] = 1;
                    break;
                case SDLK_e:
                    chip8.keys[6] = 1;
                    break;
                case SDLK_r:
                    chip8.keys[7] = 1;
                    break;
                case SDLK_a:
                    chip8.keys[8] = 1;
                    break;
                case SDLK_s:
                    chip8.keys[9] = 1;
                    break;
                case SDLK_d:
                    chip8.keys[10] = 1;
                    break;
                case SDLK_f:
                    chip8.keys[11] = 1;
                    break;
                case SDLK_z:
                    chip8.keys[12] = 1;
                    break;
                case SDLK_x:
                    chip8.keys[13] = 1;
                    break;
                case SDLK_c:
                    chip8.keys[14] = 1;
                    break;
                case SDLK_v:
                    chip8.keys[15] = 1;
                    break;
            }
        }

        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_1:
                    chip8.keys[0] = 0;
                    break;
                case SDLK_2:
                    chip8.keys[1] = 0;
                    break;
                case SDLK_3:
                    chip8.keys[2] = 0;
                    break;
                case SDLK_4:
                    chip8.keys[3] = 0;
                    break;
                case SDLK_q:
                    chip8.keys[4] = 0;
                    break;
                case SDLK_w:
                    chip8.keys[5] = 0;
                    break;
                case SDLK_e:
                    chip8.keys[6] = 0;
                    break;
                case SDLK_r:
                    chip8.keys[7] = 0;
                    break;
                case SDLK_a:
                    chip8.keys[8] = 0;
                    break;
                case SDLK_s:
                    chip8.keys[9] = 0;
                    break;
                case SDLK_d:
                    chip8.keys[10] = 0;
                    break;
                case SDLK_f:
                    chip8.keys[11] = 0;
                    break;
                case SDLK_z:
                    chip8.keys[12] = 0;
                    break;
                case SDLK_x:
                    chip8.keys[13] = 0;
                    break;
                case SDLK_c:
                    chip8.keys[14] = 0;
                    break;
                case SDLK_v:
                    chip8.keys[15] = 0;
                    break;
            }
        }
    }

    return false;
}

// System memory map:
// 0x000 - 0x050 : font data
// 0x000 - 0x1FF : Chip8 interpreter or font data
// 0x200 - 0xE9F : program + misc memory
// 0xEA0 - 0xEFF : call stack, internal use, other variables
// 0xF00 - 0xFFF : display refresh

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./main [path]" << std::endl;
        return 0;
    }

    SDL_Window* window = nullptr;
    SDL_Surface* screen_surface = nullptr;
    SDL_Renderer* renderer = nullptr;

    if (SDL_Init( SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Create window
    // Change name to name of game in future?
    window = SDL_CreateWindow(
            "Chip8 Emulator",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SCREEN_WIDTH,
            SCREEN_HEIGHT,
            SDL_WINDOW_SHOWN
        );

    if (window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Get window surface
    screen_surface = SDL_GetWindowSurface(window);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Chip8 chip8;
    chip8.initialize();
    chip8.load_program(argv[1]);

    bool quit = false;
    while (!quit) {
        unsigned long start = SDL_GetPerformanceCounter();

        try {
            chip8.emulate_cycle();
        } catch (std::exception const& e) {
            std::cout << "Exception: " << e.what() << std::endl;
            break;
        }

        // for (unsigned long i = 0; i < chip8.gfx.size(); i++) {
        //     if (i % 32 == 0)
        //         printf("\n");
        //     printf("%02x", chip8.gfx[i]);
        // }

        draw_graphics(renderer, chip8);

        quit = handle_input(chip8);

        unsigned long end = SDL_GetPerformanceCounter();
        float elapsed_ms = (end - start) / static_cast<float>(SDL_GetPerformanceFrequency()) * 1000.0f;
        // Cap to 60 FPS
        SDL_Delay(static_cast<unsigned int>(floor(16.666f - elapsed_ms)));
    }

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
