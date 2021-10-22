#include "Texture.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cmath>
Texture::Texture()
{
	surface = NULL;
}

bool Texture::Load(std::string path)
{
	surface = IMG_Load(path.c_str());
	return surface != NULL;
}

Vec4 Texture::Sample(float u, float v)
{
	float modu = u > 0.0f ? u - floor(u) : u - floor(u);
	float modv = v > 0.0f ? v - floor(v) : v - floor(v);
	// map u, v coordinate to pixel space.
	int x = (int)(modu * (surface->w - 1));
	int y = (int)((1.0f - modv) * (surface->h - 1));
	
	auto pixel = (Uint8*)surface->pixels
			+ y * surface->pitch
			+ x * surface->format->BytesPerPixel;

	return Vec4(pixel[0] / 255.0f, pixel[1] / 255.0f, pixel[2] / 255.0f, pixel[3] / 255.0f);
}


