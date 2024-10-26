#include "include/mesh.hpp"
#include <cassert>
#include <iostream>
#include <chrono>


Mesh::Mesh(const Sommet& a, const Sommet& b, const Sommet& c) {
    sommets.reserve(4);
    sommets.push_back(Sommet());
    sommets.push_back(a);
    sommets.push_back(b);
    sommets.push_back(c);
    Face newFace(1, 2, 3);
    faces.reserve(4);
    faces.push_back(newFace);
    faces.push_back({0, 1, 3});
    faces.push_back({0, 3, 2});
    faces.push_back({0, 2, 1});
    newFace.neighboursFaces = {2, 1, 3};
    faces[1].neighboursFaces = {0, 1, 3};
    faces[2].neighboursFaces = {0, 2, 1};
    faces[3].neighboursFaces = {0, 1, 2};
}

Mesh::Mesh() :
    numberOfEdge(0)
{}

std::ostream& operator<<(std::ostream& os, const Mesh& mesh) {
    for(size_t numFace = 0; numFace < mesh.faces.size(); numFace++) {
        for(size_t numLocalSommet = 0; numLocalSommet < mesh.faces[numFace].sommets.size(); numLocalSommet++) {
            os  << "face " << numFace 
                << "/ sommet local " << numLocalSommet 
                << " : " << mesh.faces[numFace].neighboursFaces[numLocalSommet] << '\n';
        }
    }
    return os;
}


void Mesh::turn_around_point(const size_t indexVertex, const Sens rotate) {
    const Sommet& s = sommets[indexVertex];
    size_t firstFace = s.getFaceIndex();
    size_t currentFace = firstFace;

    do {
        colorFace[currentFace] = Color(255, 0, 0);
        if(rotate == Sens::DROITE)
            currentFace = nextFaceD(currentFace, indexVertex);
        else
            currentFace = nextFaceG(currentFace, indexVertex);
    }while (currentFace != firstFace);
}


size_t Mesh::nextFaceG(const indiceFace currentFaceId, const size_t aroundPoint) {
    int id = 0;
    const Face& currentFace = faces[currentFaceId];
    while (currentFace.sommets[(id + 1) % 3] != aroundPoint) {
        id++;
        if(id > 3)
            throw std::runtime_error(
                "Le sommet autour du quel je tourne n'est pas trouvé"
            );
    }
    return currentFace.neighboursFaces[id];
}

size_t Mesh::nextFaceD(const indiceFace currentFaceId, const size_t aroundPoint) {
    int id = 0;
    const Face& currentFace = faces[currentFaceId];
    while (currentFace.sommets[(id + 2) % 3] != aroundPoint) {
        id++;
        if(id > 3)
            throw std::runtime_error(
                "Le sommet autour du quel je tourne n'est pas trouvé"
            );
    }
    return currentFace.neighboursFaces[id];
}

Vecteur Mesh::calculNormalTriangle(const Face& face) const {
    const std::vector<indiceGlobalSommet>& indexesPoint = face.sommets;
    Vecteur a = Vecteur(sommets[indexesPoint[0]].getPoint(), sommets[indexesPoint[1]].getPoint());
    Vecteur b = Vecteur(sommets[indexesPoint[0]].getPoint(), sommets[indexesPoint[2]].getPoint());
    return cross(a, b);     
}

std::tuple<float, float, const Point&> Mesh::calculCotanFaceV(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace) {
    std::vector<indiceGlobalSommet> localVertex = faces[currentFace].sommets;
    size_t id = 0; 
    if(localVertex[(0+1)%3] == indexeTerminal) id = 0;
    if(localVertex[(1+1)%3] == indexeTerminal) id = 1;
    if(localVertex[(2+1)%3] == indexeTerminal) id = 2;
    
    const Point& currentPoint = sommets[localVertex[id]].getPoint();
    const Point& nextPoint = sommets[localVertex[(id + 1) % 3]].getPoint();
    const Point& lastPoint = sommets[localVertex[(id + 2) % 3]].getPoint();
    Vecteur a = Vecteur(currentPoint, lastPoint);
    Vecteur b = Vecteur(currentPoint, nextPoint);
    float cos = dot(normalize(a), normalize(b));
    float sin = length(cross(normalize(a), normalize(b)));
    float area = length(cross(a, b)) / 2;
    float cotan = cos/sin;
    return {cotan, area, lastPoint};
}


std::tuple<float, float, const Point&> Mesh::calculCotanFaceF(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace) {
    std::vector<indiceFace> adjFace = faces[currentFace].neighboursFaces;
    std::vector<indiceFace> localVertex = faces[currentFace].sommets;
    size_t id = 0; 
    if(adjFace[0] == indexeTerminal) id = 0;
    if(adjFace[1] == indexeTerminal) id = 1;
    if(adjFace[2] == indexeTerminal) id = 2;
    

    const Point& currentPoint = sommets[localVertex[id]].getPoint();
    const Point& nextPoint = sommets[localVertex[(id + 1) % 3]].getPoint();
    const Point& lastPoint = sommets[localVertex[(id + 2) % 3]].getPoint();
    Vecteur a = Vecteur(currentPoint, lastPoint);
    Vecteur b = Vecteur(currentPoint, nextPoint);
    float cos = dot(normalize(a), normalize(b));
    float sin = length(cross(normalize(a), normalize(b)));
    float area = length(cross(a, b)) / 2;
    float cotan = cos/sin;
    return {cotan, area, lastPoint};
}

void Mesh::calculeLaplacian() {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    normals = std::vector<Vecteur>(sommets.size(), Vecteur(0));
    courbures = std::vector<float>(sommets.size());

    for (size_t indexVertex = 0; indexVertex < sommets.size(); indexVertex++) {
        Sommet s = sommets[indexVertex];
        int firstFace = s.getFaceIndex();
        int currentFace = firstFace;
        Vecteur normalFirstFace = calculNormalTriangle(faces[firstFace]);
        Vecteur acc(Point(0), Point(0));
        float finalArea = 0;

        // on commence à boucler pour calculer le laplacien autour du vertex
        do {
            auto [cotan1, area1, nextPoint] = calculCotanFaceV(faces[firstFace].sommets, indexVertex, currentFace);
            int indiceNextFace = nextFaceD(currentFace, indexVertex);
            auto [cotan2, area2, _] = calculCotanFaceF(faces[indiceNextFace].neighboursFaces, currentFace, indiceNextFace);
            
            acc.x += (cotan1 + cotan2) * (nextPoint.x - s.getPoint().x);
            acc.y += (cotan1 + cotan2) * (nextPoint.y - s.getPoint().y);
            acc.z += (cotan1 + cotan2) * (nextPoint.z - s.getPoint().z);
            
            finalArea += area1/3;
            currentFace = indiceNextFace;
            
        }while (currentFace != firstFace);
        Vecteur normalPoint = (1 / (2 * finalArea)) * acc;
        float tmp = length(normalPoint)/2;
        courbure_max = tmp > courbure_max ? tmp:courbure_max; 
        courbure_min = tmp < courbure_min ? tmp:courbure_min; 
        courbures[indexVertex] = tmp;
        normals[indexVertex] = dot(normalPoint, normalFirstFace) > 0 ? normalPoint:-normalPoint;
    }
    std::cout << "temps de calcul du laplacien : " 
    << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin)
    << std::endl;
}
