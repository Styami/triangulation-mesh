#ifndef __MESHOPENED__
#define __MESHOPENED__
#include "mesh.hpp"

class MeshOpened : public Mesh {
    public:
        /**
			Charge un mesh
			@param string file_name fichier à ouvrir
			@return void
		*/
		void loadMesh(const std::string& file_name) override;
        void save_mesh_off(const std::string& file_name) override;
        void save_mesh_obj(const std::string& file_name) override;

    private:
        void cleanDataIfOpen();
        /** 
			La fonction retourne si le point passé en paramètre est dans l'enveloppe convexe ou non ainsi que la face 
			dans laquelle le point appartient s'il se trouve dans l'enveloppe convexe 
			@param Point point verifié
			@return pair<bool, indiceFace>
		*/
		std::pair<bool, indiceFace> inConvexHull(const Point& p) const;
		/**
			Permet de savoir si les trois points passé en paramètre sont placé dans le sens trigonométrique
			@param p1, p2, p3 Les points formant le triangle
			@return <0 si c'est dans le sens horaire / 0 si le triangle est plat / >0 si c'est dans le sens horaire
		*/
		int inTriangle(const Face& face, const Point& p) const;
        /**
			Ajout d'un point au mesh
			@param Point coordonnées du point à insérer
			@return void
		*/
		void addPoint(const Point& newVec);
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
		float orientationTest(const Sommet& p1, const Sommet& p2, const Sommet& p3) const;
        void split3(const indiceFace indiceFace, const indiceGlobalSommet indiceVertex);
		void edgeSplit(const arete& edge, const float interpolateValue);
		void edgeFlip(const arete& edge);
		void lawson(const indiceFace currentFaceId1,  const indiceFace currentFaceId2, const indiceFace currentFaceId3);
};

#endif