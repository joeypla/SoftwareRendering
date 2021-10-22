#include "WireMesh.h"
WireMesh::WireMesh() {}
void WireMesh::Cube(float size)
{
	// Front faces, starting top left going clockwise
	points.push_back(Vec4(-size / 2, size / 2, -size / 2, 1));
	points.push_back(Vec4(size / 2, size / 2, -size / 2, 1));
	points.push_back(Vec4(size / 2, -size / 2, -size / 2, 1));
	points.push_back(Vec4(-size / 2, -size / 2, -size / 2, 1));

	// back faces, starting top left going clockwise
	points.push_back(Vec4(-size / 2, size / 2, size / 2, 1));
	points.push_back(Vec4(size / 2, size / 2, size / 2, 1));
	points.push_back(Vec4(size / 2, -size / 2, size / 2, 1));
	points.push_back(Vec4(-size / 2, -size / 2, size / 2, 1));

	// front
	lines.push_back({ 0, 1 });
	lines.push_back({ 1, 2 });
	lines.push_back({ 2, 3 });
	lines.push_back({ 3, 0 });

	// back
	lines.push_back({ 4, 5 });
	lines.push_back({ 5, 6 });
	lines.push_back({ 6, 7 });
	lines.push_back({ 7, 4 });

	// left connecting
	lines.push_back({ 0, 4 });
	lines.push_back({ 3, 7 });

	// right connecting
	lines.push_back({ 1, 5 });
	lines.push_back({ 2, 6 });
}