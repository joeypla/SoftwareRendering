#pragma once

#include <vector>
#include <utility>
#include "Math.h"
class WireMesh
{
public:
	WireMesh();

	std::vector<Vec4> points;
	std::vector<std::pair<int, int>> lines;

	void Cube(float size);
};