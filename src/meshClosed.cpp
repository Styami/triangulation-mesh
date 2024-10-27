#include "include/meshClosed.hpp"
#include <chrono>
#include <fstream>
#include <iostream>

void MeshClosed::loadMesh(const std::string& file_name) {
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

            faces[currentFace].neighboursFaces = std::vector<indexesFaceOppSommet>(numberOfVertexPerFace, -1);

            for(int numLocalVertex = 0; numLocalVertex < numberOfVertexPerFace; numLocalVertex++) {
                arete edge = std::pair(vertexFace[numLocalVertex], vertexFace[(numLocalVertex+1)%numberOfVertexPerFace]);
                int indiceLocalOppVertex = (numLocalVertex+2)%numberOfVertexPerFace;

                if(existingEdge(edge, map)) {
                    pairIndiceFaceSommet otherIndexFaceVertex = getExistingEdge(edge, map);

                    faces[currentFace].neighboursFaces[indiceLocalOppVertex] = otherIndexFaceVertex.first;
                    faces[otherIndexFaceVertex.first].neighboursFaces[otherIndexFaceVertex.second] = currentFace;

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

void MeshClosed::save_mesh_off(const std::string& file_name) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::ofstream file(file_name + ".off");

    if(file.is_open()) {
        file << "OFF\n";
        file << sommets.size()  << ' ' << faces.size()  << ' ' << numberOfEdge << '\n';
        for(const Sommet& s : sommets) {
            file << s << '\n';
        }
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
        std::cout << "temps du sauvegarde du mesh en .off: "
        << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - begin)
        << std::endl;
    } else {
        std::cerr << "le fichier d'exportation n'est pas ouvert";
    }
    file.close();
}

void MeshClosed::save_mesh_obj(const std::string& file_name) {
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
    } else {
        std::cerr << "le fichier d'exportation n'est pas ouvert";
    }
}