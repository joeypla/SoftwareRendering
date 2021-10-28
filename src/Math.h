#pragma once

#include <string>

struct Vec2
{
	float x, y;
	Vec2(float x, float y);
	Vec2(int x, int y); // to avoid warnings.
	Vec2();
	friend Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
	friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
	friend Vec2 operator*(const Vec2& lhs, const float rhs);
	friend Vec2 operator*(const float lhs, const Vec2& rhs);
	friend float operator*(const Vec2& lhs, const Vec2& rhs);

	void Normalize();
	float Length();

	static bool FloorEqual(const Vec2& v1, const Vec2& v2);
	static bool CeilEqual(const Vec2& v1, const Vec2& v2);
	static bool RoundEqual(const Vec2& v1, const Vec2& v2);
};


struct Vec4
{
	Vec4();
	Vec4(float x, float y, float z, float w);
	void Clamp01();
	float x, y, z, w;
	float operator *(const Vec4& rhs) const;
	Vec4& operator/=(float rhs);
	Vec2 XY() const;
	friend Vec4 operator*(const float lhs, const Vec4& rhs);
	friend Vec4 operator*(const Vec4& lhs, const float rhs);
	friend Vec4 operator+(const Vec4& lhs, const Vec4& rhs);
	friend Vec4 operator-(const Vec4& lhs, const Vec4& rhs);
	std::string ToString() const;
};

struct Vec3
{
	Vec3();
	Vec3(float x, float y, float z);
	Vec3(const Vec4& vec); // w loss
	float x, y, z;
	float operator *(const Vec3& rhs);
	friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
	friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
	static Vec3 Cross(const Vec3& a, const Vec3& b);
	float Length();
};

// Only used to construct minors.
struct Mat3 {
	float m[3][3]; // [rows][columns]

	float Determinant();
	std::string ToString() const;
};

struct Mat4
{
	float m[4][4]; // [rows][columns]
	Mat4();

	Mat4 operator*(const Mat4& m2) const;
	Vec4 operator*(const Vec4& rhs) const;
	Mat4 operator*(const float rhs) const;
	Vec4 Column(int col) const;
	Vec4 Row(int row) const;

	Mat4 Inverse();
	Mat4 Transpose();
	Mat4 CoFactor();
	float Determinant();
	Mat3 Minor(int row, int col);
	std::string ToString() const;

	static Mat4 Identity();
	static Mat4 Translation(float x, float y, float z);
	static Mat4 Rotation(float x, float y, float z);
	static Mat4 RotationX(float x);
	static Mat4 RotationY(float y);
	static Mat4 RotationZ(float z);
	static Mat4 Scale(float x, float y, float z);
	static Mat4 Scale(float s);
	static Mat4 PerspectiveProjection(float fov, float near, float far);
	static Mat4 Viewport(int width, int height);
};

