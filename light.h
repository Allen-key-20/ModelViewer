#pragma once
#include <Eigen/Core>

class Light
{
public:
    Eigen::Vector3f position = Eigen::Vector3f(2.0f, 2.0f, 2.0f);

    Eigen::Vector3f intensity = Eigen::Vector3f(50.0f, 50.0f, 50.0f);  //10 - 100

    float amb_intensity = 1.0f;

    float shininess = 150;

    void setIntensity(float r, float g, float b) {
        intensity = Eigen::Vector3f(r, g, b);
    }
    void setAmbIntensity(float ambIntensity) {
        intensity = Eigen::Vector3f(ambIntensity, ambIntensity, ambIntensity);
    }
    void setPosition(float x, float y, float z) {
        position = Eigen::Vector3f(x, y, z);
    }

    void reset() {
        position = Eigen::Vector3f(2.0f, 2.0f, 2.0f);
        intensity = Eigen::Vector3f(50.0f, 50.0f, 50.0f);
        amb_intensity = 1.0f;
        shininess = 150;
    }
};