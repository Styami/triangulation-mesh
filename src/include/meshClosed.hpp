#ifndef __MESHCLOSED__
#define __MESHCLOSED__
#include "mesh.hpp"
#include <map>

class MeshClosed : public Mesh {
    public:
		void loadMesh(const std::string& file_name) override;
		void save_mesh_off(const std::string& file_name) override;
        void save_mesh_obj(const std::string& file_name) override;
    private:
        inline bool existingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces);
        inline pairIndiceFaceSommet getExistingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces);
};

bool MeshClosed::existingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces) {
return (edgesFaces.find(std::pair(edge.second, edge.first)) != edgesFaces.end())
        || edgesFaces.find(edge) != edgesFaces.end();
}

pairIndiceFaceSommet MeshClosed::getExistingEdge(const arete& edge, const std::map<arete, pairIndiceFaceSommet>& edgesFaces) {
    return edgesFaces.find(edge) != edgesFaces.end() ? 
    edgesFaces.at(edge):
    edgesFaces.at(std::pair(edge.second, edge.first));
}

#endif