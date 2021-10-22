#pragma once

#include <vector>
#include <SDL.h>

#include "Math.h"

class Mesh;
struct Instance
{
	Mesh* mesh;
	Mat4 transform;
};

struct Camera {
	Mat4 transform;
	float fov;
	float near;
	float far;

	float x;
	float y;
	float z;

	float rx;
	float ry;
	float rz;

	Mat4 GetTransform();
};

class Scene {
	std::vector<Instance> instances;
	Camera camera;
public:
	void Render(SDL_Renderer* renderer);

	void AddInstance(const Instance& instance);
	Camera& GetCamera();
};