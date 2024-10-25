#include "include/face.hpp"
#include <cstddef>
#include <stdexcept>
#include <utility>

Face::Face() :
    sommets(0)
{}

Face::Face(const indiceGlobalSommet a, const indiceGlobalSommet b, const indiceGlobalSommet c) :
    sommets({a, b, c})
{}

Face::Face(const std::vector<indiceGlobalSommet>& _sommets) :
    sommets(_sommets)
{}

Face::Face(const std::vector<indiceGlobalSommet>& _sommets, const std::vector<indexesFaceOppSommet> &_oppositeFaces) :
    indexesOfFacePerVertex(_oppositeFaces),
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

const arete Face::getOppositeEdge() {
    for(size_t i = 0; i < sommets.size(); i++) {
        if(sommets[i] == 0) return std::make_pair(sommets[(i + 1) % 3], sommets[(i + 2) % 3]);
    }
    throw std::runtime_error(
        "Ce n'est pas une face infini!!!"
    );
}

const indiceGlobalSommet& Face::getOppositeVertex(const arete& edge) const {
    for (size_t i = 0; i < sommets.size(); i++) {
        if((sommets[i] == edge.first && sommets[(i + 1) % 3] == edge.second)
            || (sommets[i] == edge.second && sommets[(i + 1) % 3] == edge.first)) {
                    return sommets[(i + 2) % 3];
                }
    }
    throw std::runtime_error(
        "Le sommets opposé n'a pas été trouvé"
    );
}