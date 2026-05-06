#pragma once
#include <Eigen/Core>
#include <vector> 

//phong_fragment_shader
struct Material
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.1, 0.1, 0.1);   //0-1
    Eigen::Vector3f kd = Eigen::Vector3f(0.5, 0.5, 0.5);   //0-1
    Eigen::Vector3f ks = Eigen::Vector3f(0.7, 0.7, 0.7);   //0-1
    int shininess = 150;                                   //0-300
};

struct Vertex
{
    Eigen::Vector3f position;
    Eigen::Vector3f color;
    Eigen::Vector3f normal;
    Eigen::Vector2f uv;
};

struct Triangle
{
    Vertex v0;
    Vertex v1;
    Vertex v2;

    Material material;
};

constexpr float inf = std::numeric_limits<float>::infinity();

struct Mesh_AABB
{
    Eigen::Vector3f mesh_min = Eigen::Vector3f(inf, inf, inf);
    Eigen::Vector3f mesh_max = Eigen::Vector3f(-inf, -inf, -inf);
    Eigen::Vector3f center = Eigen::Vector3f(0, 0, 0);
    float radius = inf;
};

Mesh_AABB getMeshAABB(std::vector<Triangle>& mesh);

std::ostream& operator<<(
    std::ostream& os,
    const Vertex& v
    );

std::ostream& operator<<(
    std::ostream& os,
    const Material& m
    );

std::ostream& operator<<(
    std::ostream& os,
    const Triangle& tri
    );

