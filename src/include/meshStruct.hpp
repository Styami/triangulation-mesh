#ifndef __MESH__
#define __MESH__

#include "sommet.hpp"
#include "face.hpp"
#include "color.hpp"
#include "vecteur.hpp"
#include <cstddef>
#include <limits>
#include <vector>

class MeshStruct {
	public:
		void addPoint(const indiceFace faceId, const Point& newVec);
		MeshStruct(const std::string& file_name);
		void save_mesh_off(const std::string& file_name);
		void save_mesh_obj(const std::string& file_name);
		void turn_around_point(const size_t indexVertex);
		void calculeLaplacian();
		const std::vector<Sommet>& getSommets() const;
		const std::vector<Vecteur>& getNormals() const;
		bool inConvexHull(const Point& p) const;
		friend std::ostream& operator<<(std::ostream& os, const MeshStruct& mesh);
		void split3(const indiceFace indiceFace, const indiceGlobalSommet indiceVertex);
		void edgeSplit(const indiceGlobalSommet& vertex1, const indiceGlobalSommet& vertex2, const float interpolateValue);
		

	private:
		Vecteur calculNormalTriangle(const Face& numFace) const;
		int nextFace(const int currentFace, const size_t aroundPoint);
		indiceFace searchIncidentFace(const indiceGlobalSommet vertexId1, const indiceGlobalSommet vertexId2);
		std::tuple<indiceFace, size_t> specificIncidentFace(const indiceGlobalSommet vertexId1, const indiceGlobalSommet vertexId2, const indiceFace faceId);
		std::tuple<float, float, const Point&> calculCotanFaceV(const std::vector<size_t>& indexes, int indexeTerminal, int currentFace);
		std::tuple<float, float, const Point&> calculCotanFaceF(const std::vector<size_t>& indexes, int indexeTerminal, int currentFace);
		std::vector<Face> faces;
		std::vector<Color> colorFace;
		std::vector<Vecteur> normals;
		std::vector<Sommet> sommets;
		std::vector<float> courbures;
		float courbure_min = std::numeric_limits<float>::max();
		float courbure_max = std::numeric_limits<float>::lowest();
		int numberOfEdge;
	
};

#endif