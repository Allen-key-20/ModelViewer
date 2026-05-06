#include "fragment_shader.h"


Eigen::Vector3f fragment_shader(fragment_shader_payload& payload, Light& light)
{
    Eigen::Vector3f& view_pos = payload.view_pos;
    Eigen::Vector3f& color = payload.color;
    Eigen::Vector3f& normal = payload.normal;
    Eigen::Vector2f& uv = payload.uv;
    const Material& material = payload.material;
    const Texture& texture = payload.texture;
    Camera& camera = payload.camera;

    Eigen::Vector3f ka = material.ka;//.array() *color.array();
    Eigen::Vector3f kd = material.kd;// .array()* color.array();
    Eigen::Vector3f ks = material.ks;//.array() * color.array();
    //float p = material.shininess;
    float p = light.shininess;

    if (texture.isValid())
        color = texture.getColor(uv.x(), uv.y()) / 255.0f;

    kd = kd.cwiseProduct(color);

    Eigen::Vector3f eye_pos = camera.position;

    Eigen::Vector3f result_color = { 0, 0, 0 };
    Eigen::Vector3f light_amb_intensity(light.amb_intensity, light.amb_intensity, light.amb_intensity);
    Eigen::Vector3f kai = ka.cwiseProduct(light_amb_intensity);

    Eigen::Vector3f light_direction = (light.position - view_pos).normalized();
    Eigen::Vector3f eye_direction = (eye_pos - view_pos).normalized();

    Eigen::Vector3f point_intensity = light.intensity * (1.0f / (light.position - view_pos).squaredNorm());

    Eigen::Vector3f kdi = kd.cwiseProduct(point_intensity) * std::max(0.0f, normal.dot(light_direction));

    Eigen::Vector3f ksi = ks.cwiseProduct(point_intensity) * std::pow(std::max(0.0f, normal.dot((eye_direction + light_direction).normalized())), p);

    result_color += (kai + kdi + ksi);

    result_color = result_color.cwiseMin(1.0f).cwiseMax(0.0f);

    return result_color;
}