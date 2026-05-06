#pragma once
#include <Eigen/Eigen>
#include "texture.h"
#include "triangle.h"
#include "camera.h"
#include "light.h"


struct fragment_shader_payload
{
    fragment_shader_payload(const Eigen::Vector3f& col, const Eigen::Vector3f& nor, const Eigen::Vector2f& tc, const Material& mtl, const Texture& tex, Camera& cam) :
        color(col), normal(nor), uv(tc), material(mtl), texture(tex), camera(cam){}

    Eigen::Vector3f view_pos;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f uv;
    const Material& material;
    const Texture& texture;
    Camera& camera;
};

Eigen::Vector3f fragment_shader(fragment_shader_payload& payload, Light& light);


