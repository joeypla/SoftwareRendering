#pragma once
#include <string>
#include "Math.h"

struct SDL_Surface;

class Texture {
public:
	Texture();

	bool Load(std::string path);
	Vec4 Sample(float u, float v);
private:
	SDL_Surface* surface;
};