#pragma once

#include "Math.h"
#include <vector>
#include <memory>
#include "Material.h"
#include "Geometry.h"
class Mesh {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Material* material;
public:
	Mesh()=default;
	void v(const Vertex& v);
	void i(unsigned int i);
	void SetMaterial(Material* mat);
	Material* GetMaterial();
	std::vector<Vertex>& GetVertices();
	std::vector<unsigned int>& GetIndices();
};