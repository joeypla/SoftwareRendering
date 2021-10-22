#include "Resources.h"

#include <memory>
#include <utility>

#include "ObjLoader.h"
#include "Mesh.h"
#include "Material.h"


// Originally, I had shared_ptr here, but I misunderstood its purpose. A unique ptr
// is appropriate given that I want this module to have ownership of these objects, however
// I want my meshes to have weak references to the pointer (only works with shared_ptr). 

// See this link for an "observer"-augmented type pointer that handles the target becoming invalid
// correctly (in addition to removing ability to call delete)
// https://www.codeproject.com/Articles/1011134/Smart-observers-to-use-with-unique-ptr

// This is worth going over again as well:
// https://stackoverflow.com/questions/43569581/non-ownership-copies-of-stdunique-ptr?rq=1

std::unordered_map<std::string, std::unique_ptr<Mesh>> meshes;
std::unordered_map<std::string, std::unique_ptr<Material>> materials;

bool Resources::LoadObjFile(std::string path) {
	objl::Loader l;
	bool found = l.LoadFile(path);

	for (int i = 0; i < l.LoadedMaterials.size(); i++) {
		objl::Material* m = &l.LoadedMaterials[i];

		// need to load texture for material.
		Material* newMat = new Material();
		newMat->LoadDiffuse(m->map_Kd);
		materials.insert(std::make_pair(m->name, newMat));
	}

	for (int i = 0; i < l.LoadedMeshes.size(); i++) {
		objl::Mesh* m = &l.LoadedMeshes[i];
		Mesh* newMesh = new Mesh();
		std::string materialName = m->MeshMaterial.name;
		auto v = materials.find(materialName);
		if (v != materials.end())
		{
			newMesh->SetMaterial(v->second.get());
		}
		
		// Load/Convert vertices and indices.
		for (int j = 0; j < m->Vertices.size(); j++) {
			objl::Vertex& vo = m->Vertices[j];

			Vertex v = {
				Vec4(vo.Position.X, vo.Position.Y, vo.Position.Z, 1),
				Vec4(vo.Normal.X, vo.Normal.Y, vo.Normal.Z, 0),
				Vec2(vo.TextureCoordinate.X, vo.TextureCoordinate.Y),
				Vec4(1,1,1,1)
			};

			newMesh->v(v);
		}

		for (int j = 0; j < m->Indices.size(); j++) {
			newMesh->i(m->Indices[j]);
		}
		std::string name = m->MeshName + "/" + std::to_string(i);

		meshes.insert(std::make_pair(name, newMesh));
	}

	int i = 0;
	return found;
}

using namespace std; // Still haven't settled on standard for including standard.. lol
const unordered_map<string, unique_ptr<Mesh>>& Resources::Meshes()
{
	return meshes;
}

