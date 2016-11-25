#ifndef DYNAMICMESH_H
#define DYNAMICMESH_H

#include "mesh.h"

class DynamicMesh : public Mesh
{
public:
	DynamicMesh();

	DynamicMesh(std::vector<glm::vec3> vertex_data, std::vector<glm::vec2> uv_data);

	void Draw(std::vector<glm::vec3> vertex_data, std::vector<glm::vec2> uv_data);

	void SendData(std::vector<glm::vec3> vertex_data, std::vector<glm::vec2> uv_data);

protected:

private:

};

#endif // DYNAMICMESH_H