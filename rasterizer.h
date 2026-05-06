#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <filesystem>
#include <omp.h>
#include <mutex>
#include "eigen_handle.h"
#include "fragment_shader.h"
#include "triangle.h"
#include "camera.h"
#include "texture.h"
#include "light.h"


class Rasterizer
{
public:

    Rasterizer(int image_height, int image_width);

    void clear();

    void draw(
        const std::vector<Triangle>& mesh,
        const Texture& texture,
        Camera& camera,
        Light& light
    );

    int get_index(int x, int y);

    void save(const std::string& output_path, const std::string& filename);

    bool aabbCorrection(Eigen::Vector2i& box_min, Eigen::Vector2i& box_max);

    std::vector<Eigen::Vector3f> frame_buffer;

    std::vector<std::mutex> pixel_mutex;

private:
    std::vector<float> depth_buffer;

    int width;
    int height;

private:

    void setPixel(
        int x,
        int y,
        const Eigen::Vector3f& color
    );

    Eigen::Vector4f worldToScreen(
        const Eigen::Vector3f& pos,
        const Eigen::Matrix4f& mvp,
        float f1, float f2
    );    
    Eigen::Vector3f worldToCamera(
        const Eigen::Vector3f& pos,
        const Eigen::Matrix4f& mv
    );
};