#pragma once
#include "OBJLoader.h"
#include "eigen_handle.h"
//#include "rasterizer.h"
#include "viewer.h"

#define image_height  600
#define image_width   900

int main(int argc, const char** argv){

    //std::string filename = "bunny";
    //std::string filename = "rock";
    std::string filename = "spot";
    std::string obj_path = "./models";
    std::string output_path = "./output";

    //auto mesh = OBJLoader::load(obj_path, filename);
    //Texture texture(obj_path, filename);
    //Mesh_AABB mesh_aabb = getMeshAABB(mesh);
    // 
    //std::cout << mesh_aabb.mesh_max << '\n' << std::endl;
    //std::cout << mesh_aabb.mesh_min << '\n' << std::endl;
    //std::cout << mesh_aabb.center << '\n' << std::endl;
    //std::cout << mesh_aabb.radius << '\n' << std::endl;
    //Camera camera(mesh_aabb.center, mesh_aabb.radius * 3.0f);

    //Light light;

    //std::cout << "Triangle Count: " << mesh.size() << std::endl;
    //std::cout << mesh[0] << std::endl;
    //std::cout << texture.isValid() << std::endl;
    //std::cout << mesh[0].v0.normal.x() << std::endl;

    //Rasterizer rasterizer(image_height, image_width);
    //rasterizer.clear();
    //rasterizer.draw(mesh, texture, camera, light);
    //rasterizer.save(output_path, filename);

    Viewer viewer(image_height, image_width, obj_path, filename);

    viewer.run();

    return 0;
}