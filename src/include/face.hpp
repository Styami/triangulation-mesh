#ifndef __FACE__
#define __FACE__
#include "sommet.hpp"
#include <cstddef>
#include <vector>

typedef size_t indiceGlobalSommet;
typedef size_t indexesFaceOppSommet;
typedef std::pair<indiceGlobalSommet, indiceGlobalSommet> arete;
typedef std::pair<indiceFace, indiceGlobalSommet> pairIndiceFaceSommet;

class Face {
    public:
        Face();
        Face(const indiceGlobalSommet a, const indiceGlobalSommet b, const indiceGlobalSommet c);
        Face(const std::vector<indiceGlobalSommet>& _sommets);
        Face(const std::vector<indiceGlobalSommet>& _sommets, const std::vector<indexesFaceOppSommet> &_oppositeFaces);
        void setNewOppositeVertexPoint(const indiceFace previousOppFace, const indiceFace newOppFace);
        std::vector<indexesFaceOppSommet> indexesOfFacePerVertex;
        std::vector<indiceGlobalSommet> sommets;
        /**
            Cette fonction permet de récupérer l'arrête opposée à notre sommet infini.
            /!\ Cette fonction est à utiliser uniquement pour les faces infinies.
            @return arete 
        */
        const arete getOppositeEdge();

        /**
            Récupère la vertex opposée à l'arête passée en paramètre
            @param arete
            @return indiceGlobalSommet
        */
        const indiceGlobalSommet& getOppositeVertex(const arete& edge) const;

        inline bool isInfinite() const;
    private:
};

bool Face::isInfinite() const {
    for (const size_t& s : sommets) {
        if(s == 0)
            return true;
    }
    return false;
}

#endif