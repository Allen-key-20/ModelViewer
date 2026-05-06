#pragma once
#include <vector>
#include <stdexcept>
#include <algorithm>
#define SDL_MAIN_HANDLED
#pragma comment(lib, "SDL3.lib")
#include <SDL3/SDL.h>
#include "OBJLoader.h"
#include "rasterizer.h"
#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"

class Viewer
{
public:

    Viewer(int height, int width, std::string obj_path, std::string filename);

    ~Viewer();

    void run();

private:

    SDL_Window* window = nullptr;

    SDL_Renderer* renderer = nullptr;

    SDL_Texture* screen_texture = nullptr;

    std::vector<uint8_t> pixels;

    bool running = true;

    int width;
    int height;

    int rasterizer_width;
    int rasterizer_height;

    std::string obj_path;
    std::string filename;

    void updateTexture(const std::vector<Eigen::Vector3f>& frame_buffer);
};