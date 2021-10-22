#pragma once
#include "Math.h"

struct Vertex {
	Vec4 pos;
	Vec4 normal;
	Vec2 uv;
	Vec4 color;
	float inverseW;
};

struct Triangle {
	Vertex v0;
	Vertex v1;
	Vertex v2;
};

void barycentric(
	Vec3 v0, Vec3 v1, Vec3 v2,
	Vec3 p,
	float& u, float& v, float& w
);

void barycentric_fast(
const Vec3& a, const Vec3& b, const Vec3& c,
const Vec3& p,
float& u, float& v, float& w
);

void barycentric2d_fast(
	Vec2 a, Vec2 b, Vec2 c,
	Vec2 p,
	float& u, float& v, float& w
);

Vertex interpVertex(
	const Vertex& v0, const Vertex& v1, const Vertex& v2,
	float u, float v, float w
);

void interpVertex_fast(
	const Triangle& tri,
	float u, float v, float w, Vertex* vOut
);

Vertex interpolate(const Vertex& v1, const Vertex& v2, float alpha);