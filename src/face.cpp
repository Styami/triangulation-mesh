#include "include/face.hpp"
#include "include/sommet.hpp"

Face::Face() :
    sommets(0)
{}

Face::Face(const indiceGlobalSommet a, const indiceGlobalSommet b, const indiceGlobalSommet c) :
    sommets({a, b, c})
{}

Face::Face(const std::vector<indiceGlobalSommet>& _sommets) :
    sommets(_sommets)
{}

void Face::setNewOppositeVertexPoint(const indiceFace previousOppFace, const indiceFace newOppFace) {
    for (indiceFace& i : indexesOfFacePerVertex) {
        if(i == previousOppFace) {
            i = newOppFace;
            return;
        }
    }
}