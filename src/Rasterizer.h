#pragma once
#include <memory>
#include <SDL.h>
#include "Material.h"
struct Vertex;
struct Triangle;
class Material;

namespace Rasterizer {
	const int WIDTH = 512;
	const int HEIGHT = 512;

	void Init(SDL_Renderer* renderer, SDL_Window* pWindow);
	void SetMaterial(Material* material);
	void DrawLine_naive(const Vec2& p1, const Vec2& p2);
	void DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	void DrawTriangle(const Triangle& tri);
	bool EarlyFullClip(const Triangle& tri);
	bool EarlyFullClip_NDC(const Triangle& tri);
	void GeometryClipZNear(Triangle& tri, std::vector<Triangle>& newTris);
	void CreateDepthBuffer();
	void ClearDepthBuffer();
	void ClearColorBuffer();
	int GetWriteCount();
	void ResetWriteCount();
	void ShowScanlineOnce();
	void ResetShowScanline();
}