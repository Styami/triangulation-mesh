#include "include/mesh.hpp"
#include "include/meshOpened.hpp"
#include <cmath>
#include <cstdlib>


int main() {
    MeshOpened mesh;
    mesh.loadMesh("data/alpes_random_1.txt");
    mesh.calculeLaplacian();
    mesh.save_mesh_obj("data/test");
    mesh.save_mesh_off("data/test");
    return 0;
}