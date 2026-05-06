#pragma once
#include <cassert>
#include <Eigen/Eigen>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <algorithm> // for std::clamp
#include <filesystem>
namespace fs = std::filesystem;

class Texture {
private:
    cv::Mat image_data;
    int width = 0;
    int height = 0;

    // 把扩展名列表放进类里（静态常量，所有实例共享）
    static const std::vector<std::string> supported_extensions;

    // 内部加载函数：自动尝试所有格式
    bool tryLoadImage(const std::string& base_path, const std::string& model_name);

public:
    // 构造函数只需要传入 "models/rock"，自动加后缀尝试加载
    Texture(const std::string& base_path, const std::string& model_name);

    // 获取颜色
    Eigen::Vector3f getColor(float u, float v) const;

    // 判断纹理是否有效
    bool isValid() const {
        return !image_data.empty();
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
};

