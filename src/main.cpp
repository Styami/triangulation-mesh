#include "include/meshStruct.hpp"

int main() {
    MeshStruct mesh("data/cube_maillage_triangles.off");
    mesh.addPoint(0, {0.75, 0.15, 0});
    mesh.addPoint(0, {0.95, 0.35, 0});
    mesh.addPoint(13, {0.6, 0.3, 0});
    mesh.edgeSplit(6, 2, 0.5);
    mesh.edgeFlip(6, 8);
    mesh.edgeFlip(8, 6);
    mesh.turn_around_point(2);
    mesh.save_mesh_off("data/test_addPoint");
    return 0;
}