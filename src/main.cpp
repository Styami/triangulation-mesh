#include "include/meshStruct.hpp"

int main() {
    MeshStruct mesh("data/cube_maillage_triangles.off");
    //mesh.calculeLaplacian();
    //mesh.turn_around_point(5);
    //mesh.turn_around_point(5);
    mesh.addPoint(0, {0.15, 0.15, 0});
    mesh.save_mesh_off("data/test_addPoint");
    //std::cout << mesh;
    return 0;
}