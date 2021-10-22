#pragma once

#include "Texture.h"
#include <memory>
struct Vertex;

class Material {
	std::unique_ptr<Texture> diffuse;

public:

	bool LoadDiffuse(std::string path);
	Vec4 Shade(const Vertex& vi) const;
};