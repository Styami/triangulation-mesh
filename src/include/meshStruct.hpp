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
		enum class Sens{GAUCHE, DROITE};
		/**
			Constructeur par defaut de MeshStruct
			@param bool isClosed permet de savoir si le mesh est fermé ou non
		*/
		MeshStruct(const bool isClosed);

		/**
			Constructeur par défault, maillage basique avec 6 triangles + sommet et faces infinis
		*/
		MeshStruct();

		/**
			Constructeur de MeshStruct
			@param Sommets a du triangle
			@param Sommets b du triangle
			@param Sommets c du triangle
			@param bool isClosed permet de savoir si le mesh est fermé ou non
		*/
		MeshStruct(const Sommet& a, const Sommet& b, const Sommet& c, const bool isClosed);
		/**
			Charge un mesh ouvert
			@param string file_name fichier à ouvrir
			@return void
		*/
		void loadOpenMesh(const std::string& file_name);
		/**
			Charge un mesh fermé
			@param string file_name fichier à ouvrir
			@return void
		*/
		void loadCloseMesh(const std::string& file_name);
		/**
			Ajout d'un point au mesh
			@param Point coordonnées du point à insérer
			@return void
		*/
		void addPoint(const Point& newVec);
		/**
			Sauvegarde un mesh sous le format OFF
			@param string nom du fichier (sans l'extension)
			@return void
		*/
		void save_mesh_off(const std::string& file_name);
		/**
			Sauvegarde un mesh sous le format OBJ
			@param string nom du fichier (sans l'extension)
			@return void
		*/
		void save_mesh_obj(const std::string& file_name);
		/**
			Permet de simplement tourner autour d'une vertex tout mettant de la couleur aux faces traversées
			@param size_t indexVertex
			@param Sens sens de rotation autour de la vertex
			@return void
		*/
		void turn_around_point(const size_t indexVertex, const Sens rotate);
		/**
			Permet de calculer les normales aux sommets de mon mesh en s'appuyant sur les calculs du laplacian
			@return void
		*/
		void calculeLaplacian();
		void lawson(const indiceFace currentFaceId1,  const indiceFace currentFaceId2, const indiceFace currentFaceId3);
		friend std::ostream& operator<<(std::ostream& os, const MeshStruct& mesh);

	private:public:

		/**
			Calcul la normal d'une face à partir de son indice de face
			@param Face
			@return Vecteur
		*/
		Vecteur calculNormalTriangle(const Face& face) const;
		/**
			permet d'obtenir la face adjacente à une face incident au Sommet passé en paramètre
			version gauche
			@param indiceFace, size_t
			@return size_t
		*/
		size_t nextFaceG(const indiceFace currentFace, const size_t aroundPoint);
		/**
			permet d'obtenir la face adjacente à une face incident au Sommet passé en paramètre
			version droite
			@param indiceFace, size_t
			@return size_t
		*/
		size_t nextFaceD(const indiceFace currentFace, const size_t aroundPoint);
		/**
			La fonction retourne l'indice d'une des deux faces (ou de la face) adjacente
			@param arete
			@return indiceFace
		*/
		indiceFace searchIncidentFace(const arete& edge);
		/**
			La fonction retourne l'indice de la face partageant la même arete avec la face faceId
			ainsi que l'indice local du sommet
			@param arete, indiceFace
			@return tuple<indiceFace, size_t>
		*/
		std::pair<indiceFace, size_t> specificIncidentFace(const arete& edge, const indiceFace faceId);
		/** 
			La fonction retourne si le point passé en paramètre est dans l'enveloppe convexe ou non ainsi que la face 
			dans laquelle le point appartient s'il se trouve dans l'enveloppe convexe 
			@param Point point verifié
			@return pair<bool, indiceFace>
		*/
		std::pair<bool, indiceFace> inConvexHull(const Point& p) const;
		float orientationTest(const Sommet& p1, const Sommet& p2, const Sommet& p3) const;
		std::tuple<float, float, const Point&> calculCotanFaceV(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace);
		std::tuple<float, float, const Point&> calculCotanFaceF(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace);
		void split3(const indiceFace indiceFace, const indiceGlobalSommet indiceVertex);
		void edgeSplit(const arete& edge, const float interpolateValue);
		void edgeFlip(const arete& edge);
		/**
			Permet de savoir si les trois points passé en paramètre sont placé dans le sens trigonométrique
			@param p1, p2, p3 Les points formant le triangle
			@return <0 si c'est dans le sens horaire / 0 si le triangle est plat / >0 si c'est dans le sens horaire
		*/
		int inTriangle(const Face& face, const Point& p) const;
		const std::vector<Sommet>& getSommets() const;
		const std::vector<Vecteur>& getNormals() const;
		std::vector<Face> faces;
		std::vector<Color> colorFace;
		std::vector<Vecteur> normals;
		std::vector<Sommet> sommets;
		std::vector<float> courbures;
		float courbure_min = std::numeric_limits<float>::max();
		float courbure_max = std::numeric_limits<float>::lowest();
		int numberOfEdge;
		const bool isClosed;
	
};

#endif