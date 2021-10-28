#include "Material.h"
#include "main.h"
#include "Geometry.h"

bool Material::LoadDiffuse(std::string path) {
	diffuse = std::unique_ptr<Texture>(new Texture());
	return diffuse->Load(path);
}

Vec4 Material::Shade(const Vertex& vi) const
{
	
	return diffuse->Sample(vi.uv.x, vi.uv.y);
}
