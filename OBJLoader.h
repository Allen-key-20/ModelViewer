#pragma once
#include <string>
#include <vector>
#include <Eigen/Core>
#include "triangle.h"
#include "tiny_obj_loader.h"


class OBJLoader
{
public:

    static std::vector<Triangle> load(
        const std::string& model_path,
        const std::string& model_name
    );

private:

    static Vertex getVertex(
        const tinyobj::attrib_t& attrib,
        const tinyobj::index_t& idx
    );
};