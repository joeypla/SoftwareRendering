#pragma once

#include <string>
#include <unordered_map>
#include <SDL.h>
class Benchmark {
public:
	static std::unordered_map<std::string, double> timings;
private:
	Uint64 start;
	Uint64 end;

	std::string name;
public:
	Benchmark(std::string name);
	~Benchmark();
};