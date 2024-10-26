#ifndef __MESH__
#define __MESH__

#include "sommet.hpp"
#include "face.hpp"
#include "color.hpp"
#include "vecteur.hpp"
#include <cstddef>
#include <limits>
#include <vector>


class Mesh {
	public:
		enum class Sens{GAUCHE, DROITE};
		/**
			Constructeur par defaut de MeshStruct
		*/
		Mesh();

		/**
			Constructeur de MeshStruct
			@param Sommets a du triangle
			@param Sommets b du triangle
			@param Sommets c du triangle
		*/
		Mesh(const Sommet& a, const Sommet& b, const Sommet& c);
		/**
			Charge un mesh
			@param string file_name fichier à ouvrir
			@return void
		*/
		virtual void loadMesh(const std::string& file_name) = 0;
		/**
			Sauvegarde un mesh sous le format OFF
			@param string nom du fichier (sans l'extension)
			@return void
		*/
		virtual void save_mesh_off(const std::string& file_name) = 0;
		/**
			Sauvegarde un mesh sous le format OBJ
			@param string nom du fichier (sans l'extension)
			@return void
		*/
		virtual void save_mesh_obj(const std::string& file_name) = 0;
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
		friend std::ostream& operator<<(std::ostream& os, const Mesh& mesh);


	private:
		/**
			Calcul la normal d'une face à partir de son indice de face
			@param Face
			@return Vecteur
		*/
		Vecteur calculNormalTriangle(const Face& face) const;
		std::tuple<float, float, const Point&> calculCotanFaceV(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace);
		std::tuple<float, float, const Point&> calculCotanFaceF(const std::vector<size_t>& indexes, indiceFace indexeTerminal, indiceFace currentFace);
	protected:
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