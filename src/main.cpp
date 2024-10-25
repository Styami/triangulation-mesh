#include "include/meshStruct.hpp"
#include "include/point.hpp"
#include <cmath>
#include <cstdlib>


int main() {

#if 1
    MeshStruct mesh(false);
    mesh.loadOpenMesh("data/openMesh.off");
    mesh.addPoint({0.5, 0, 0.75});
    mesh.addPoint({1.0, 0, 0.5});

    mesh.addPoint({0.6, 0, 3.3});
    mesh.addPoint({9.5, 0, 9.35});
    mesh.addPoint({10.69, 0, 0.42});
    mesh.addPoint({2, 0, 7.42});
    mesh.addPoint({3, 0, 1.5});
    mesh.addPoint({2, 0, 2});
    mesh.addPoint({-10, 0, 0.92});
    mesh.addPoint({8, 0, 9.2});
    mesh.addPoint({3, 0, 5});
    mesh.addPoint({-1, 0, 3});
    
    //mesh.turn_around_point(2);
    mesh.save_mesh_off("data/test_addPoint");

#else

    MeshStruct mesh;
    mesh.addPoint({1, 0, 0.75});
    mesh.addPoint({1, 0, 1.25});
    mesh.addPoint({3, 0, -10});
    mesh.edgeFlip({4,2});
    std::random_device rng;
    std::mt19937 gen(rng());
    std::uniform_real_distribution<float> dis_x(0.5, 1.5);
    std::uniform_real_distribution<float> dis_y(0.5, 1);
    for(int i = 0; i < 10; i++) {
        mesh.addPoint({dis_x(gen), 0, dis_y(gen)});
    }
    mesh.save_mesh_off("data/test_basic");
#endif

    return 0;
}