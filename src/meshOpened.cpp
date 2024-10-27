#include "include/meshOpened.hpp"
#include <cassert>
#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>

void MeshOpened::loadMesh(const std::string& file_name) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::ifstream myfile (file_name);
    Point p;
    int nbSommets;
    Sommet newVertex;
    sommets.push_back(Sommet({0, 0, 0}));
    sommets[0].setIndexeFace(1);

    if(myfile.is_open()) {
        myfile >> nbSommets;
        for (int i = 1; i < 4; i ++) {
            myfile >> p.x >> p.y >> p.z;
            sommets.push_back(p);
            sommets[i].setIndexeFace(0);
        }
        if(orientationTest(sommets[1], sommets[2], sommets[3])) {
            faces.push_back({1, 2, 3});
            faces[0].neighboursFaces = {2, 1, 3};
            faces.push_back({0, 1, 3});
            faces[1].neighboursFaces = {0, 2, 3};
            faces.push_back({0, 3, 2});
            faces[2].neighboursFaces = {0, 3, 1};
            faces.push_back({0, 2, 1});
            faces[3].neighboursFaces = {0, 1, 2};
        } else {
            faces.push_back({1, 3, 2});
            faces[0].neighboursFaces = {2, 1, 3};
            faces.push_back({0, 1, 2});
            faces[1].neighboursFaces = {0, 2, 3};
            faces.push_back({0, 2, 3});
            faces[2].neighboursFaces = {0, 3, 1};
            faces.push_back({0, 3, 1});
            faces[3].neighboursFaces = {0, 1, 2};
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

void MeshOpened::cleanDataIfOpen() {
    if(has_been_cleaned) 
        return;
    else
        has_been_cleaned = true;
    std::vector<bool> elementsToRemove(colorFace.size(), false);
    size_t indice = 0;
    // on enlève le sommet infini
    std::vector<Sommet>::iterator its = sommets.begin();
    sommets.erase(its);
    std::vector<Vecteur>::iterator itn = normals.begin();
    normals.erase(itn);
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

void MeshOpened::save_mesh_off(const std::string& file_name) {
    std::ofstream file(file_name + ".off");
    if(file.is_open()) {
        std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
        file << "OFF\n";
        cleanDataIfOpen();
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

void MeshOpened::save_mesh_obj(const std::string& file_name) {
    std::chrono::high_resolution_clock::time_point begin = std::chrono::high_resolution_clock::now();
    std::ofstream file(file_name + ".obj");
    if(file.is_open()) {
        cleanDataIfOpen();
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

float MeshOpened::orientationTest(const Sommet& a, const Sommet& b, const Sommet& c) const {
    Vecteur ab = b.getPoint() - a.getPoint();
    Vecteur ac = c.getPoint() - a.getPoint();
    float det = ab.x * ac.y - ac.x * ab.y;
    return det;
}

int MeshOpened::inTriangle(const Face& face, const Point& p) const {
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

std::pair<bool, indiceFace> MeshOpened::inConvexHull(const Point& p) const {
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

void MeshOpened::lawson(const indiceFace currentFaceId1, const indiceFace currentFaceId2, const indiceFace currentFaceId3) {
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
            if(faces[faces[currentFaceId].neighboursFaces[i]].isInfinite())
                continue;

            const arete edge = {faces[currentFaceId].sommets[(i+1) % 3], faces[currentFaceId].sommets[(i+2) % 3]};
            auto [oppositeFace, oppositeVertexLocalId] = specificIncidentFace(edge, currentFaceId);
            auto oppositeFaceOppositeVertexLocalId = specificIncidentFace(edge, oppositeFace).second;
            const Point& pa = sommets[faces[currentFaceId].sommets[i]].getPoint();
            const Point& pb = sommets[edge.first].getPoint();
            const Point& pc = sommets[edge.second].getPoint();
            const Point& pd = sommets[faces[oppositeFace].sommets[/*oppositeVertexLocalId*/oppositeFaceOppositeVertexLocalId]].getPoint();

            Point sureleveA(pa.x, pa.x*pa.x + pa.y*pa.y, pa.y);
            Point sureleveB(pb.x, pb.x*pb.x + pb.y*pb.y, pb.y);
            Point sureleveC(pc.x, pc.x*pc.x + pc.y*pc.y, pc.y);
            Point sureleveD(pd.x, pd.x*pd.x + pd.y*pd.y, pd.y);

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

indiceFace MeshOpened::searchIncidentFace(const arete& edge) {
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

std::pair<indiceFace, size_t> MeshOpened::specificIncidentFace(const arete& edge, const indiceFace faceId) {
    const Face& currentFace = faces[faceId];
    for(int i = 0; i < 3; i++) {
        if(currentFace.sommets[i] != edge.first && currentFace.sommets[i] != edge.second)
            return {currentFace.neighboursFaces[i], i};
    }
    throw std::runtime_error(
        "La face opposé n'a pas été trouvée"
    );
}

void MeshOpened::addPoint(const Point& newVec) {
    sommets.push_back(newVec);
    std::pair res = inConvexHull(newVec);
    if(res.first) {
        split3(res.second, sommets.size() - 1);
        lawson(res.second, faces.size() - 2, faces.size() - 1);

    } else {
        // recherche d'une face visible
        size_t currentFaceId = nextFaceD(sommets[0].getFaceIndex(), 0);
        arete edge = faces[currentFaceId].OppositeEdgeFromInfiniteFace();
        while(orientationTest(newVec, sommets[edge.first], sommets[edge.second]) < 0) {
            currentFaceId = nextFaceD(currentFaceId, 0);
            edge = faces[currentFaceId].OppositeEdgeFromInfiniteFace();
        }
        while (orientationTest(newVec, sommets[edge.first], sommets[edge.second]) >= 0) {
            currentFaceId = nextFaceD(currentFaceId, 0);
            edge = faces[currentFaceId].OppositeEdgeFromInfiniteFace();
        }
        currentFaceId = nextFaceG(currentFaceId, 0);
        indiceFace nextFace = nextFaceG(currentFaceId, 0);
        split3(currentFaceId, sommets.size() - 1);
        lawson(currentFaceId, faces.size() - 2, faces.size() - 1);
        currentFaceId = nextFace;
        edge = faces[nextFace].OppositeEdgeFromInfiniteFace();

        // flip des arêtes visibles par le nouveau sommet insérée
        while(orientationTest(newVec, sommets[edge.first], sommets[edge.second]) >= 0) {
            currentFaceId = nextFaceG(currentFaceId, 0);
            edgeFlip({edge.second, 0}); // FIXME : CE N'EST PAS LA BONNE ARETE QUI EST FLIP ICI!!!
            edge = faces[currentFaceId].OppositeEdgeFromInfiniteFace();

        }
    }
}

void MeshOpened::split3(const indiceFace indiceFace, const indiceGlobalSommet vertexId) {
    Face& currentFace = faces[indiceFace];
    const std::vector<indexesFaceOppSommet> neighbourFaces = currentFace.neighboursFaces;
    const std::vector<indiceGlobalSommet> indexeVertexInFace = currentFace.sommets;

    // rattache le sommet qui sera avant déplacement à sa nouvelle face
    sommets[currentFace.sommets[0]].setIndexeFace(faces.size());
    currentFace.sommets[0] = vertexId;
    sommets[vertexId].setIndexeFace(indiceFace);
    
    // recoud notre triangle de base
    currentFace.neighboursFaces[1] = faces.size();
    faces[neighbourFaces[1]].setNewOppositeVertexPoint(indiceFace, faces.size());
    currentFace.neighboursFaces[2] = faces.size() + 1;
    faces[neighbourFaces[2]].setNewOppositeVertexPoint(indiceFace, faces.size()+1);

    Face newFace = Face(vertexId, indexeVertexInFace[2], indexeVertexInFace[0]);
    newFace.neighboursFaces = {neighbourFaces[1], faces.size() + 1, indiceFace};
    colorFace.push_back(Color());
    faces.push_back(newFace);

    newFace = Face(vertexId, indexeVertexInFace[0], indexeVertexInFace[1]);
    newFace.neighboursFaces = {neighbourFaces[2], indiceFace, faces.size()-1};
    colorFace.push_back(Color());
    faces.push_back(newFace);

}

void MeshOpened::edgeSplit(const arete& edge, const float interpolateValue) {
    Point newVertexCoor = sommets[edge.first].getPoint() + (sommets[edge.second].getPoint() - sommets[edge.first].getPoint()) * interpolateValue;
    sommets.push_back(newVertexCoor);
    indiceFace face1 = searchIncidentFace(edge);
    sommets[sommets.size() - 1].setIndexeFace(face1);
    auto [face2, oppositeOfFace2] = specificIncidentFace(edge, face1);
    auto [trash, oppositeOfFace1] = specificIncidentFace(edge, face2);

    //réorganisation des deux faces initiales
    const std::vector<indiceFace> neighbourFaces1 = faces[face1].neighboursFaces;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace1 = faces[face1].sommets;
    faces[face1].sommets[(oppositeOfFace2 + 2) % 3] = sommets.size() - 1;
    sommets[vertexIdIncidentFace1[(oppositeOfFace2 + 2) % 3]].setIndexeFace(faces.size());
    const std::vector<indiceFace> neighbourFaces2 = faces[face2].neighboursFaces;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace2 = faces[face2].sommets;
    faces[face2].sommets[(oppositeOfFace1 + 1) % 3] = sommets.size() - 1;
    sommets[vertexIdIncidentFace2[(oppositeOfFace1 + 1) % 3]].setIndexeFace(faces.size() + 1);


    //Couture de la première nouvelle face
    Face newFace(vertexIdIncidentFace1[oppositeOfFace2], sommets.size() - 1, vertexIdIncidentFace1[(oppositeOfFace2 + 2) % 3]);
    newFace.neighboursFaces = {faces.size() + 1, neighbourFaces1[(oppositeOfFace2+1) % 3], face1};
    faces[neighbourFaces1[(oppositeOfFace2 + 1) % 3]].setNewOppositeVertexPoint(face1, faces.size());
    faces[face1].neighboursFaces[(oppositeOfFace2+1) % 3] = faces.size();
    colorFace.push_back(Color());
    faces.push_back(newFace);


    //Couture de la seconde nouvelle face
    newFace = Face(vertexIdIncidentFace2[oppositeOfFace1], vertexIdIncidentFace2[(oppositeOfFace1 + 1) % 3], sommets.size() - 1);
    newFace.neighboursFaces = {faces.size() - 1, face2, neighbourFaces2[(oppositeOfFace1 + 2) % 3]};
    faces[neighbourFaces2[(oppositeOfFace1 + 2) % 3]].setNewOppositeVertexPoint(face2, faces.size());
    faces[face2].neighboursFaces[(oppositeOfFace1 + 2) % 3] = faces.size();
    colorFace.push_back(Color());
    faces.push_back(newFace);

    numberOfEdge += 2;
}

void MeshOpened::edgeFlip(const arete& edge) {
    assert(edge.first != edge.second);
    indiceFace faceId1 = searchIncidentFace(edge);
    auto [faceId2, oppositeOfFace2] = specificIncidentFace(edge, faceId1);
    auto [trash, oppositeOfFace1] = specificIncidentFace(edge, faceId2);
    assert(faceId1==trash);

    const std::vector<indiceFace> neighbourFaces1 = faces[faceId1].neighboursFaces;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace1 = faces[faceId1].sommets;
    const std::vector<indiceFace> neighbourFaces2 = faces[faceId2].neighboursFaces;
    const std::vector<indiceGlobalSommet> vertexIdIncidentFace2 = faces[faceId2].sommets;

    // couture du premier triangle
    faces[faceId1].sommets[(oppositeOfFace2 + 2) % 3] = vertexIdIncidentFace2[oppositeOfFace1];
    sommets[vertexIdIncidentFace1[(oppositeOfFace2 + 1)%3]].setIndexeFace(faceId1);
    faces[faceId1].neighboursFaces[oppositeOfFace2] = neighbourFaces2[(oppositeOfFace1 + 1) % 3];
    faces[faceId1].neighboursFaces[(oppositeOfFace2 + 1) % 3] = faceId2;
    faces[neighbourFaces2[(oppositeOfFace1 + 1) % 3]].setNewOppositeVertexPoint(faceId2, faceId1);

    // couture du second triangle
    faces[faceId2].sommets[(oppositeOfFace1 + 2) % 3] = vertexIdIncidentFace1[oppositeOfFace2];
    sommets[vertexIdIncidentFace2[(oppositeOfFace1 + 1)%3]].setIndexeFace(faceId2);
    faces[faceId2].neighboursFaces[oppositeOfFace1] = neighbourFaces1[(oppositeOfFace2 + 1) % 3];
    faces[faceId2].neighboursFaces[(oppositeOfFace1 + 1) % 3] = faceId1;
    faces[neighbourFaces1[(oppositeOfFace2 + 1) % 3]].setNewOppositeVertexPoint(faceId1, faceId2);
}