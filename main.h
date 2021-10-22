#pragma once

#include <memory>
#include "Scene.h"

struct SDL_Renderer;
struct Vec2;
struct Vec3;

std::shared_ptr<Scene> CreateSceneFromLoadedResources();
void update(float deltaSeconds, std::shared_ptr<Scene> scene);

void PixelFillBenchmark();
void simdBenchmark();

void multithreadedFillBenchmark();