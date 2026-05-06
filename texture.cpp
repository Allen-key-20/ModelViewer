#include "texture.h"

// ==============================================
// 静态成员初始化（必须写在类外面）
// ==============================================
const std::vector<std::string> Texture::supported_extensions = {
    ".png",
    ".jpg",
    ".jpeg",
    ".bmp"
};

// ==============================================
// 构造函数：自动尝试所有图片格式
// ==============================================
Texture::Texture(const std::string& base_path, const std::string& model_name) {
    // 直接调用内部自动加载逻辑
    bool success = tryLoadImage(base_path, model_name);

    if (!success) {
        image_data.release();
        width = 0;
        height = 0;
    }
}

// ==============================================
// 自动尝试加载图片
// ==============================================
bool Texture::tryLoadImage(const std::string& base_path, const std::string& model_name) {
    for (const auto& ext : supported_extensions) {
        std::string full_path = base_path + '/' + model_name + ext;

        if (fs::exists(full_path)) {
            cv::Mat img = cv::imread(full_path);
            cv::cvtColor(img, image_data, cv::COLOR_RGB2BGR);
            width = image_data.cols;
            height = image_data.rows;
            return true;
        }
    }
    return false;
}

// ==============================================
// 采样纹理颜色
// ==============================================
Eigen::Vector3f Texture::getColor(float u, float v) const {
    if (!isValid()) {
        return Eigen::Vector3f(0.5f, 0.5f, 0.5f); // 灰色
    }

    if (0 <= u && u <= 1 && 0 <= v && v <= 1) {
        auto u_img = u * width;
        auto v_img = (1 - v) * height;

        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    return Eigen::Vector3f(0.5f, 0.5f, 0.5f);
}