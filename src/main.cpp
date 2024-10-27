#include "include/meshOpened.hpp"


int main() {
    MeshOpened mesh;
    mesh.loadMesh("data/noise_random_2.txt");
    mesh.calculeLaplacian();
    mesh.save_mesh_obj("data/test");
    mesh.save_mesh_off("data/test");
    return 0;
}