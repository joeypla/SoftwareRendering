#include "Helpers.h"

using namespace std;

unordered_map<string, double> Benchmark::timings;

Benchmark::Benchmark(string name) {
	this->name = name;
	start = SDL_GetPerformanceCounter();
}

Benchmark::~Benchmark()
{
	end = SDL_GetPerformanceCounter();
	double timing = ((double)end - (double)start) / (double)SDL_GetPerformanceFrequency();

	auto it = timings.find(name);
	if (it != timings.end()) {
		it->second = timing;
	}
	else {
		timings.insert(make_pair(name, timing));
	}
}