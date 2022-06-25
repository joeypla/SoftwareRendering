#include <vector>
#include <algorithm>

#include "Rasterizer.h"
#include "Math.h"
#include "Geometry.h"
#include "Material.h"
#include <iostream>
#include <SDL.h>

#include <thread>

static SDL_Renderer* activeRenderer = nullptr;
static SDL_Surface* colorSurface = nullptr;
static SDL_Surface* depthSurface = nullptr;
static SDL_Window* window = nullptr;
static Material* activeMaterial;

static int writeCount = 0;
static bool showScanline = false;
void Rasterizer::Init(SDL_Renderer* renderer, SDL_Window* pWindow)
{
    window = pWindow;
    colorSurface = SDL_GetWindowSurface(window);
    activeRenderer = renderer;
    CreateDepthBuffer();
}

void Rasterizer::SetMaterial(Material* material) {
    activeMaterial = material;
}

void WriteDepth(int x, int y, float depth)
{
    
    Uint32* pixel = (Uint32*)((Uint8*)depthSurface->pixels
        + y * depthSurface->pitch
        + x * depthSurface->format->BytesPerPixel);
    // Will extract this later to separate class for color.
    *pixel = *reinterpret_cast<Uint32*>(&depth);

    //writeCount++;
}

float GetDepth(int x, int y) {
    Uint32* pixel = (Uint32*)((Uint8*)depthSurface->pixels
        + y * depthSurface->pitch
        + x * depthSurface->format->BytesPerPixel);
    // Will extract this later to separate class for color.
    return *(float*)pixel;
}

void Write(int x, int y, Vec4 color)
{
    //PutPixel32_nolock(activeRenderer->)
    // color.Clamp01(); actually clamp seems to be VERY SLOW!

    // Will extract this later to separate class for color.
    Uint8 r = (Uint8)lround(color.x * 255.0f);
    Uint8 g = (Uint8)lround(color.y * 255.0f);
    Uint8 b = (Uint8)lround(color.z * 255.0f);
    Uint8 a = 255; // Don't care about alpha yet.

    SDL_SetRenderDrawColor(activeRenderer, r, g, b, a);
    SDL_RenderDrawPoint(activeRenderer, x, y);

    writeCount++;
}

void Write_fast(int x, int y, Vec4 color)
{
    //PutPixel32_nolock(activeRenderer->)
    // color.Clamp01(); actually clamp seems to be VERY SLOW!

    // Will extract this later to separate class for color.
    Uint8 r = (Uint8)lround(color.x * 255.0f);
    Uint8 g = (Uint8)lround(color.y * 255.0f);
    Uint8 b = (Uint8)lround(color.z * 255.0f);
    Uint8 a = 255; // Don't care about alpha yet.
    
    Uint32* pixel = (Uint32*)((Uint8*)colorSurface->pixels
        + y * depthSurface->pitch
        + x * depthSurface->format->BytesPerPixel);

    // Will extract this later to separate class for color.
    char* c = (char*)pixel;
    c[0] = r;
    c[1] = g;
    c[2] = b;
    c[3] = a;

    //writeCount++;

    writeCount++;
}

void Write_faster(Uint8* pixels, int pitch, int x, int y, Vec4 color) {
    Uint8* pixel = pixels + y * pitch + x * 4; // assumes 4 bytes per pixel.
    pixel[2] = (Uint8)(color.x * 255.0f);
    pixel[1] = (Uint8)(color.y * 255.0f);
    pixel[0] = (Uint8)(color.z * 255.0f);
    writeCount++;
}

float xLiney(int y, const Vec2& p0, const Vec2& p1) {
    // Find line equation - note: doing slope with respect to y, not x.
    float m = (p1.x - p0.x) / (p1.y - p0.y);
    float b = p0.x - m * p0.y;

    // now using equation x = my + b, find a midpoint.
    return m * y + b;
}

void scanline(int xStart, int xEnd, float depthStart, float depthEnd, int y, const Triangle& tri)
{
    // store simple values for cache coherency when calling write. 
    Uint8* surfacePixel = (Uint8*)colorSurface->pixels;
    int surfacePitch = colorSurface->pitch;

    for (int x = xStart; x < xEnd; x++) {
        float q = (float)(x - xStart) / (float)(xEnd - xStart);
        float z = (1.0f - q) * depthStart + q * depthEnd;

        // Given x, y, compute barycentric coordinates u, v, w
        // to interpolate vertex attributes.
        float u, v, w; // v0, v1, v1 weights.


        barycentric_fast(
            Vec3(tri.v0.pos),
            Vec3(tri.v1.pos),
            Vec3(tri.v2.pos),
            Vec3((float)x, (float)y, z),
            u, v, w);

        Vertex vi; // Putting outside of the loop to avoid excessive copies.
        //interpVertex_fast(tri, u, v, w, &vi);
        vi = interpVertex(tri.v0, tri.v1, tri.v2, u, v, w);

        if (vi.pos.z < GetDepth(x, y)) {
            Write_faster(surfacePixel, surfacePitch, x, y, activeMaterial->Shade(vi));
            //Write_faster(surfacePixel, surfacePitch, x, y, Vec4(1,1,1,1));
            WriteDepth(x, y, vi.pos.z);
            if (showScanline)
                SDL_UpdateWindowSurface(window);
        }
    }
}

void fill(const Vertex* pv0, const Vertex* pv1, const Vertex* pv2, const Triangle& tri){
    // Flat top.
    if (floor(pv0->pos.y) == floor(pv1->pos.y)) {
        // But we want pv0 to be on the left (smaller x).
        if (pv0->pos.x > pv1->pos.x)
            std::swap(pv0, pv1);

        for (int y = std::clamp((int)floor(pv0->pos.y), 0, Rasterizer::HEIGHT - 1); 
            y < std::clamp((int)floor(pv2->pos.y), 0, Rasterizer::HEIGHT - 1); 
            y++)
        {
            int xStart = std::clamp((int)floor(xLiney(y, pv0->pos.XY(), pv2->pos.XY())), 0, Rasterizer::WIDTH - 1);
            int xEnd = std::clamp((int)floor(xLiney(y, pv1->pos.XY(), pv2->pos.XY())), 0, Rasterizer::WIDTH - 1);

            // alpha for y value between highest and lowest scanline.
            float yq = (y - pv0->pos.y) / (pv2->pos.y - pv0->pos.y);
            float d02 = (1.0f - yq) * pv0->pos.z + yq * pv2->pos.z;
            float d12 = (1.0f - yq) * pv1->pos.z + yq * pv2->pos.z;

            scanline(xStart, xEnd, d02, d12, y, tri);
        }
    }
    else if (floor(pv1->pos.y) == floor(pv2->pos.y)) {
        // But we want pv1 to be on the left of pv2 (smaller x).
        if (pv1->pos.x > pv2->pos.x)
            std::swap(pv1, pv2);

        for (int y = std::clamp((int)floor(pv0->pos.y), 0, Rasterizer::HEIGHT - 1);
            y <= std::clamp((int)floor(pv1->pos.y), 0, Rasterizer::HEIGHT - 1); 
            y++) 
        {
            // since we're always going left to right, we need to know
            // which of two bottom vertices are first.
            int xStart = std::clamp((int)floor(xLiney(y, pv0->pos.XY(), pv1->pos.XY())), 0, Rasterizer::WIDTH - 1);
            int xEnd = std::clamp((int)floor(xLiney(y, pv0->pos.XY(), pv2->pos.XY())), 0, Rasterizer::WIDTH - 1);

            // alpha for y value between highest and lowest scanline.
            float yq = (y - pv0->pos.y) / (pv2->pos.y - pv0->pos.y);
            float d01 = (1.0f - yq) * pv0->pos.z + yq * pv1->pos.z;
            float d02 = (1.0f - yq) * pv0->pos.z + yq * pv2->pos.z;

            scanline(xStart, xEnd, d01, d02, y, tri);
        }
    }
    else {
        // Most cases of triangles. 3 different heights on screen.
        // Break it up into two triangles, flat top and bottom to 
        // make scanline easy.


        Vec3 p4 = Vec3(xLiney((int)floor(pv1->pos.y), pv0->pos.XY(), pv2->pos.XY()), pv1->pos.y, 0);
        // Now find depth for p4.
        float yq = (p4.y - pv0->pos.y) / (pv2->pos.y - pv0->pos.y);
        float d02 = (1.0f - yq) * pv0->pos.z + yq * pv2->pos.z;
        p4.z = d02;
        Vertex fakeVertex;
        fakeVertex.pos = Vec4(p4.x, p4.y, p4.z, 1);

        fill(pv0, pv1, &fakeVertex, tri); // Flat bottom.
        fill(pv1, &fakeVertex, pv2, tri); // Flat top.
        
        
    }
}

void fill_multithreaded(const Vertex* pv0, const Vertex* pv1, const Vertex* pv2, const Triangle& tri) {
    // Flat top.
    if (floor(pv0->pos.y) == floor(pv1->pos.y)) {
        // But we want pv0 to be on the left (smaller x).
        if (pv0->pos.x > pv1->pos.x)
            std::swap(pv0, pv1);

        
        
        int minY = std::clamp((int)floor(pv0->pos.y), 0, Rasterizer::HEIGHT - 1);
        int maxY = std::clamp((int)floor(pv2->pos.y), 0, Rasterizer::HEIGHT - 1);

        auto scanlineFunc = [](
            int startY,
            int endY, 
            const Vertex* pv0,
            const Vertex* pv1,
            const Vertex* pv2,
            const Triangle& tri) {

            for (int y = startY; y <= endY; y = y + 2) {
                int xStart = std::clamp((int)floor(xLiney(y, pv0->pos.XY(), pv2->pos.XY())), 0, Rasterizer::WIDTH - 1);
                int xEnd = std::clamp((int)floor(xLiney(y, pv1->pos.XY(), pv2->pos.XY())), 0, Rasterizer::WIDTH - 1);
                // alpha for y value between highest and lowest scanline.
                float yq = (y - pv0->pos.y) / (pv2->pos.y - pv0->pos.y);
                float d02 = (1.0f - yq) * pv0->pos.z + yq * pv2->pos.z;
                float d12 = (1.0f - yq) * pv1->pos.z + yq * pv2->pos.z;
                scanline(xStart, xEnd, d02, d12, y, tri);
            }
        };

        std::thread t1(scanlineFunc, minY, maxY, pv0, pv1, pv2, tri);
        std::thread t2(scanlineFunc, minY + 1, maxY, pv0, pv1, pv2, tri);
        
        t1.join();
        t2.join();
    }
    else if (floor(pv1->pos.y) == floor(pv2->pos.y)) {
        // But we want pv1 to be on the left of pv2 (smaller x).
        if (pv1->pos.x > pv2->pos.x)
            std::swap(pv1, pv2);

        int minY = std::clamp((int)floor(pv0->pos.y), 0, Rasterizer::HEIGHT - 1);
        int maxY = std::clamp((int)floor(pv1->pos.y), 0, Rasterizer::HEIGHT - 1);

        auto scanlineFunc = [](
            int startY,
            int endY,
            const Vertex* pv0,
            const Vertex* pv1,
            const Vertex* pv2,
            const Triangle& tri) {

                for (int y = startY; y <= endY; y = y + 2) {
                    int xStart = std::clamp((int)floor(xLiney(y, pv0->pos.XY(), pv1->pos.XY())), 0, Rasterizer::WIDTH - 1);
                    int xEnd = std::clamp((int)floor(xLiney(y, pv0->pos.XY(), pv2->pos.XY())), 0, Rasterizer::WIDTH - 1);
                    // alpha for y value between highest and lowest scanline.
                    float yq = (y - pv0->pos.y) / (pv2->pos.y - pv0->pos.y);
                    float d01 = (1.0f - yq) * pv0->pos.z + yq * pv1->pos.z;
                    float d02 = (1.0f - yq) * pv0->pos.z + yq * pv2->pos.z;
                    scanline(xStart, xEnd, d01, d02, y, tri);
                }
        };

        std::thread t1(scanlineFunc, minY, maxY, pv0, pv1, pv2, tri);
        std::thread t2(scanlineFunc, minY + 1, maxY, pv0, pv1, pv2, tri);
        t1.join();
        t2.join();
    }
    else {
        // Most cases of triangles. 3 different heights on screen.
        // Break it up into two triangles, flat top and bottom to 
        // make scanline easy.


        Vec3 p4 = Vec3(xLiney((int)floor(pv1->pos.y), pv0->pos.XY(), pv2->pos.XY()), pv1->pos.y, 0);
        // Now find depth for p4.
        float yq = (p4.y - pv0->pos.y) / (pv2->pos.y - pv0->pos.y);
        float d02 = (1.0f - yq) * pv0->pos.z + yq * pv2->pos.z;
        p4.z = d02;
        Vertex fakeVertex;
        fakeVertex.pos = Vec4(p4.x, p4.y, p4.z, 1);

        fill_multithreaded(pv0, pv1, &fakeVertex, tri); // Flat bottom.
        fill_multithreaded(pv1, &fakeVertex, pv2, tri); // Flat top.


    }
}


void Rasterizer::DrawLine_naive(const Vec2& p1, const Vec2& p2) {
    // Floor float values to get pixel integer values.
    int x1 = lround(p1.x);
    int x2 = lround(p2.x);

    float length = (p2 - p1).Length();
    int steps = lround(length);
    Vec2 dir = p2 - p1;
    dir.Normalize();

    Vec2 p = p1;
    while (steps-- > 0) {
        int x = lround(p.x);
        int y = lround(p.y);
        p = p + dir;
        Write(x, y, Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    }
}

void Rasterizer::DrawTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2)
{
    if (!activeMaterial || activeRenderer == nullptr) return;

    // Base case of drawing flat top or flat bottom.
    std::vector<const Vertex*> pVerts = {&v0,&v1,&v2 }; // overkill to use std sort.
    std::sort(pVerts.begin(), pVerts.end(),
        [](const Vertex* v1, const Vertex* v2) { 
            return v1->pos.y < v2->pos.y;
        }
    );

    const Vertex* pv0, *pv1, *pv2; // Screen points used for scanning.
    pv0 = pVerts[0];
    pv1 = pVerts[1];
    pv2 = pVerts[2];

    Triangle tri;
    tri.v0 = v0;
    tri.v1 = v1;
    tri.v2 = v2;

    fill(pv0, pv1, pv2, tri);
}

void Rasterizer::DrawTriangle(const Triangle& tri)
{
    Rasterizer::DrawTriangle(tri.v0, tri.v1, tri.v2);
}

// Case where all 3 vertices of a triangle lie on the outside of one of the six
// planes of clip space.
bool Rasterizer::EarlyFullClip(const Triangle& tri) {
    if (tri.v0.pos.x < -tri.v0.pos.w && tri.v1.pos.x < -tri.v1.pos.w && tri.v2.pos.x < -tri.v2.pos.w) return true; // -x
    if (tri.v0.pos.x > tri.v0.pos.w && tri.v1.pos.x > tri.v1.pos.w && tri.v2.pos.x > tri.v2.pos.w) return true; // +x
    
    if (tri.v0.pos.y < -tri.v0.pos.w && tri.v1.pos.y < -tri.v1.pos.w && tri.v2.pos.y < -tri.v2.pos.w) return true;
    if (tri.v0.pos.y > tri.v0.pos.w && tri.v1.pos.y > tri.v1.pos.w && tri.v2.pos.y > tri.v2.pos.w) return true;
    

    // Depends on if we want homogenous z coord [0,w] or [-w,w].
    if (tri.v0.pos.z < -tri.v0.pos.w && tri.v1.pos.z < -tri.v1.pos.w && tri.v2.pos.z < -tri.v2.pos.w) return true;
    if (tri.v0.pos.z > tri.v0.pos.w && tri.v1.pos.z > tri.v1.pos.w && tri.v2.pos.z > tri.v2.pos.w) return true;
    

    // WRONG! You can't do this you stupid fucking moron. If one point is on one side of box, and other on other side, this will pass.
    /*if (abs(tri.v0.pos.x) > abs(tri.v0.pos.w) && abs(tri.v1.pos.x) > abs(tri.v1.pos.w) && abs(tri.v2.pos.x) > abs(tri.v2.pos.w)) return true;
    if (abs(tri.v0.pos.y) > abs(tri.v0.pos.w) && abs(tri.v1.pos.y) > abs(tri.v1.pos.w) && abs(tri.v2.pos.y) > abs(tri.v2.pos.w)) return true;
    if (abs(tri.v0.pos.z) > abs(tri.v0.pos.w) && abs(tri.v1.pos.z) > abs(tri.v1.pos.w) && abs(tri.v2.pos.z) > abs(tri.v2.pos.w)) return true;*/
    return false;
}

bool Rasterizer::EarlyFullClip_NDC(const Triangle& tri) {
    if (tri.v0.pos.x < -1.0f && tri.v1.pos.x < -1.0f && tri.v2.pos.x < -1.0f) return true;
    if (tri.v0.pos.x < -1.0f && tri.v1.pos.x < -1.0f && tri.v2.pos.x < -1.0f) return true;
    if (tri.v0.pos.x < -1.0f && tri.v1.pos.x < -1.0f && tri.v2.pos.x < -1.0f) return true;
}


void Rasterizer::GeometryClipZNear(Triangle& tri, std::vector<Triangle>& newTris) {
    // Brings v0 and v1 onto the near plane. No new triangle
    // needs to be created.
    auto clip2 = [](Vertex* v0, Vertex* v1, const Vertex& v2) {
        float a20 = v2.pos.z / (v2.pos.z - v0->pos.z);
        float a21 = v2.pos.z / (v2.pos.z - v1->pos.z);

        Vertex v20 = interpolate(v2, *v0, a20);
        Vertex v21 = interpolate(v2, *v1, a21);

        // We reuse the triangle since we're just pulling the vertices
        // onto the near plane, with interpolated attributes.
        *v0 = v20;
        *v1 = v21;
    };

    // Assumes v0 is the vertex behind the near plane.
    auto clip1 = [](
        const Vertex& v0, const Vertex& v1, const Vertex& v2,
        Triangle& outTri1, Triangle& outTri2) {
            float a10 = v1.pos.z / (v1.pos.z - v0.pos.z);
            float a20 = v2.pos.z / (v2.pos.z - v0.pos.z);

            a10 -= 0.01f;
            a20 -= 0.01f;

            Vertex v10 = interpolate(v1, v0, a10);
            Vertex v20 = interpolate(v2, v0, a20);

            /*outTri1 = { v10, v1, v2 };
            outTri2 = { v10, v2, v20 };*/

            outTri1 = { v10, v1, v20 };
            outTri2 = { v20, v1, v2 };
    };

    // Checks to see if vertex is behind the near plane in homogenous clip-space.
    auto behindNear = [](const Vertex& v) -> bool {
        // WAHT THE FUCK
        return v.pos.z < -v.pos.w;//&& abs(v.pos.z) > abs(v.pos.w);
    };

    Vertex& v0 = tri.v0;
    Vertex& v1 = tri.v1;
    Vertex& v2 = tri.v2;

    // This can definitely most probably be cleaned up if we sort
    // the vertices by z before processing.
    if (behindNear(v0)) {
        if (behindNear(v1)) {
            clip2(&v0, &v1, v2);
            newTris.push_back(tri);
        }
        else if (behindNear(v2)) {
            clip2(&tri.v0, &tri.v2, tri.v1);
            newTris.push_back(tri);
        }
        else {
            // Clip just v0.
            Triangle new1, new2;
            clip1(tri.v0, tri.v1, tri.v2, new1, new2);
            newTris.push_back(new1);
            newTris.push_back(new2);
        }
    } else if (behindNear(v1)) {
        if (behindNear(v0)) {
            clip2(&tri.v0, &tri.v1, tri.v2);
            newTris.push_back(tri);
        }
        else if (behindNear(v2)) {
            clip2(&tri.v1, &tri.v2, tri.v0);
            newTris.push_back(tri);
        }
        else {
            // Clip just v1.
            Triangle new1, new2;
            clip1(tri.v1, tri.v0, tri.v2, new1, new2);
            newTris.push_back(new1);
            newTris.push_back(new2);
        }
    } else if (behindNear(v2)) {
        if (behindNear(v0)) {
            clip2(&tri.v0, &tri.v2, tri.v1);
            newTris.push_back(tri);
        }
        else if (behindNear(v1)) {
            clip2(&tri.v1, &tri.v2, tri.v0);
            newTris.push_back(tri);
        }
        else {
            // Clip just v2.
            Triangle new1, new2;
            clip1(tri.v2, tri.v0, tri.v1, new1, new2);
            newTris.push_back(new1);
            newTris.push_back(new2);
        }
    }
    else {
        // Nothing clipped, just fill with the original triangle.
        newTris.push_back(tri);
    }
}

void Rasterizer::CreateDepthBuffer() {
    depthSurface = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    ClearDepthBuffer();
}

void Rasterizer::ClearDepthBuffer() {
    //SDL_memset(depthSurface->pixels, c, depthSurface->h * depthSurface->pitch);
    memset(depthSurface->pixels, 9999999, depthSurface->h * depthSurface->pitch);
}

void Rasterizer::ClearColorBuffer() {
    SDL_memset(colorSurface->pixels, 0, colorSurface->h * colorSurface->pitch);
}
int Rasterizer::GetWriteCount() { return writeCount; }
void Rasterizer::ResetWriteCount() { writeCount = 0; }

void Rasterizer::ShowScanlineOnce() {
    showScanline = true;
}

void Rasterizer::ResetShowScanline() {
    showScanline = false;
}