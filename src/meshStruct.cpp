#include "include/meshStruct.hpp"
#include "include/face.hpp"
#include "include/sommet.hpp"
#include "include/vecteur.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <chrono>
#include <map>
#include <fstream>
#include <queue>
#include <stdexcept>
#include <vector>


inline bool existingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces) {
    return (edgesFaces.find(std::pair(edge.second, edge.first)) != edgesFaces.end())
            || edgesFaces.find(edge) != edgesFaces.end();
}

inline pairIndiceFaceSommet getExistingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces) {
    return edgesFaces.find(edge) != edgesFaces.end() ? 
    edgesFaces.at(edge):
    edgesFaces.at(std::pair(edge.second, edge.first));
}

float MeshStruct::orientationTest(const Sommet& a, const Sommet& b, const Sommet& c) const {
    Vecteur ab = b.getPoint() - a.getPoint();
    Vecteur ac = c.getPoint() - a.getPoint();
    float det = ab.x * ac.z - ac.x * ab.z;
    return det;
}

int MeshStruct::inTriangle(const Face& face, const Point& p) const {
    const std::vector<indiceGlobalSommet>& trianglePoints = face.sommets;
    float res = 0;
    for (int i = 0; i < 3; i++) {
        float tmp = orientationTest(sommets[trianglePoints[i % 3]], sommets[trianglePoints[(i + 1) % 3]], p);
        if(tmp > 0) {
            res++;
        } else if (tmp < 0) {
            return - 1;
        }
    }
    if(res == 0) return 0;
    return 1;
}

std::pair<bool, indiceFace> MeshStruct::inConvexHull(const Point& p) const {
    for (size_t numFace = 0; numFace < faces.size(); numFace++) {
        const Face& currentFace = faces[numFace];
        //vérifie que ce n'es pas un triangle de la face infinie
        if(!(currentFace.sommets[0] != 0 && currentFace.sommets[1] != 0 && currentFace.sommets[2] != 0)) continue;

        if(inTriangle(faces[numFace], p) >= 0) {
            return {true, numFace} ;
        }
    }
    return {false, - 1};
}

MeshStruct::MeshStruct()
: isClosed(false)
{
    sommets = {
        {{0,-100,0}, 2},
        {{1,0,0},0},
        {{2,0,1},1},
        {{1,0,2},4},
        {{0,0,1},5},
    };
    faces = {
        {{4,1,2},{3,1,2}},
        {{2,3,4},{5,0,4}},
        {{0,1,4},{0,5,3}},
        {{1,0,2},{4,0,2}},
        {{2,0,3},{5,1,3}},
        {{3,0,4},{2,1,4}},
    };
    colorFace = std::vector<Color>(6, Color());
}

MeshStruct::MeshStruct(const Sommet& a, const Sommet& b, const Sommet& c, const bool _isClosed) :
    isClosed(_isClosed)
{
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
    newFace.indexesOfFacePerVertex = {2, 1, 3};
    faces[1].indexesOfFacePerVertex = {0, 1, 3};
    faces[2].indexesOfFacePerVertex = {0, 2, 1};
    faces[3].indexesOfFacePerVertex = {0, 1, 2};
}

MeshStruct::MeshStruct(const bool _isClosed) :
    numberOfEdge(0),
    isClosed(_isClosed)
{}

void MeshStruct::loadOpenMesh(const std::string& file_name) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::ifstream myfile (file_name);
    Point p1;
    Point p;
    Point p3;
    int nbSommets;
    int nbFaces;
    std::map<arete, pairIndiceFaceSommet> map;
    Sommet newVertex;
    sommets.push_back(Sommet());
    sommets[0].setIndexeFace(1);

    if(myfile.is_open()) {
        myfile.ignore(4);
        myfile >> nbSommets >> nbFaces;
        myfile.ignore(1000, '\n');
        myfile >> p.x >> p.y >> p.z;
        sommets.push_back(p);
        sommets[1].setIndexeFace(0);
        myfile >> p.x >> p.y >> p.z;
        sommets.push_back(p);
        sommets[2].setIndexeFace(0);
        myfile >> p.x >> p.y >> p.z;
        sommets.push_back(p);
        sommets[3].setIndexeFace(0);
        if(orientationTest(sommets[1], sommets[2], sommets[3])) {
            faces.push_back({1, 2, 3});
            faces[0].indexesOfFacePerVertex = {2, 1, 3};
            faces.push_back({0, 1, 3});
            faces[1].indexesOfFacePerVertex = {0, 2, 3};
            faces.push_back({0, 3, 2});
            faces[2].indexesOfFacePerVertex = {0, 3, 1};
            faces.push_back({0, 2, 1});
            faces[3].indexesOfFacePerVertex = {0, 1, 2};
        } else {
            faces.push_back({1, 3, 2});
            faces[0].indexesOfFacePerVertex = {2, 1, 3};
            faces.push_back({0, 1, 2});
            faces[1].indexesOfFacePerVertex = {0, 2, 3};
            faces.push_back({0, 2, 3});
            faces[2].indexesOfFacePerVertex = {0, 3, 1};
            faces.push_back({0, 3, 1});
            faces[3].indexesOfFacePerVertex = {0, 1, 2};
        }
        colorFace = std::vector(4, Color());

        numberOfEdge += 6;
        for(int i = 3; i < nbSommets; i++) {
            myfile >> p.x >> p.y >> p.z;
            addPoint(p);
        }
        std::cout << "temps d'importation du mesh en : "
        << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin)
        << std::endl;
        myfile.close();
    } else {
        std::cerr << "erreur lors de l'ouverture du fichier.";
        return;
    }
}

void MeshStruct::loadCloseMesh(const std::string& file_name) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    Point p;
    std::ifstream myfile (file_name);
    size_t nbSommets;
    size_t nbFaces;
    std::map<arete, pairIndiceFaceSommet> map;

    if(myfile.is_open()) {
        myfile.ignore(4);
        myfile >> nbSommets >> nbFaces;
        myfile.ignore(1000, '\n');
        for(size_t i = 0; i < nbSommets; i++) {
            myfile >> p.x >> p.y >> p.z;
            sommets.push_back(p);
        }
        for (indiceFace currentFace = 0; currentFace < nbFaces; currentFace++) {
            int numberOfVertexPerFace = 0;
            myfile >> numberOfVertexPerFace;
            
            std::vector<indiceGlobalSommet> vertexFace(numberOfVertexPerFace, 0); 

            myfile >> vertexFace[0] >> vertexFace[1] >> vertexFace[2];
            faces.push_back(Face(vertexFace));
            colorFace.push_back(Color());

            faces[currentFace].indexesOfFacePerVertex = std::vector<indexesFaceOppSommet>(numberOfVertexPerFace, -1);

            for(int numLocalVertex = 0; numLocalVertex < numberOfVertexPerFace; numLocalVertex++) {
                arete edge = std::pair(vertexFace[numLocalVertex], vertexFace[(numLocalVertex+1)%numberOfVertexPerFace]);
                int indiceLocalOppVertex = (numLocalVertex+2)%numberOfVertexPerFace;

                if(existingEdge(edge, map)) {
                    pairIndiceFaceSommet otherIndexFaceVertex = getExistingEdge(edge, map);

                    faces[currentFace].indexesOfFacePerVertex[indiceLocalOppVertex] = otherIndexFaceVertex.first;
                    faces[otherIndexFaceVertex.first].indexesOfFacePerVertex[otherIndexFaceVertex.second] = currentFace;

                    sommets[edge.first].setIndexeFace(currentFace);
                    sommets[edge.second].setIndexeFace(currentFace);

                } else {
                    map[edge] = pairIndiceFaceSommet(currentFace, indiceLocalOppVertex);
                }

            }
        }
        numberOfEdge = map.size();
        std::cout << "temps d'importation du mesh en : "
        << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin)
        << std::endl;
        myfile.close();
    } else {
        std::cerr << "erreur lors de l'ouverture du fichier.";
        return;
    }
}

void MeshStruct::save_mesh_off(const std::string& file_name) {
    std::ofstream file(file_name + ".off");

    if(file.is_open()) {
        file << "OFF\n";
        // Si le mesh est ouvert, Nous devons modifier nos structures de données pour qu'elles gardent leur cohérence malgrè la suppréssion d'éléments
        if (!isClosed) {
            std::vector<bool> elementsToRemove(colorFace.size(), 0);
            size_t indice = 0;
            // on enlève le sommet infini
            std::vector<Sommet>::iterator it = sommets.begin();
            sommets.erase(it);
            // on cherche enlève toutes les faces infinies
            faces.erase(std::remove_if(faces.begin(), faces.end(), [&hint = indice, &etr = elementsToRemove](Face& f) {
                bool remove = false;
                for(indiceGlobalSommet& i: f.sommets) {
                    if(i == 0) {
                        etr[hint] = true;
                        remove = true;
                        break;
                    }
                    i--; // on décrémente l'indexe de la vertex puisque nous avons supprimer le sommet infini
                }
                hint++;
                return remove;
            }), faces.end());
            std::vector<Color>tmp;
            // On conserve la couleur des faces qui ne sont pas infinies
            for (size_t i = 0; i < colorFace.size(); i++) {
                if(elementsToRemove[i]) continue;
                tmp.push_back(colorFace[i]);
            }
            colorFace = tmp;
        }
        file << sommets.size()  << ' ' << faces.size()  << ' ' << numberOfEdge << '\n';
        for(const Sommet& s : sommets) {
            file << s << '\n';
        }
        // On regarde écrit dans notre fichier uniquement les faces qui ne sont pas infinie pour ne pas qu'elle soit visible dans notre visionneur.
        for(size_t indiceFace = 0; indiceFace < faces.size(); indiceFace++) {
            file << faces[indiceFace].sommets.size() << ' ';
            for (const indiceGlobalSommet& i : faces[indiceFace].sommets) {
                file << i << ' ';
            }  
            file << colorFace[indiceFace].getRed() << ' ' 
                << colorFace[indiceFace].getGreen() << ' '
                << colorFace[indiceFace].getBlue();

            file << '\n';
        }
    } else {
        std::cerr << "le fichier d'exportation n'est pas ouvert";
    }
    file.close();
}

const std::vector<Sommet>& MeshStruct::getSommets() const {
    return sommets;
} 

const std::vector<Vecteur>& MeshStruct::getNormals() const {
    return normals;
} 

void MeshStruct::save_mesh_obj(const std::string& file_name) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::ofstream file(file_name + ".obj");
    if(file.is_open()) {
        for(const Sommet& s : sommets) {
            file << "v " << s.getPoint() << " \n";
        }
        for (const float& c: courbures) {
            float normalizeCoor = c < 0 ? (c/courbure_min)/2:(0.5 + (c/courbure_max)/2);
            file << "vt " << normalizeCoor << ' ' << 1 <<'\n';
        }
        for (const Vecteur& n : normals) {
            file << "vn " << n << '\n'; 
        }
        for (const Face& f : faces) {
            file << 'f';
            for (size_t i = 0; i < f.sommets.size(); i++) {
                indiceGlobalSommet indice = f.sommets[i];
                if(normals.empty())
                    file << ' ' << indice + 1;
                else
                   file << ' ' << indice + 1 << "//" << indice + 1;
            }
            file << '\n';
        }
        std::cout << "temps du sauvegarde du mesh en .obj : "
        << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin)
        << std::endl;
    }
}


std::ostream& operator<<(std::ostream& os, const MeshStruct& mesh) {
    for(size_t numFace = 0; numFace < mesh.faces.size(); numFace++) {
        for(size_t numLocalSommet = 0; numLocalSommet < mesh.faces[numFace].sommets.size(); numLocalSommet++) {
            os  << "face " << numFace 
                << "/ sommet local " << numLocalSommet 
                << " : " << mesh.faces[numFace].indexesOfFacePerVertex[numLocalSommet] << '\n';
        }
    }
    return os;
}


void MeshStruct::turn_around_point(const size_t indexVertex, const Sens rotate) {
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


size_t MeshStruct::nextFaceG(const indiceFace currentFaceId, const size_t aroundPoint) {
    int id = 0;
    const Face& currentFace = faces[currentFaceId];
    while (currentFace.sommets[(id + 1) % 3] != aroundPoint) {
        id++;
        if(id > 3)
            throw std::runtime_error(
                "Le sommet autour du quel je tourne n'est pas trouvé"
            );
    }
    return currentFace.indexesOfFacePerVertex[id];
}

size_t MeshStruct::nextFaceD(const indiceFace currentFaceId, const size_t aroundPoint) {
    int id = 0;
    const Face& currentFace = faces[currentFaceId];
    while (currentFace.sommets[(id + 2) % 3] != aroundPoint) {
        id++;
        if(id > 3)
            throw std::runtime_error(
                "Le sommet autour du quel je tourne n'est pas trouvé"
            );
    }
    return currentFace.indexesOfFacePerVertex[id];
}

Vecteur MeshStruct::calculNormalTriangle(const Face& face) const {
    const std::vector<indiceGlobalSommet>& indexesPoint = face.sommets;
    Vecteur a = Vecteur(sommets[indexesPoint[0]].getPoint(), sommets[indexesPoint[1]].getPoint());
    Vecteur b = Vecteur(sommets[indexesPoint[0]].getPoint(), sommets[indexesPoint[2]].getPoint());
    return cross(a, b);     
}

std::tuple<float, float, const Point&> MeshStruct::calculCotanFaceV(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace) {
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


std::tuple<float, float, const Point&> MeshStruct::calculCotanFaceF(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace) {
    std::vector<indiceFace> adjFace = faces[currentFace].indexesOfFacePerVertex;
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

void MeshStruct::calculeLaplacian() {
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
            auto [cotan2, area2, _] = calculCotanFaceF(faces[indiceNextFace].indexesOfFacePerVertex, currentFace, indiceNextFace);
            
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

void MeshStruct::lawson(const indiceFace currentFaceId1, const indiceFace currentFaceId2, const indiceFace currentFaceId3) {
    std::queue<indiceFace> queueFaces;
    queueFaces.push(currentFaceId1);
    queueFaces.push(currentFaceId2);
    queueFaces.push(currentFaceId3);
    while (!queueFaces.empty()) {
        indiceFace currentFaceId = queueFaces.front();
        queueFaces.pop();
        if(faces[currentFaceId].isInfinite()) 
            continue;
        for (size_t i = 0; i < 3; i++) {
            if(faces[faces[currentFaceId].indexesOfFacePerVertex[i]].isInfinite())
                continue;

            const arete edge = {faces[currentFaceId].sommets[(i+1) % 3], faces[currentFaceId].sommets[(i+2) % 3]};
            auto [oppositeFace, oppositeVertexLocalId] = specificIncidentFace(edge, currentFaceId);
            auto oppositeFaceOppositeVertexLocalId = specificIncidentFace(edge, oppositeFace).second;
            const Point& pa = sommets[faces[currentFaceId].sommets[i]].getPoint();
            const Point& pb = sommets[edge.first].getPoint();
            const Point& pc = sommets[edge.second].getPoint();
            const Point& pd = sommets[faces[oppositeFace].sommets[/*oppositeVertexLocalId*/oppositeFaceOppositeVertexLocalId]].getPoint();

            Point sureleveA(pa.x, pa.x*pa.x + pa.z*pa.z, pa.z);
            Point sureleveB(pb.x, pb.x*pb.x + pb.z*pb.z, pb.z);
            Point sureleveC(pc.x, pc.x*pc.x + pc.z*pc.z, pc.z);
            Point sureleveD(pd.x, pd.x*pd.x + pd.z*pd.z, pd.z);

            Vecteur normale = cross(sureleveB - sureleveA, sureleveC - sureleveA);
            float a = dot(normale, sureleveD - sureleveA);


            if(a > 0) {
                queueFaces.push(currentFaceId);
                queueFaces.push(oppositeFace);
                edgeFlip(edge);
                break;
            }

        }
    }
}

void MeshStruct::addPoint(const Point& newVec) {
    sommets.push_back(newVec);
    std::pair res = inConvexHull(newVec);
    if(res.first) {
        split3(res.second, sommets.size() - 1);
        lawson(res.second, faces.size() - 2, faces.size() - 1);

    } else {
        // recherche d'une face visible
        size_t currentFaceId = nextFaceD(sommets[0].getFaceIndex(), 0);
        arete edge = faces[currentFaceId].getOppositeEdge();
        while(orientationTest(newVec, sommets[edge.first], sommets[edge.second]) < 0) {
            currentFaceId = nextFaceD(currentFaceId, 0);
            edge = faces[currentFaceId].getOppositeEdge();
        }
        while (orientationTest(newVec, sommets[edge.first], sommets[edge.second]) >= 0) {
            currentFaceId = nextFaceD(currentFaceId, 0);
            edge = faces[currentFaceId].getOppositeEdge();
        }
        currentFaceId = nextFaceG(currentFaceId, 0);
        indiceFace nextFace = nextFaceG(currentFaceId, 0);
        split3(currentFaceId, sommets.size() - 1);
        lawson(currentFaceId, faces.size() - 2, faces.size() - 1);
        currentFaceId = nextFace;
        edge = faces[nextFace].getOppositeEdge();

        // flip des arêtes visibles par le nouveau sommet insérée
        while(orientationTest(newVec, sommets[edge.first], sommets[edge.second]) >= 0) {
            currentFaceId = nextFaceG(currentFaceId, 0);
            edgeFlip({edge.second, 0}); // FIXME : CE N'EST PAS LA BONNE ARETE QUI EST FLIP ICI!!!
            edge = faces[currentFaceId].getOppositeEdge();

        }
    }
}

void MeshStruct::split3(const indiceFace indiceFace, const indiceGlobalSommet vertexId) {
    Face& currentFace = faces[indiceFace];
    const std::vector<indexesFaceOppSommet> neighbourFaces = currentFace.indexesOfFacePerVertex;
    const std::vector<indiceGlobalSommet> indexeVertexInFace = currentFace.sommets;

    // rattache le sommet qui sera avant déplacement à sa nouvelle face
    sommets[currentFace.sommets[0]].setIndexeFace(faces.size());
    currentFace.sommets[0] = vertexId;
    sommets[vertexId].setIndexeFace(indiceFace);
    
    // recoud notre triangle de base
    currentFace.indexesOfFacePerVertex[1] = faces.size();
    faces[neighbourFaces[1]].setNewOppositeVertexPoint(indiceFace, faces.size());
    currentFace.indexesOfFacePerVertex[2] = faces.size() + 1;
    faces[neighbourFaces[2]].setNewOppositeVertexPoint(indiceFace, faces.size()+1);

    Face newFace = Face(vertexId, indexeVertexInFace[2], indexeVertexInFace[0]);
    newFace.indexesOfFacePerVertex = {neighbourFaces[1], faces.size() + 1, indiceFace};
    colorFace.push_back(Color());
    faces.push_back(newFace);

    newFace = Face(vertexId, indexeVertexInFace[0], indexeVertexInFace[1]);
    newFace.indexesOfFacePerVertex = {neighbourFaces[2], indiceFace, faces.size()-1};
    colorFace.push_back(Color());
    faces.push_back(newFace);

}

indiceFace MeshStruct::searchIncidentFace(const arete& edge) {
    unsigned int localId = 0;
    indiceFace currentFaceId = sommets[edge.first].getFaceIndex();
    const indiceFace initFaceID = currentFaceId;
    indiceGlobalSommet vertexOfCurrentFace = faces[currentFaceId].sommets[localId];
    while(vertexOfCurrentFace != edge.second) {
        localId++;
        if(localId % 3 == 0) {
            currentFaceId = nextFaceD(currentFaceId, edge.first);

            if(currentFaceId == initFaceID) {
                std::runtime_error error(
                    "ce segment n'existe pas!!!\n"
                );
                throw error;
            }

            Face tmp = faces[currentFaceId];
        }
        vertexOfCurrentFace = faces[currentFaceId].sommets[localId % 3];
    }
    return currentFaceId;
}

std::pair<indiceFace, size_t> MeshStruct::specificIncidentFace(const arete& edge, const indiceFace faceId) {
    const Face& currentFace = faces[faceId];
    for(int i = 0; i < 3; i++) {
        if(currentFace.sommets[i] != edge.first && currentFace.sommets[i] != edge.second)
            return {currentFace.indexesOfFacePerVertex[i], i};
    }
    throw std::runtime_error(
        "La face opposé n'a pas été trouvée"
    );
}

void MeshStruct::edgeSplit(const arete& edge, const float interpolateValue) {
    Point newVertexCoor = sommets[edge.first].getPoint() + (sommets[edge.second].getPoint() - sommets[edge.first].getPoint()) * interpolateValue;
    sommets.push_back(newVertexCoor);
    indiceFace face1 = searchIncidentFace(edge);
    sommets[sommets.size() - 1].setIndexeFace(face1);
    auto [face2, oppositeOfFace2] = specificIncidentFace(edge, face1);
    auto [trash, oppositeOfFace1] = specificIncidentFace(edge, face2);

    //réorganisation des deux faces initiales
    const std::vector<indiceFace> neighbourFaces1 = faces[face1].indexesOfFacePerVertex;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace1 = faces[face1].sommets;
    faces[face1].sommets[(oppositeOfFace2 + 2) % 3] = sommets.size() - 1;
    sommets[vertexIdIncidentFace1[(oppositeOfFace2 + 2) % 3]].setIndexeFace(faces.size());
    const std::vector<indiceFace> neighbourFaces2 = faces[face2].indexesOfFacePerVertex;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace2 = faces[face2].sommets;
    faces[face2].sommets[(oppositeOfFace1 + 1) % 3] = sommets.size() - 1;
    sommets[vertexIdIncidentFace2[(oppositeOfFace1 + 1) % 3]].setIndexeFace(faces.size() + 1);


    //Couture de la première nouvelle face
    Face newFace(vertexIdIncidentFace1[oppositeOfFace2], sommets.size() - 1, vertexIdIncidentFace1[(oppositeOfFace2 + 2) % 3]);
    newFace.indexesOfFacePerVertex = {faces.size() + 1, neighbourFaces1[(oppositeOfFace2+1) % 3], face1};
    faces[neighbourFaces1[(oppositeOfFace2 + 1) % 3]].setNewOppositeVertexPoint(face1, faces.size());
    faces[face1].indexesOfFacePerVertex[(oppositeOfFace2+1) % 3] = faces.size();
    colorFace.push_back(Color());
    faces.push_back(newFace);


    //Couture de la seconde nouvelle face
    newFace = Face(vertexIdIncidentFace2[oppositeOfFace1], vertexIdIncidentFace2[(oppositeOfFace1 + 1) % 3], sommets.size() - 1);
    newFace.indexesOfFacePerVertex = {faces.size() - 1, face2, neighbourFaces2[(oppositeOfFace1 + 2) % 3]};
    faces[neighbourFaces2[(oppositeOfFace1 + 2) % 3]].setNewOppositeVertexPoint(face2, faces.size());
    faces[face2].indexesOfFacePerVertex[(oppositeOfFace1 + 2) % 3] = faces.size();
    colorFace.push_back(Color());
    faces.push_back(newFace);

    numberOfEdge += 2;
}

void MeshStruct::edgeFlip(const arete& edge) {
    assert(edge.first != edge.second);
    indiceFace faceId1 = searchIncidentFace(edge);
    auto [faceId2, oppositeOfFace2] = specificIncidentFace(edge, faceId1);
    auto [trash, oppositeOfFace1] = specificIncidentFace(edge, faceId2);
    assert(faceId1==trash);

    const std::vector<indiceFace> neighbourFaces1 = faces[faceId1].indexesOfFacePerVertex;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace1 = faces[faceId1].sommets;
    const std::vector<indiceFace> neighbourFaces2 = faces[faceId2].indexesOfFacePerVertex;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace2 = faces[faceId2].sommets;

    // couture du premier triangle
    faces[faceId1].sommets[(oppositeOfFace2 + 2) % 3] = vertexIdIncidentFace2[oppositeOfFace1];
    sommets[vertexIdIncidentFace1[(oppositeOfFace2 + 1)%3]].setIndexeFace(faceId1);
    faces[faceId1].indexesOfFacePerVertex[oppositeOfFace2] = neighbourFaces2[(oppositeOfFace1 + 1) % 3];
    faces[faceId1].indexesOfFacePerVertex[(oppositeOfFace2 + 1) % 3] = faceId2;
    faces[neighbourFaces2[(oppositeOfFace1 + 1) % 3]].setNewOppositeVertexPoint(faceId2, faceId1);

    // couture du second triangle
    faces[faceId2].sommets[(oppositeOfFace1 + 2) % 3] = vertexIdIncidentFace1[oppositeOfFace2];
    sommets[vertexIdIncidentFace2[(oppositeOfFace1 + 1)%3]].setIndexeFace(faceId2);
    faces[faceId2].indexesOfFacePerVertex[oppositeOfFace1] = neighbourFaces1[(oppositeOfFace2 + 1) % 3];
    faces[faceId2].indexesOfFacePerVertex[(oppositeOfFace1 + 1) % 3] = faceId1;
    faces[neighbourFaces1[(oppositeOfFace2 + 1) % 3]].setNewOppositeVertexPoint(faceId1, faceId2);
}
