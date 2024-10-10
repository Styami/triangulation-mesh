#ifndef __FACE__
#define __FACE__
#include "sommet.hpp"
#include <vector>

typedef size_t indiceGlobalSommet;
typedef size_t indexesFaceOppSommet;

class Face {
    public:
        Face();
        Face(const indiceGlobalSommet a, const indiceGlobalSommet b, const indiceGlobalSommet c);
        Face(const std::vector<indiceGlobalSommet>& _sommets);
        void setNewOppositeVertexPoint(const indiceFace previousOppFace, const indiceFace newOppFace);
        std::vector<indexesFaceOppSommet> indexesOfFacePerVertex;
        std::vector<indiceGlobalSommet> sommets;
    private:
};

#endif