#pragma once
#include <Eigen/Eigen>

class Camera
{
public:

    Camera(Eigen::Vector3f look_target, float target_distance);

    // ===== Camera Parameters =====

    Eigen::Vector3f target;

    float yaw;
    float pitch;

    float distance;
    float origin_distance;

    float fov;
    float aspect_ratio;

    float z_near;
    float z_far;

    float model_yaw;
    float model_pitch;

    Eigen::Matrix4f mm;//ModelMatrix;
    Eigen::Matrix4f vm;//ViewMatrix;
    Eigen::Matrix4f pjm;//ProjectionMatrix;
    Eigen::Matrix4f mvm;//MVMatrix;
    Eigen::Matrix4f mvpm;//MVPMatrix;

    Eigen::Vector3f position;

    // ===== Matrix =====

    void setModelMatrix();

    void setViewMatrix();

    void setProjectionMatrix();

    void setMVMatrix();

    void setMVPMatrix();

    void setAll();

    void reset();

    // ===== Camera Control =====

    void setPosition();

    void rotate(float delta_yaw, float delta_pitch);

    void zoom(float delta);

    void pan(float delta_x, float delta_y);

private:

    Eigen::Matrix4f lookAt(
        const Eigen::Vector3f& eye,
        const Eigen::Vector3f& center,
        const Eigen::Vector3f& up
    ) const;
};