#include "Scene.h"
#include <memory>

#include "Geometry.h"
#include "Mesh.h"

#include "Rasterizer.h"
#include "Helpers.h"
#include <iostream>

// Very simple linear rendering of scene, object by object.
// No need for graph or acceleration structures.
static int frameNumber = 0; // just for debug temporarily. remove later.
void Scene::Render(SDL_Renderer* renderer) {
	
	Rasterizer::ResetWriteCount();
	Rasterizer::ClearDepthBuffer();
	Rasterizer::ClearColorBuffer();

	Mat4 view = camera.GetTransform().Inverse();
	Mat4 proj = Mat4::PerspectiveProjection(camera.fov, camera.near, camera.far);
	Mat4 projView = proj * view;
	Mat4 screen = Mat4::Viewport(Rasterizer::WIDTH, Rasterizer::HEIGHT);
	Mat4 screenProj = screen * proj;

	{
		Benchmark frameTime("FrameTime");

		for (int mi = 0; mi < instances.size(); mi++) {

			auto instance = instances[mi];
			Mat4 model = instance.transform;
			Mat4 transform = projView * instance.transform;
			Mesh* mesh = instance.mesh;

			Rasterizer::SetMaterial(mesh->GetMaterial());
			std::vector<Vertex>& vertices = mesh->GetVertices();
			std::vector<unsigned int>& indices = mesh->GetIndices();
			for (int i = 0; i < indices.size(); i += 3) {
				if (i + 2 >= indices.size()) break;

				/*if (mi == 23 && frameNumber == 9 && i == 21) {
					std::cout << "Geometry divide by zero alpha" << std::endl;
				}*/

				/*if (mi == 5 && frameNumber == 0 && i == 27) {
					std::cout << "Geometry divide by zero alpha" << std::endl;
				}*/

				Triangle tri = {
					vertices[indices[i]],
					vertices[indices[i + 1]],
					vertices[indices[i + 2]]
				};
				auto t = [](Triangle& tri, Mat4& transform) {
					tri.v0.pos = transform * tri.v0.pos;
					tri.v1.pos = transform * tri.v1.pos;
					tri.v2.pos = transform * tri.v2.pos;
				};

				auto divide = [](Vertex& v) {
					v.inverseW = 1.0f / v.pos.w;
					v.pos /= v.pos.w;
				};

				auto p = [divide](Triangle& tri) {
					if (tri.v0.pos.w != 0)
						divide(tri.v0);

					if (tri.v1.pos.w != 0)
						divide(tri.v1);

					if (tri.v2.pos.w != 0)
						divide(tri.v2);
				};



				t(tri, model);
				t(tri, view);


				t(tri, proj);

				// Before we put coordinates into ndc space with perspective divide, we're going to do some
				// quick clipping tests.
				if (Rasterizer::EarlyFullClip(tri))
					continue;

				/*Vec3 projNormal = Vec3::Cross(Vec3(tri.v1.pos - tri.v0.pos), Vec3(tri.v2.pos - tri.v0.pos));
				if ((projNormal * Vec3(0, 0, -1)) >= 0.0f) continue;*/

				std::vector<Triangle> tris;
				Rasterizer::GeometryClipZNear(tri, tris);


				for (auto& triangle : tris) {
					// Perspective divide (Clip space => NDC space)
					p(triangle);

					// Check for back face culling.




					t(triangle, screen);

					Rasterizer::DrawTriangle(triangle);
				}
			}
		}
	}
	int writeCount = Rasterizer::GetWriteCount();
	//std::cout << "Raster write Count: " << writeCount << std::endl;
	//std::cout << "Frame time(ms): " << Benchmark::timings.find("FrameTime")->second << std::endl;
	frameNumber++;
}

void Scene::AddInstance(const Instance& instance)
{
	instances.push_back(instance);
}

Camera& Scene::GetCamera()
{
	return camera;
}

Mat4 Camera::GetTransform()
{
	return Mat4::Translation(x, y, z) * Mat4::Rotation(rx, ry, rz);
}
