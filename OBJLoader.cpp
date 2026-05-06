#define TINYOBJLOADER_IMPLEMENTATION

#include "OBJLoader.h"

#include <iostream>
#include <stdexcept>

Vertex OBJLoader::getVertex(
    const tinyobj::attrib_t& attrib,
    const tinyobj::index_t& idx
)
{
    Vertex v;

    // =========================================
    // Position
    // =========================================

    if (idx.vertex_index >= 0)
    {
        v.position =
            Eigen::Vector3f(
                attrib.vertices[
                    3 * idx.vertex_index + 0
                ],
                attrib.vertices[
                    3 * idx.vertex_index + 1
                ],
                        attrib.vertices[
                            3 * idx.vertex_index + 2
                        ]
                        );
    }
    else
    {
        v.position =
            Eigen::Vector3f(0, 0, 0);
    }


    // =========================================
    // Color
    // =========================================

    if (idx.vertex_index >= 0 && !attrib.colors.empty())
    {
        v.color = Eigen::Vector3f(
            attrib.colors[3 * idx.vertex_index + 0],            
            attrib.colors[3 * idx.vertex_index + 1], 
            attrib.colors[3 * idx.vertex_index + 2]);
    }
    else
    {
        // 128a → 128.0f，默认灰色（0~1浮点格式）
        v.color = Eigen::Vector3f(0.5f, 0.5f, 0.5f);
    }

    // =========================================
    // Normal
    // =========================================

    if (
        idx.normal_index >= 0 &&
        !attrib.normals.empty()
        )
    {
        v.normal =
            Eigen::Vector3f(
                attrib.normals[
                    3 * idx.normal_index + 0
                ],
                attrib.normals[
                    3 * idx.normal_index + 1
                ],
                        attrib.normals[
                            3 * idx.normal_index + 2
                        ]
                        );
    }
    else
    {
        v.normal =
            Eigen::Vector3f(0, 0, 0);
    }

    // =========================================
    // UV
    // =========================================

    if (
        idx.texcoord_index >= 0 &&
        !attrib.texcoords.empty()
        )
    {
        v.uv =
            Eigen::Vector2f(
                attrib.texcoords[
                    2 * idx.texcoord_index + 0
                ],
                attrib.texcoords[
                    2 * idx.texcoord_index + 1
                ]
                        );
    }
    else
    {
        v.uv =
            Eigen::Vector2f(0, 0);
    }

    return v;
}

std::vector<Triangle>
OBJLoader::load(
    const std::string& model_path,
    const std::string& model_name
)
{
    // =========================================
    // tinyobj containers
    // =========================================

    tinyobj::attrib_t attrib;

    std::vector<tinyobj::shape_t> shapes;

    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;

    // =========================================
    // Load OBJ
    // =========================================
    const std::string& model = model_path + '/' + model_name + ".obj";
    bool success =
        tinyobj::LoadObj(
            &attrib,
            &shapes,
            &materials,
            &warn,
            &err,
            model.c_str(),
            model_path.c_str(),
            true
        );

    // =========================================
    // Error check
    // =========================================
    if (!warn.empty())
    {
        std::cout
            << warn
            << std::endl;
    }
    if (!err.empty())
    {
        std::cout
            << err
            << std::endl;
    }

    if (!success)
    {
        throw std::runtime_error(
            "Failed to load OBJ."
        );
    }

    // =========================================
    // Triangle List
    // =========================================

    std::vector<Triangle> triangles;

    // =========================================
    // Traverse shapes
    // =========================================

    for (const auto& shape : shapes)
    {
        size_t indexOffset = 0;

        // =========================================
        // Traverse faces
        // =========================================

        for (
            size_t f = 0;
            f < shape.mesh.num_face_vertices.size();
            f++
            )
        {
            int fv =
                shape.mesh.num_face_vertices[f];

            // triangulate=true
            // theoretically fv should be 3

            if (fv != 3)
            {
                indexOffset += fv;
                continue;
            }

            Triangle tri;

            // =========================================
            // material
            // =========================================
            int mat_id = shape.mesh.material_ids[f];

            if (mat_id >= 0 && mat_id < materials.size())
            {
                // 有材质 → 用 mtl, 否则默认值
                const auto& mat = materials[mat_id]; // tinyobj 材质

                tri.material.ka = Eigen::Vector3f(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
                tri.material.kd = Eigen::Vector3f(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
                tri.material.ks = Eigen::Vector3f(mat.specular[0], mat.specular[1], mat.specular[2]);
                tri.material.shininess = mat.shininess;
            }

            // =========================================
            // 3 vertices
            // =========================================

            for (int v = 0; v < 3; v++)
            {
                tinyobj::index_t idx =
                    shape.mesh.indices[
                        indexOffset + v
                    ];

                Vertex vertex =
                    getVertex(
                        attrib,
                        idx
                    );

                if (v == 0)
                {
                    tri.v0 = vertex;
                }

                if (v == 1)
                {
                    tri.v1 = vertex;
                }

                if (v == 2)
                {
                    tri.v2 = vertex;
                }
            }

            triangles.push_back(tri);

            indexOffset += fv;
        }
    }

    //std::cout
    //    << "Loaded Triangles: "
    //    << triangles.size()
    //    << std::endl;

    return triangles;
}