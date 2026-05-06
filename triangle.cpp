#include "triangle.h"


Mesh_AABB getMeshAABB(std::vector<Triangle>& mesh)
{
    Eigen::Vector3f min_mesh(inf, inf, inf);
    Eigen::Vector3f max_mesh(-inf, -inf, -inf);

    for (const auto& tri : mesh)
    {
        auto p = tri.v0.position;
        min_mesh = min_mesh.cwiseMin(p);
        max_mesh = max_mesh.cwiseMax(p);
        p = tri.v1.position;
        min_mesh = min_mesh.cwiseMin(p);
        max_mesh = max_mesh.cwiseMax(p);
        p = tri.v2.position;
        min_mesh = min_mesh.cwiseMin(p);
        max_mesh = max_mesh.cwiseMax(p);
    }

    Eigen::Vector3f center = (min_mesh + max_mesh) * 0.5f;
    float radius = (max_mesh - center).norm();

    Mesh_AABB mesh_aabb{ min_mesh, max_mesh, center, radius };

    //std::cout << min_mesh << std::endl;
    //std::cout << max_mesh << std::endl;
    //std::cout << center << std::endl;
    //std::cout << radius << std::endl;

    return mesh_aabb;
}


std::ostream& operator<<(
    std::ostream& os,
    const Vertex& v
    )
{
    os
        << "Position  : ("
        << v.position.x() << ", "
        << v.position.y() << ", "
        << v.position.z() << ")\n"

        << "color\t  : ("
        << v.color.x() << ", "
        << v.color.y() << ", "
        << v.color.z() << ")\n"

        << "Normal\t  : ("
        << v.normal.x() << ", "
        << v.normal.y() << ", "
        << v.normal.z() << ")\n"

        << "UV\t  : ("
        << v.uv.x() << ", "
        << v.uv.y() << ")";

    return os;
}

std::ostream& operator<<(
    std::ostream& os,
    const Material& m
    )
{
    os
        << "ka\t  : ("
        << m.ka.x() << ", "
        << m.ka.y() << ", "
        << m.ka.z() << ")\n"

        << "kd\t  : ("
        << m.kd.x() << ", "
        << m.kd.y() << ", "
        << m.kd.z() << ")\n"

        << "ks\t  : ("
        << m.ks.x() << ", "
        << m.ks.y() << ", "
        << m.ks.z() << ")\n"

        << "shininess : ("
        << m.shininess << ")";

    return os;
}

// =========================================
// Triangle Output
// =========================================

std::ostream& operator<<(
    std::ostream& os,
    const Triangle& tri
    )
{
    os << "\nTriangle Face\n";

    os << "================================================\n";

    os << "v0 :\n";
    os << tri.v0 << "\n\n";

    os << "v1 :\n";
    os << tri.v1 << "\n\n";

    os << "v2 :\n";
    os << tri.v2 << "\n\n";

    os << "material  :\n";
    os << tri.material << "\n";

    os << "================================================";

    return os;
}