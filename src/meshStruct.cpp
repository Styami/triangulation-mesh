#include "include/meshStruct.hpp"
#include <cstddef>
#include <iostream>
#include <chrono>
#include <map>
#include <fstream>
#include <vector>



typedef std::pair<indiceGlobalSommet, indiceGlobalSommet> arete;
typedef std::pair<indiceFace, indiceGlobalSommet> pairIndiceFaceSommet;


inline bool existingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces) {
    return (edgesFaces.find(std::pair(edge.second, edge.first)) != edgesFaces.end())
            || edgesFaces.find(edge) != edgesFaces.end();
}

inline pairIndiceFaceSommet getExistingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces) {
    return edgesFaces.find(edge) != edgesFaces.end() ? 
    edgesFaces.at(edge):
    edgesFaces.at(std::pair(edge.second, edge.first));
}



// bool MeshStruct::inConvexHull(const Point& p) const {
//     for (size_t numFace = 0; numFace < faces.size(); numFace++) {
//         Vecteur v = calculNormalTriangle(numFace);
//         if(dot())
//     }
// } FIXME

MeshStruct::MeshStruct(const std::string& file_name) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    Point p;
    std::string line;
    std::ifstream myfile (file_name);
    int nbSommets;
    int nbFaces;
    std::map<arete, pairIndiceFaceSommet> map;

    if(myfile.is_open()) {
        myfile.ignore(4);
        myfile >> nbSommets >> nbFaces;
        myfile.ignore(1000, '\n');
        for(int i = 0; i < nbSommets; i++) {
            myfile >> p.x >> p.y >> p.z;
            sommets.push_back(p);
            //poidsVertex.push_back(rand() % 100);
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
        file << sommets.size() << ' ' << faces.size() << ' ' << numberOfEdge << '\n';
        for(const Sommet& s : sommets) {
            file << s << '\n';
        }
        for(unsigned int indiceFace = 0; indiceFace < faces.size(); indiceFace++) {
            file << faces[indiceFace].sommets.size() << ' ';
            for (const indiceGlobalSommet& i : faces[indiceFace].sommets) {
                file << i << ' ';
            }
            file << colorFace[indiceFace].getRed() << ' ' 
                << colorFace[indiceFace].getGreen() << ' '
                << colorFace[indiceFace].getBlue() << '\n';
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


void MeshStruct::turn_around_point(const size_t indexVertex) {
    const Sommet& s = sommets[indexVertex];
    size_t firstFace = s.getFaceIndex();
    size_t currentFace = firstFace;

    do {
        colorFace[currentFace] = Color(255, 0, 0);
        std::vector<indiceGlobalSommet> LocalVertex = faces[currentFace].sommets;
        currentFace = nextFace(currentFace, indexVertex);
    }while (currentFace != firstFace);
}


int MeshStruct::nextFace(int currentFace, size_t aroundPoint) {
    int id = 0;
    while (faces[currentFace].sommets[(id + 1) % 3] != aroundPoint) {
        id++;
    }
    return faces[currentFace].indexesOfFacePerVertex[id];
}


Vecteur MeshStruct::calculNormalTriangle(const Face& face) const {
    const std::vector<indiceGlobalSommet>& indexesPoint = face.sommets;
    Vecteur a = Vecteur(sommets[indexesPoint[0]].getPoint(), sommets[indexesPoint[1]].getPoint());
    Vecteur b = Vecteur(sommets[indexesPoint[0]].getPoint(), sommets[indexesPoint[2]].getPoint());
    return cross(a, b);     
}

std::tuple<float, float, const Point&> MeshStruct::calculCotanFaceV(const std::vector<size_t>& indexes, int indexeTerminal, int currentFace) {
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


std::tuple<float, float, const Point&> MeshStruct::calculCotanFaceF(const std::vector<size_t>& indexes, int indexeTerminal, int currentFace) {
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
            int indiceNextFace = nextFace(currentFace, indexVertex);
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

void MeshStruct::addPoint(const indiceFace faceId, const Point& newVec) {
    sommets.push_back(Sommet(newVec));
    split3(faceId, sommets.size() - 1);
    numberOfEdge += 3;
    turn_around_point(0);
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
    faces[neighbourFaces[1]].setNewOppositeVertexPoint(indiceFace, faces.size()+1);

    

    Face newFace(vertexId, indexeVertexInFace[0], indexeVertexInFace[1]);
    newFace.indexesOfFacePerVertex = {neighbourFaces[2], indiceFace, faces.size()+1};
    colorFace.push_back(Color());
    faces.push_back(newFace);

    newFace = Face(vertexId, indexeVertexInFace[2], indexeVertexInFace[0]);
    newFace.indexesOfFacePerVertex = {neighbourFaces[1], faces.size() - 1, indiceFace};
    colorFace.push_back(Color());
    faces.push_back(newFace);
}