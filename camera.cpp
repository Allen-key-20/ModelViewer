#include "camera.h"
#include <cmath>

constexpr float MY_PI = 3.1415926f;

Camera::Camera(Eigen::Vector3f look_target, float target_distance)
{
    target = look_target;

    yaw = 0.0f;
    pitch = 0.0f;

    distance = target_distance;

    origin_distance = target_distance;

    fov = 45.0f;
    aspect_ratio = 1.0f;

    z_near = 0.1f;
    z_far = 100.0f;

    model_yaw = 150.0f;
    model_pitch = 0.0f;

    setAll();
}

void Camera::reset()
{
    yaw = 0.0f;
    pitch = 0.0f;

    model_yaw = 150.0f;
    model_pitch = 0.0f;

    distance = origin_distance;

    setAll();
}

void Camera::setPosition()
{
    Eigen::Vector3f pos;

    float yaw_rad = yaw * MY_PI / 180.0f;
    float pitch_rad = pitch * MY_PI / 180.0f;

    pos.x() =
        target.x() +
        distance * std::cos(pitch_rad) * std::sin(yaw_rad);

    pos.y() =
        target.y() +
        distance * std::sin(pitch_rad);

    pos.z() =
        target.z() +
        distance * std::cos(pitch_rad) * std::cos(yaw_rad);

    position = pos;

}

Eigen::Matrix4f Camera::lookAt(const Eigen::Vector3f& eye, const Eigen::Vector3f& center, const Eigen::Vector3f& up) const
{
    Eigen::Vector3f z = (eye - center).normalized();

    Eigen::Vector3f x = up.cross(z).normalized();

    Eigen::Vector3f y = z.cross(x).normalized();

    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    view(0, 0) = x.x();
    view(0, 1) = x.y();
    view(0, 2) = x.z();
    view(0, 3) = -x.dot(eye);

    view(1, 0) = y.x();
    view(1, 1) = y.y();
    view(1, 2) = y.z();
    view(1, 3) = -y.dot(eye);

    view(2, 0) = z.x();
    view(2, 1) = z.y();
    view(2, 2) = z.z();
    view(2, 3) = -z.dot(eye);

    return view;
}

void Camera::setModelMatrix()
{
    //mm = Eigen::Matrix4f::Identity();
    float yaw_rad = model_yaw * MY_PI / 180.0f;

    float pitch_rad = model_pitch * MY_PI / 180.0f;

    // =========================
    // YÖáÐý×ª
    // =========================
    Eigen::Matrix4f rot_y = Eigen::Matrix4f::Identity();

    rot_y(0, 0) = cos(yaw_rad);
    rot_y(0, 2) = sin(yaw_rad);

    rot_y(2, 0) = -sin(yaw_rad);
    rot_y(2, 2) = cos(yaw_rad);

    // =========================
    // XÖáÐý×ª
    // =========================
    Eigen::Matrix4f rot_x = Eigen::Matrix4f::Identity();

    rot_x(1, 1) = cos(pitch_rad);
    rot_x(1, 2) = -sin(pitch_rad);

    rot_x(2, 1) = sin(pitch_rad);
    rot_x(2, 2) = cos(pitch_rad);

    // =========================
    // ×îÖÕModel Matrix
    // =========================
    mm = rot_y * rot_x;

    setViewMatrix();
    setProjectionMatrix();
    setMVMatrix();
    setMVPMatrix();
}

void Camera::setViewMatrix()
{
    Eigen::Vector3f eye = position;

    Eigen::Vector3f up(0.0f, 1.0f, 0.0f);

    vm = lookAt(eye, target, up);
}

void Camera::setProjectionMatrix()
{
    float radian = fov / 180.0f * MY_PI;

    float tan_half_fov = std::tan(radian / 2.0f);

    Eigen::Matrix4f projection =
        Eigen::Matrix4f::Zero();

    projection(0, 0) =
        1.0f / (aspect_ratio * tan_half_fov);

    projection(1, 1) =
        1.0f / tan_half_fov;

    projection(2, 2) =
        -(z_far + z_near) / (z_far - z_near);

    projection(2, 3) =
        -(2.0f * z_far * z_near) / (z_far - z_near);

    projection(3, 2) = -1.0f;

    pjm = projection;
}

void Camera::setMVMatrix()
{
    mvm = vm * mm;
}

void Camera::setMVPMatrix()
{
    mvpm = pjm * vm * mm;
}

void Camera::setAll()
{
    setPosition();
    setModelMatrix();
}

void Camera::rotate(float delta_yaw, float delta_pitch)
{
    yaw += delta_yaw;

    pitch += delta_pitch;

    pitch = std::clamp(pitch, -89.0f, 89.0f);


    setPosition();
    setViewMatrix();
    setProjectionMatrix();
    setMVMatrix();
    setMVPMatrix();
}

void Camera::zoom(float delta)
{
    distance -= delta;

    if (distance > 10.0f)
        distance = 10.0f;

    if (distance < 0.1f)
        distance = 0.1f;

    setAll();
}

void Camera::pan(float delta_x, float delta_y)
{
    Eigen::Vector3f eye = position;

    Eigen::Vector3f forward =
        (target - eye).normalized();

    Eigen::Vector3f up(0.0f, 1.0f, 0.0f);

    Eigen::Vector3f right =
        forward.cross(up).normalized();

    Eigen::Vector3f camera_up =
        right.cross(forward).normalized();

    target +=
        right * delta_x +
        camera_up * delta_y;

    setAll();
}