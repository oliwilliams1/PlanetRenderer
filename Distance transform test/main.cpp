#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <SDL2/SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

// Code was made almost completely by chatgpt
void distanceTransform(const unsigned char* input, unsigned char* output, int width, int height) {

    std::vector<float> dist(width * height, 2147483648); // Use float for distance

    // Forward pass
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (input[y * width + x] > 0) {
                dist[y * width + x] = 0; // Foreground pixel
            } else {
                // Check neighbors (4-connectivity)
                if (x > 0) dist[y * width + x] = std::min(dist[y * width + x], dist[y * width + (x - 1)] + 1.0f);
                if (y > 0) dist[y * width + x] = std::min(dist[y * width + x], dist[(y - 1) * width + x] + 1.0f);
                // Check diagonal neighbors (8-connectivity)
                if (x > 0 && y > 0) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y - 1) * width + (x - 1)] + std::sqrt(2.0f));
                if (x < width - 1 && y > 0) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y - 1) * width + (x + 1)] + std::sqrt(2.0f));
                if (x > 0 && y < height - 1) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y + 1) * width + (x - 1)] + std::sqrt(2.0f));
                if (x < width - 1 && y < height - 1) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y + 1) * width + (x + 1)] + std::sqrt(2.0f));
            }
        }
    }

    // Backward pass
    for (int y = height - 1; y >= 0; --y) {
        for (int x = width - 1; x >= 0; --x) {
            if (input[y * width + x] > 0) {
                dist[y * width + x] = 0; // Foreground pixel
            } else {
                // Check neighbors (4-connectivity)
                if (x < width - 1) dist[y * width + x] = std::min(dist[y * width + x], dist[y * width + (x + 1)] + 1.0f);
                if (y < height - 1) dist[y * width + x] = std::min(dist[y * width + x], dist[(y + 1) * width + x] + 1.0f);
                // Check diagonal neighbors (8-connectivity)
                if (x < width - 1 && y < height - 1) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y + 1) * width + (x + 1)] + std::sqrt(2.0f));
                if (x > 0 && y < height - 1) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y + 1) * width + (x - 1)] + std::sqrt(2.0f));
                if (x < width - 1 && y > 0) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y - 1) * width + (x + 1)] + std::sqrt(2.0f));
                if (x > 0 && y > 0) 
                    dist[y * width + x] = std::min(dist[y * width + x], dist[(y - 1) * width + (x - 1)] + std::sqrt(2.0f));
            }
        }
    }

    // Normalize output to be in the range [0, 255]
    float maxDist = 0.0f;
    for (int i = 0; i < width * height; ++i) {
        maxDist = std::max(maxDist, dist[i]);
    }

    for (int i = 0; i < width * height; ++i) {
        output[i] = static_cast<unsigned char>(255.0f - std::min((dist[i] / maxDist) * 255, 255.0f));
    }
}

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create an SDL window
    SDL_Window *win = SDL_CreateWindow("SDL2 Alpha Channel as Grayscale", 100, 100, 800, 600, SDL_WINDOW_SHOWN);
    if (win == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create an SDL renderer
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if (ren == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // Load the image using stb_image
    int width, height, channels;
    unsigned char *data = stbi_load("elm leaf.png", &width, &height, &channels, 0);
    if (data == nullptr) {
        std::cerr << "Failed to load image: " << stbi_failure_reason() << std::endl;
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    unsigned char *singleData = new unsigned char[width * height];
    for (int i = 0; i < width * height; i++) {
        unsigned char v = data[i * channels + 3];
        singleData[i] = v;
    }

    unsigned char *distanceData = new unsigned char[width * height];
    distanceTransform(singleData, distanceData, width, height);

    // Create an array for the grayscale texture
    unsigned char *grayData = new unsigned char[width * height * 4];

    // Process the image data to create a grayscale image based on the alpha channel
    for (int i = 0; i < width * height; ++i) {
        unsigned char alpha = distanceData[i];

        grayData[i * 4 + 0] = alpha;
        grayData[i * 4 + 1] = alpha;
        grayData[i * 4 + 2] = alpha;
        grayData[i * 4 + 3] = 255; 
    }

    // Create an SDL texture from the grayscale data
    SDL_Texture *texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    if (texture == nullptr) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        delete[] singleData;
        delete[] distanceData;
        delete[] grayData; // Cleanup
        stbi_image_free(data);
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    SDL_UpdateTexture(texture, nullptr, grayData, width * 4);
    delete[] grayData;
    stbi_image_free(data);

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, texture, nullptr, nullptr);
        SDL_RenderPresent(ren);
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}