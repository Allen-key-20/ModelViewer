#pragma once
#include <Eigen/Eigen>


inline std::ostream& operator<<(
    std::ostream& os,
    Eigen::Vector4f& vector
    )
{
    os
        << "("
        << vector.x() << ", "
        << vector.y() << ", "
        << vector.z() << ", "
        << vector.w() << ")";

    return os;
}

inline std::ostream& operator<<(
    std::ostream& os,
    Eigen::Vector3f& vector
    )
{
    os
        << "(" 
        << vector.x() << ", "
        << vector.y() << ", "
        << vector.z() << ")";

    return os;
}

inline std::ostream& operator<<(
    std::ostream& os,
    Eigen::Vector2f& vector
    )
{
    os
        << "("
        << vector.x() << ", "
        << vector.y() << ")";

    return os;
}


