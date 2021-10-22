#include "Mesh.h"

void Mesh::v(const Vertex& v) {
	vertices.push_back(v);
}
void Mesh::i(unsigned int i) {
	indices.push_back(i);
}

void Mesh::SetMaterial(Material* mat) {
	material = mat;
}

Material* Mesh::GetMaterial() { return material; }
std::vector<Vertex>& Mesh::GetVertices() { return vertices; }
std::vector<unsigned int>& Mesh::GetIndices() { return indices; };
