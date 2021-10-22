#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "Mesh.h"

namespace Resources
{
	bool LoadObjFile(std::string path);

	const std::unordered_map<std::string, std::unique_ptr<Mesh>>& Meshes();

}