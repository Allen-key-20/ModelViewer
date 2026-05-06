#include "rasterizer.h"

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Eigen::Vector4f(v3.x(), v3.y(), v3.z(), w);
}

auto to_vec3(const Eigen::Vector4f& v4)
{
    return Eigen::Vector3f(v4.x(), v4.y(), v4.z());
}

static Eigen::Vector3f interpolate(float alpha, float beta, float gamma, const Eigen::Vector3f& vert1, const Eigen::Vector3f& vert2, const Eigen::Vector3f& vert3, float weight)
{
    return (alpha * vert1 + beta * vert2 + gamma * vert3) / weight;
}

static Eigen::Vector2f interpolate(float alpha, float beta, float gamma, const Eigen::Vector2f& vert1, const Eigen::Vector2f& vert2, const Eigen::Vector2f& vert3, float weight)
{
    auto u = (alpha * vert1[0] + beta * vert2[0] + gamma * vert3[0]);
    auto v = (alpha * vert1[1] + beta * vert2[1] + gamma * vert3[1]);

    u /= weight;
    v /= weight;

    return Eigen::Vector2f(u, v);
}

static std::tuple<double , double, double> computeBarycentric2D(float x, float y, const std::array <Eigen::Vector4f, 3> v) {
    double c1 = (x * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * y + v[1].x() * v[2].y() - v[2].x() * v[1].y()) / (v[0].x() * (v[1].y() - v[2].y()) + (v[2].x() - v[1].x()) * v[0].y() + v[1].x() * v[2].y() - v[2].x() * v[1].y());
    double c2 = (x * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * y + v[2].x() * v[0].y() - v[0].x() * v[2].y()) / (v[1].x() * (v[2].y() - v[0].y()) + (v[0].x() - v[2].x()) * v[1].y() + v[2].x() * v[0].y() - v[0].x() * v[2].y());
    double c3 = (x * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * y + v[0].x() * v[1].y() - v[1].x() * v[0].y()) / (v[2].x() * (v[0].y() - v[1].y()) + (v[1].x() - v[0].x()) * v[2].y() + v[0].x() * v[1].y() - v[1].x() * v[0].y());
    return { c1,c2,c3 };
}

static bool insideTriangle(int x, int y, const std::array <Eigen::Vector4f, 3> _v) {
    Eigen::Vector3f v[3];
    for (int i = 0; i < 3; i++)
        v[i] = { _v[i].x(),_v[i].y(), 1.0 };
    Eigen::Vector3f f0, f1, f2;
    f0 = v[1].cross(v[0]);
    f1 = v[2].cross(v[1]);
    f2 = v[0].cross(v[2]);
    Eigen::Vector3f p(x, y, 1.);
    if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) && (p.dot(f2) * f2.dot(v[1]) > 0))
        return true;
    return false;
}

Rasterizer::Rasterizer(int image_height, int image_width)
{
    width = image_width;
    height = image_height;

    frame_buffer.resize(height * width);

    depth_buffer.resize(height * width);

    pixel_mutex = std::vector<std::mutex>(width * height);
}

int Rasterizer::get_index(int x, int y)
{
    return (height - 1 - y) * width + x;
}

void Rasterizer::clear()
{
    std::fill(
        frame_buffer.begin(),
        frame_buffer.end(),
        Eigen::Vector3f(0.0f, 0.0f, 0.0f)
    );

    std::fill(
        depth_buffer.begin(),
        depth_buffer.end(),
        std::numeric_limits<float>::infinity()
    );
}

void Rasterizer::setPixel(int x, int y, const Eigen::Vector3f& color)
{
    if (x < 0 || x >= width)
        return;

    if (y < 0 || y >= height)
        return;

    int index = (height - y) * width + x;

    frame_buffer[index] = color;
}

Eigen::Vector3f Rasterizer::worldToCamera(const Eigen::Vector3f& pos, const Eigen::Matrix4f& mv)
{
    Eigen::Vector4f p_tmp(pos.x(), pos.y(), pos.z(), 1.0f);

    p_tmp = mv * p_tmp;

    Eigen::Vector3f p(p_tmp.x(), p_tmp.y(), p_tmp.z());
    return p;
}

Eigen::Vector4f Rasterizer::worldToScreen(const Eigen::Vector3f& pos, const Eigen::Matrix4f& mvp, float f1, float f2)
{
    Eigen::Vector4f p(
        pos.x(),
        pos.y(),
        pos.z(),
        1.0f
    );

    // MVP
    p = mvp * p;

    // Perspective Divide
    p.x() /= p.w();
    p.y() /= p.w();
    p.z() /= p.w();

    // NDC -> Screen

    p.x() = 0.5f * width * (p.x() + 1.0f);
    p.y() = 0.5f * height * (p.y() + 1.0f);
    p.z() = p.z() * f1 + f2;
    return p;
}

bool Rasterizer::aabbCorrection(Eigen::Vector2i& box_min, Eigen::Vector2i& box_max)
{
    int x_min = box_min.x();
    int y_min = box_min.y();
    int x_max = box_max.x();
    int y_max = box_max.y();

    if (x_min >= width)
        return true;
    if (y_min >= height)
        return true;
    if (x_max < 0)
        return true;
    if (y_max < 0)
        return true;

    x_min = std::max(x_min, 1);
    y_min = std::max(y_min, 1);

    x_max = std::min(x_max, width-1);
    y_max = std::min(y_max, height-1);

    box_min = Eigen::Vector2i(x_min, y_min);
    box_max = Eigen::Vector2i(x_max, y_max);

    return false;
}

void Rasterizer::draw(const std::vector<Triangle>& mesh, const Texture& texture, Camera& camera, Light& light)
{
    float f1 = (camera.z_far - camera.z_near) / 2.0;
    float f2 = (camera.z_far + camera.z_near) / 2.0;

    Eigen::Matrix4f mv = camera.mvm;
    Eigen::Matrix4f mvp = camera.mvpm;

    #pragma omp parallel for
    for (int t = 0; t < mesh.size(); t++)
    {
        const auto& tri = mesh[t];
        Eigen::Vector3f pos_in_cam0 = worldToCamera(tri.v0.position, mv);
        Eigen::Vector3f pos_in_cam1 = worldToCamera(tri.v1.position, mv);
        Eigen::Vector3f pos_in_cam2 = worldToCamera(tri.v2.position, mv);
        std::array <Eigen::Vector3f, 3> pos_in_camera = { pos_in_cam0, pos_in_cam1, pos_in_cam2 };

        if (pos_in_cam0.z() > -camera.z_near && pos_in_cam1.z() > -camera.z_near && pos_in_cam2.z() > -camera.z_near)
            continue;
        if (pos_in_cam0.z() < -camera.z_far && pos_in_cam1.z() < -camera.z_far && pos_in_cam2.z() < -camera.z_far)
            continue;

        Eigen::Vector4f pos_in_scr0 = worldToScreen(tri.v0.position, mvp, f1, f2);
        Eigen::Vector4f pos_in_scr1 = worldToScreen(tri.v1.position, mvp, f1, f2);
        Eigen::Vector4f pos_in_scr2 = worldToScreen(tri.v2.position, mvp, f1, f2);
        std::array <Eigen::Vector4f, 3> pos_in_screen = { pos_in_scr0, pos_in_scr1, pos_in_scr2 };

        if ((pos_in_scr0.x() < 0 && pos_in_scr1.x() < 0 && pos_in_scr2.x() < 0) ||
            (pos_in_scr0.x() >= width && pos_in_scr1.x() >= width && pos_in_scr2.x() >= width) ||
            (pos_in_scr0.y() < 0 && pos_in_scr1.y() < 0 && pos_in_scr2.y() < 0) ||
            (pos_in_scr0.y() >= height && pos_in_scr1.y() >= height && pos_in_scr2.y() >= height))
            continue;

        Eigen::Matrix4f inv_trans = camera.mm.inverse().transpose();
        Eigen::Vector3f n[] = {
          to_vec3(inv_trans * to_vec4(tri.v0.normal, 0.0f)),
          to_vec3(inv_trans * to_vec4(tri.v1.normal, 0.0f)),
          to_vec3(inv_trans * to_vec4(tri.v2.normal, 0.0f)) };

        //std::cout << pos_in_scr0 << std::endl;
        //std::cout << pos_in_scr1 << std::endl;
        //std::cout << pos_in_scr2 << std::endl;

        int x_min = INT_MAX;
        int y_min = INT_MAX;
        int x_max = INT_MIN;
        int y_max = INT_MIN;

        for (auto& vert : pos_in_screen)
        {
            x_min = std::min(x_min, static_cast<int>(std::floor(vert.x())));
            y_min = std::min(y_min, static_cast<int>(std::floor(vert.y())));
            x_max = std::max(x_max, static_cast<int>(std::ceil(vert.x())));
            y_max = std::max(y_max, static_cast<int>(std::ceil(vert.y())));
        }

        Eigen::Vector2i box_min(x_min, y_min);
        Eigen::Vector2i box_max(x_max, y_max);

        if (aabbCorrection(box_min, box_max))
            continue;

        for (int i = box_min.x(); i < box_max.x(); i++)
        {
            for (int j = box_min.y(); j < box_max.y(); j++)
            {
                float x = i;
                float y = j;

                //if (!insideTriangle(x, y, pos_in_screen))
                //    continue;
                auto [alpha, beta, gamma] = computeBarycentric2D(x, y, pos_in_screen);
                if (alpha < 0 || beta < 0 || gamma < 0)
                    continue;
                float w_reciprocal = 1.0 / (alpha / pos_in_screen[0].w() + beta / pos_in_screen[1].w() + gamma / pos_in_screen[2].w());
                float z_interpolated = alpha * pos_in_screen[0].z() / pos_in_screen[0].w() + beta * pos_in_screen[1].z() / pos_in_screen[1].w() + gamma * pos_in_screen[2].z() / pos_in_screen[2].w();
                z_interpolated *= w_reciprocal;

                int index = get_index(x, y);

                //if (index < 0 || index >= 360000) {
                //    std::cout << index << std::endl;
                //    std::cout << box_min << '\n' << std::endl;
                //    std::cout << box_max << '\n' << std::endl;
                //    exit(1);
                //}

                auto interpolated_color = interpolate(alpha, beta, gamma, tri.v0.color, tri.v1.color, tri.v2.color, 1);
                auto interpolated_normal = interpolate(alpha, beta, gamma, n[0], n[1], n[2], 1);
                auto interpolated_texcoords = interpolate(alpha, beta, gamma, tri.v0.uv, tri.v1.uv, tri.v2.uv, 1);
                auto interpolated_shadingcoords = interpolate(alpha, beta, gamma, pos_in_camera[0], pos_in_camera[1], pos_in_camera[2], 1);

                fragment_shader_payload payload(interpolated_color, interpolated_normal.normalized(), interpolated_texcoords, tri.material, texture, camera);
                payload.view_pos = interpolated_shadingcoords;

                auto pixel_color = fragment_shader(payload, light);

                {
                    std::lock_guard<std::mutex> lock(pixel_mutex[index]);

                    if (z_interpolated > depth_buffer[index])
                        continue;

                    depth_buffer[index] = z_interpolated;

                    setPixel(x, y, pixel_color);
                }
            }
        }
    }
}

namespace fs = std::filesystem;
void Rasterizer::save(
    const std::string& output_path,
    const std::string& filename
)
{
    fs::create_directory(output_path);
    std::string full_path = output_path + '/' + filename + ".png";

    // ==============================
    // 自动判断：0~1 还是 0~255
    // ==============================
    float max_val = 0.0f;
    for (const auto& color : frame_buffer) {
        max_val = std::max({ max_val, color[0], color[1], color[2] });
    }

    // 复制一份用于保存（避免修改原始数据）
    std::vector<Eigen::Vector3f> buffer = frame_buffer;

    // 如果最大值 <= 1.0f → 说明是 0~1 范围，需要放大到 0~255
    if (max_val <= 1.0f) {
        for (auto& color : buffer) {
            color *= 255.0f;
        }
    }
    // 如果 >1 → 已经是 0~255，什么都不用做

    cv::Mat image(height, width, CV_32FC3, buffer.data());
    image.convertTo(image, CV_8UC3, 1.0f);
    cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

    cv::imwrite(full_path, image);
}