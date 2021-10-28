
#include <SDL.h>
#include <SDL_image.h>

#include <stdio.h>
#include <algorithm>
#include <filesystem>
#include <iostream>

#include "Math.h"
#include "main.h"
#include "WireMesh.h"
#include "Mesh.h"
#include "Texture.h"
#include "Resources.h"
#include "Rasterizer.h"
#include "Scene.h"

#include "Helpers.h"
float rf(float min, float max)
{
    // this  function assumes max > min, you may want 
    // more robust error checking for a non-debug build
    float random = ((float)rand()) / (float)RAND_MAX;

    // generate (in your case) a float between 0 and (4.5-.78)
    // then add .78, giving you a float between .78 and 4.5
    float range = max - min;
    return (random * range) + min;
}

static float timeT;
static SDL_Renderer* activeRenderer;

int main(int argc, char* args[])
{

    std::cout << std::filesystem::current_path();
    SDL_Window* window = NULL;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        SDL_Delay(2000);
        SDL_Quit();        
        return 0;
    }

    SDL_CreateWindowAndRenderer(Rasterizer::WIDTH, Rasterizer::HEIGHT, 0, &window, &activeRenderer);
    Rasterizer::Init(activeRenderer, window);
    
    int tic = (int)(1.0f / 60.0f * 1000.0f); // for milliseconds
    float rotate = 0.0f;

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "Failed to init SDL_IMG" << std::endl;
        return 0;
    }



    // A few benchmarks that I should move into a separate performance tests module.
    //PixelFillBenchmark();
    //multithreadedFillBenchmark();
    //simdBenchmark();
    // 
    // Load Test texture
    std::vector<std::string> objects = {
        std::string("resources/doom_E1M1.obj"),
        std::string("resources/DefaultCube/defaultcube.obj"),
        std::string("resources/DefaultCube/flattriangle_1p.obj"),
        std::string("resources/NatureFreePack1.obj")
    };

    bool bLoaded = Resources::LoadObjFile(objects[0]);

    if (!bLoaded)
        std::cout << "Could not load file." << std::endl;


    std::shared_ptr<Scene> scene = CreateSceneFromLoadedResources();
    


    float deltaSeconds = 0;
    // render loop
    while (true)
    {   
        Uint32 startTicks = SDL_GetTicks();
        SDL_Event* e = NULL;
        SDL_PollEvent(e);
        if (e != NULL && e->type == SDL_QUIT) {
            break;
        }

        update(deltaSeconds, scene);

        /*auto& cam = scene->GetCamera();
        cam.ry += (0.2f * 2 * 3.14159f / 60.0f);
        cam.z = -3800.0f + (sin(rotate) * 300.0f);
        cam.y = 30.0f + sin(rotate) * 100.0f;*/
        {
            auto _ = Benchmark("Scene Render");
            scene->Render(activeRenderer);
        }

        timeT += 1.0f / 60.0f;
        //SDL_Delay(tic);
        Uint32 endTicks = SDL_GetTicks();
        deltaSeconds = ((float)endTicks - (float)startTicks) / 1000.0f;
        SDL_UpdateWindowSurface(window);
    }

    SDL_Quit();
	return 0;
}

void update(float deltaSeconds, std::shared_ptr<Scene> scene) {
    const Uint8 * keystate = SDL_GetKeyboardState(NULL);
    Camera& cam = scene->GetCamera();
    if (keystate[SDL_SCANCODE_UP]) {
        cam.y += deltaSeconds * 100.0f;
    }

    if (keystate[SDL_SCANCODE_DOWN]) {
        cam.y -= deltaSeconds * 100.0f;
    }

    if (keystate[SDL_SCANCODE_LEFT]) {
        cam.x -= deltaSeconds * 100.0f;
    }

    if (keystate[SDL_SCANCODE_RIGHT]) {
        cam.x += deltaSeconds * 100.0f;
    }

    if (keystate[SDL_SCANCODE_KP_MINUS]) {
        cam.z -= deltaSeconds * 100.0f;
    }

    if (keystate[SDL_SCANCODE_KP_PLUS]) {
        cam.z += deltaSeconds * 100.0f;
    }

    if (keystate[SDL_SCANCODE_COMMA]) {
        cam.ry += deltaSeconds * 100.0f;
    }

    if (keystate[SDL_SCANCODE_COMMA]) {
        cam.ry += deltaSeconds * 1.0f;
    }

    if (keystate[SDL_SCANCODE_PERIOD]) {
        cam.ry -= deltaSeconds * 1.0f;
    }

    if (keystate[SDL_SCANCODE_SPACE]) {
        Rasterizer::ShowScanlineOnce();
    }
}

std::shared_ptr<Scene> CreateSceneFromLoadedResources() {

    auto scene = std::shared_ptr<Scene>(new Scene());
    auto& meshes = Resources::Meshes();
    for (auto mi = meshes.cbegin(); mi != meshes.cend(); mi++) {
        scene->AddInstance({
            mi->second.get(),
            Mat4::Identity()
        });
    }

    // Doom level values.
    auto& cam = scene->GetCamera();
    cam.near = 1.0f;
    cam.far = 5000.0f;
    cam.fov = 3.14159f / 2.0f; // 90 degrees.
    cam.x = -1600.0f;
    cam.y = 30.0f;
    cam.z = -3800.0f;

    //auto& cam = scene->GetCamera();
    //cam.near = 0.1f;
    //cam.far = 200.0f;
    //cam.fov = 3.14159f / 2.0f; // 90 degrees.
    //cam.z = -5.0f;
    //cam.y = 0;
    return scene;
}

void set(SDL_Surface* surface, int x, int y, int c)
{
    //PutPixel32_nolock(activeRenderer->)
    // color.Clamp01(); actually clamp seems to be VERY SLOW!

    //// Will extract this later to separate class for color.
    //Uint8 r = (Uint8)lround(color.x * 255.0f);
    //Uint8 g = (Uint8)lround(color.y * 255.0f);
    //Uint8 b = (Uint8)lround(color.z * 255.0f);
    //Uint8 a = 255; // Don't care about alpha yet.

    Uint32* pixel = (Uint32*)((Uint8*)surface->pixels
        + y * surface->pitch
        + x * surface->format->BytesPerPixel);

    *pixel = c;
    //// Will extract this later to separate class for color.
    //char* c = (char*)pixel;
    //c[0] = r;
    //c[1] = g;
    //c[2] = b;
    //c[3] = a;
}

void PixelFillBenchmark() {
    std::cout << "STARTING SINGLE THREADED FILL" << std::endl;
    int width = 2048;
    int height = 2048;

    auto surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

    for (int i = 0; i < 50; i++) {
        {
            Benchmark b("fill test");

#if 1
            int pitch = surface->pitch;
            int bytesPerPixel = surface->format->BytesPerPixel;
            Uint8* pixels = (Uint8*)surface->pixels;
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < height; x++) {
                    Uint32* pixel = (Uint32*)(pixels + y * pitch + x * bytesPerPixel);
                    *pixel = 10;
                    
                }
            }
#else
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < height; x++) {
                    set(surface, x, y, 10);
                }
            }
#endif

            
        }

        std::cout << Benchmark::timings.find("fill test")->second << std::endl;

    }
    int dummyVariable = 0;
}

#include <immintrin.h>

void simdBenchmark() {
    Vec3 thing(1, 2, 3);
    Vec3 thing2(3.0f, 1.0f, 2.0f);

    __m128 _1 = _mm_set_ps(thing.x, thing.y, thing.z, 0);
    __m128 _2 = _mm_set_ps(thing2.x, thing2.y, thing2.z, 0);

    __m128 times = _mm_mul_ps(_1, _2);

    Vec3 newThing(times.m128_f32[3], times.m128_f32[2], times.m128_f32[1]);
    std::cout << newThing.x << ", " << newThing.y << ", " << newThing.z << std::endl;
}

#include <thread>
void multithreadedFillBenchmark() {
    std::cout << "STARTING MULTITHREADED FILL BENCH!" << std::endl;
    int width = 2048;
    int height = 2048;
    
    auto surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    int samples = 200;
    for (int t = 1; t <= 4; t++) {
        float average = 0;
        for (int i = 0; i < samples; i++) {
            {
                Benchmark b("fill_multi");

                auto fillFunc = [](int yStart, int yEnd, Uint8* pixels, int width, int pitch, int bytesPerPixel, int numThreads) {
                    Uint32* line = new Uint32[width];
                    for (int y = yStart; y <= yEnd; y = y + numThreads) {
                        for (int x = 0; x < width; x++) {
                            line[x] = 10;
                        }

                        memcpy((Uint32*)(pixels + y * pitch), line, width);
                    }
                    delete [] line;
                };

                std::vector<std::thread*> threads;
                for (int i = 0; i < t; i++) {
                    int lines = height / t;
                    int yStart = i * height / 4;
                    int yEnd = yStart + lines - ((i == t - 1) ? 1 : 0);
                    std::thread* t1 = new std::thread(fillFunc, yStart, yEnd, (Uint8*)surface->pixels, width, surface->pitch, surface->format->BytesPerPixel, t);
                    threads.push_back(t1);
                }


                for (int i = 0; i < t; i++) {
                    threads[i]->join();
                }

                for (int i = 0; i < t; i++) {
                    delete threads[i];
                }
            }

            
            average += Benchmark::timings.find("fill_multi")->second;
        }

        average /= (float)samples;
        average *= 1000.0f;
        std::cout << "Threads(" << t << ") => " << average << "ms" << std::endl;
    }
}
