#include "Geometry.h"
#include <immintrin.h>

// Note: This is slow, but the easiest way to understand
// barycentric coords for me is that u, the weight for v0, is the ratio between the
// areas of the triangle made up of p and the other two points, and the entire triangle.
void barycentric(Vec3 v0, Vec3 v1, Vec3 v2, Vec3 p, float& u, float& v, float& w) {
    float area = Vec3::Cross(v1 - v0, v2 - v0).Length();
    u = Vec3::Cross(v1 - p, v2 - p).Length() / area; // area opposite v0.
    v = Vec3::Cross(v0 - p, v2 - p).Length() / area; // area opposite v1.
    w = Vec3::Cross(v0 - p, v1 - p).Length() / area; // area opposite v2.
}

// https://math.stackexchange.com/questions/1887215/fast-way-of-computing-barycentric-coordinates-explained
void barycentric_fast(const Vec3& a, const Vec3& b, const Vec3& c, const Vec3& p, float& u, float& v, float& w)
{
#define simd
#define expand



#ifdef simd
    __m128 _ma = _mm_set_ps(a.x, a.y, a.z, 0);
    __m128 _mb = _mm_set_ps(b.x, b.y, b.z, 0);
    __m128 _mc = _mm_set_ps(c.x, c.y, c.z, 0);
    __m128 _mp = _mm_set_ps(p.x, p.y, p.z, 0);
    

    __m128 _mba = _mm_sub_ps(_mb, _ma);
    __m128 _mca = _mm_sub_ps(_mc, _ma);
    __m128 _mpa = _mm_sub_ps(_mp, _ma);

#else

#ifdef expand
    Vec3 v0(b.x - a.x, b.y - a.y, b.z - a.z);
    Vec3 v1(c.x - a.x, c.y - a.y, c.z - a.z);
    Vec3 v2(p.x - a.x, p.y - a.y, p.z - a.z);
#else
    Vec3 v0 = b - a, v1 = c - a, v2 = p - a;
#endif

#endif
    


#ifdef simd
    auto md00 = _mm_mul_ps(_mba, _mba);
    float d00 = md00.m128_f32[3] + md00.m128_f32[2] + md00.m128_f32[1];

    auto md01 = _mm_mul_ps(_mba, _mca);
    float d01 = md01.m128_f32[3] + md01.m128_f32[2] + md01.m128_f32[1];

    auto md11 = _mm_mul_ps(_mca, _mca);
    float d11 = md11.m128_f32[3] + md11.m128_f32[2] + md11.m128_f32[1];

    auto md20 = _mm_mul_ps(_mpa, _mba);
    float d20 = md20.m128_f32[3] + md20.m128_f32[2] + md20.m128_f32[1];

    auto md21 = _mm_mul_ps(_mpa, _mca);
    float d21 = md21.m128_f32[3] + md21.m128_f32[2] + md21.m128_f32[1];


#else
#ifdef expand
    float d00 = v0.x * v0.x + v0.y * v0.y + v0.z * v0.z;
    float d01 = v0.x * v1.x + v0.y * v1.y + v0.z * v1.z;
    float d11 = v1.x * v1.x + v1.y * v1.y + v1.z * v1.z;
    float d20 = v2.x * v0.x + v2.y * v0.y + v2.z * v0.z;
    float d21 = v2.x * v1.x + v2.y * v1.y + v2.z * v1.z;
#else
    float d00 = v0 * v0;
    float d01 = v0 * v1;
    float d11 = v1 * v1;
    float d20 = v2 * v0;
    float d21 = v2 * v1;
#endif

#endif


#undef expand
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}

void barycentric2d_fast(Vec2 a, Vec2 b, Vec2 c, Vec2 p, float& u, float& v, float& w)
{
    Vec2 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = v0 * v0;
    float d01 = v0 * v1;
    float d11 = v1 * v1;
    float d20 = v2 * v0;
    float d21 = v2 * v1;
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}


Vertex interpVertex(const Vertex& v0, const Vertex& v1, const Vertex& v2, float u, float v, float w)
{
    Vertex vi;
    vi.pos = v0.pos * u + v1.pos * v + v2.pos * w;
    vi.color = v0.color * u + v1.color * v + v2.color * w;
    vi.uv = v0.uv * u + v1.uv * v + v2.uv * w;
    vi.inverseW = v0.inverseW * u + v1.inverseW * v + v2.inverseW * w;
    return vi;
}

// Trying to get this faster by avoiding copies??

// To-do: Is it possible that the read access violation is caused by a pointer to a const reference?
void interpVertex_fast(const Triangle& tri, 
    float u, float v, float w, Vertex* vOut)
{
//#define simd
#ifdef simd
    __m128 mu = _mm_set1_ps(u);
    __m128 mv = _mm_set1_ps(v);
    __m128 mw = _mm_set1_ps(w);
    __m128 mpos0 = _mm_load_ps((float*)&tri.v0.pos.x);
    __m128 mpos1 = _mm_load_ps((float*)&tri.v1.pos.x);
    __m128 mpos2 = _mm_load_ps((float*)&tri.v2.pos.x);

    auto v0u = _mm_mul_ps(mpos0, mu);
    auto v1v = _mm_mul_ps(mpos1, mv);
    auto v2w = _mm_mul_ps(mpos2, mw);
    auto sumPos = _mm_add_ps(_mm_add_ps(v0u, v1v), v2w);
    _mm_store_ps(&vOut->pos.x, sumPos);
#else
    vi.pos.x = v0.pos.x * u + v1.pos.x * v + v2.pos.x * w;
    vi.pos.y = v0.pos.y * u + v1.pos.y * v + v2.pos.y * w;
    vi.pos.z = v0.pos.z * u + v1.pos.z * v + v2.pos.z * w;
    vi.pos.w = v0.pos.w * u + v1.pos.w * v + v2.pos.w * w;
#endif
    /*vi.color.x = v0.color.x * u + v1.color.x * v + v2.color.x * w;
    vi.color.y = v0.color.y * u + v1.color.y * v + v2.color.y * w;
    vi.color.z = v0.color.z * u + v1.color.z * v + v2.color.z * w;
    vi.color.w = v0.color.w * u + v1.color.w * v + v2.color.w * w;*/

    vOut->uv.x = tri.v0.uv.x * tri.v0.inverseW * u + tri.v1.uv.x * tri.v1.inverseW * v + tri.v2.uv.x * tri.v2.inverseW * w;
    vOut->uv.y = tri.v0.uv.y * tri.v0.inverseW * u + tri.v1.uv.y * tri.v1.inverseW * v + tri.v2.uv.y * tri.v2.inverseW * w;

    
    vOut->inverseW = tri.v0.inverseW * u + tri.v1.inverseW * v + tri.v2.inverseW * w;
    vOut->uv.x /= vOut->inverseW;
    vOut->uv.y /= vOut->inverseW;
}

Vertex interpolate(const Vertex& v1, const Vertex& v2, float alpha)
{
    return {
        (1.0f - alpha) * v1.pos + v2.pos * alpha,
        (1.0f - alpha) * v1.normal + v2.normal * alpha,
        (1.0f - alpha) * v1.uv + v2.uv * alpha,
        (1.0f - alpha) * v1.color + v2.color * alpha
    };
}
