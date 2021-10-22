#include "Math.h"
#include <cmath>

#pragma region Vec2
Vec2::Vec2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vec2::Vec2(int x, int y) {
	this->x = (float)x;
	this->y = (float)y;
}

Vec2::Vec2()
{
	x = 0;
	y = 0;
}

void Vec2::Normalize()
{
	float length = Length();
	x /= length;
	y /= length;
}

float Vec2::Length()
{
	return sqrt(x * x + y * y);
}

bool Vec2::FloorEqual(const Vec2& v1, const Vec2& v2)
{
	// Note: Probably don't need int cast here.
	return (int)floor(v1.x) == (int)floor(v2.x) && (int)floor(v1.y) == (int)floor(v2.y);
}

bool Vec2::CeilEqual(const Vec2& v1, const Vec2& v2)
{
	// Note: Probably don't need int cast here.
	return (int)ceil(v1.x) == (int)ceil(v2.x) && (int)ceil(v1.y) == (int)ceil(v2.y);
}

bool Vec2::RoundEqual(const Vec2& v1, const Vec2& v2)
{
	// Note: Probably don't need int cast here.
	return (int)round(v1.x) == (int)round(v2.x) && (int)round(v1.y) == (int)round(v2.y);
}

Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
{
	return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
{
	return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

Vec2 operator*(const Vec2& lhs, const float rhs)
{
	return Vec2(lhs.x*rhs, lhs.y*rhs);
}

Vec2 operator*(const float lhs, const Vec2& rhs)
{
	return rhs * lhs;
}

float operator*(const Vec2& lhs, const Vec2& rhs) {
	return lhs.x * rhs.x + lhs.y + rhs.y;
}

#pragma endregion

#pragma region Vec3
Vec3::Vec3()
{
	x = 0;
	y = 0;
	z = 0;
}
Vec3::Vec3(float x, float y, float z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
Vec3::Vec3(const Vec4& vec)
{
	x = vec.x;
	y = vec.y;
	z = vec.z;
}

float Vec3::operator*(const Vec3& rhs) {
	return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vec4 operator*(const float lhs, const Vec4& rhs)
{
	return Vec4(rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs);
}

Vec4 operator*(const Vec4& lhs, const float rhs)
{
	return rhs * lhs;
}

Vec4 operator+(const Vec4& lhs, const Vec4& rhs)
{
	return Vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

Vec4 operator-(const Vec4& lhs, const Vec4& rhs)
{
	return Vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
{
	return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}
Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
{
	return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}
Vec3 Vec3::Cross(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}
float Vec3::Length()
{
	return sqrt(x * x + y * y + z * z);
}
#pragma endregion

#pragma region Vec4
Vec4::Vec4()
{
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Vec4::Vec4(float x, float y, float z, float w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

void Vec4::Clamp01()
{
	x = std::min(1.0f, std::max(x, 0.0f));
	y = std::min(1.0f, std::max(y, 0.0f));
	z = std::min(1.0f, std::max(z, 0.0f));
	w = std::min(1.0f, std::max(w, 0.0f));
}

float Vec4::operator*(const Vec4& rhs) const {
	return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

Vec2 Vec4::XY() const
{
	return Vec2(x, y);
}

Vec4& Vec4::operator/=(float rhs)
{
	x /= rhs;
	y /= rhs;
	z /= rhs;
	w /= rhs;
	return *this;
}

std::string Vec4::ToString() const {
	std::string s;
	s += "x: " + std::to_string(x) + "\n";
	s += "y: " + std::to_string(y) + "\n";
	s += "z: " + std::to_string(z) + "\n";
	s += "w: " + std::to_string(w) + "\n";
	return s;
}
#pragma endregion

#pragma region Mat3
float Mat3::Determinant() {
	return
		m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
		m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
		m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

std::string Mat3::ToString() const {
	std::string s;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			s += std::to_string(m[i][j]) + "///";
		}
		s += "\n";
	}
	return s;
}
#pragma endregion
#pragma region Mat4
Mat4::Mat4() {
}

Mat4 Mat4::Identity() {
	Mat4 mat;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat.m[i][j] = i == j ? 1.0f : 0.0f;

	return mat;
}

Mat4 Mat4::Translation(float x, float y, float z) {
	Mat4 mat = Identity();
	mat.m[0][3] = x;
	mat.m[1][3] = y;
	mat.m[2][3] = z;
	return mat;
}

// Order is yaw, pitch, roll (y -> x -> z)
Mat4 Mat4::Rotation(float x, float y, float z) {
	return RotationZ(z) * RotationX(x) * RotationY(y);
}


Mat4 Mat4::RotationX(float rad) {
	Mat4 mat = Identity();
	mat.m[1][1] = cos(rad);
	mat.m[2][2] = cos(rad);
	mat.m[1][2] = -sin(rad);
	mat.m[2][1] = sin(rad);
	return mat;
}

Mat4 Mat4::RotationY(float rad) {
	Mat4 mat = Identity();
	mat.m[0][0] = cos(rad);
	mat.m[2][2] = cos(rad);
	mat.m[0][2] = sin(rad);
	mat.m[2][0] = -sin(rad);
	return mat;
}

Mat4 Mat4::RotationZ(float rad) {
	Mat4 mat = Identity();
	mat.m[0][0] = cos(rad);
	mat.m[1][1] = cos(rad);
	mat.m[0][1] = -sin(rad);
	mat.m[1][0] = sin(rad);
	return mat;
}

Mat4 Mat4::Scale(float x, float y, float z) {
	Mat4 mat = Identity();
	mat.m[0][0] = x;
	mat.m[1][1] = y;
	mat.m[2][2] = z;
	return mat;
}

Mat4 Mat4::Scale(float s) {
	return Scale(s, s, s);
}

Mat4 Mat4::PerspectiveProjection(float fov, float near, float far) {
	Mat4 mat = Identity();
	mat.m[0][0] = 1.0f / tan(fov / 2.0f);
	mat.m[1][1] = 1.0f / tan(fov / 2.0f);
	mat.m[2][2] = (far + near) / (far - near);
	mat.m[2][3] = -(2 * far * near) / (far - near);
	mat.m[3][2] = 1.0f; // Capture original z in w prime for perspective divide.
	mat.m[3][3] = 0.0f;
	return mat;
}

Mat4 Mat4::Viewport(int width, int height)
{
	Mat4 mat = Identity();
	mat.m[0][0] = (float)width / 2.0f;
	mat.m[0][3] = (float)width / 2.0f;
	mat.m[1][1] = -(float)height / 2.0f;
	mat.m[1][3] = (float)height / 2.0f;
	return mat;
}

// Not fast. Making copies many times over.
Mat4 Mat4::operator*(const Mat4& rhs) const {
	Mat4 t;
	for (int row = 0; row < 4; row++)
		for (int col = 0; col < 4; col++)
			t.m[row][col] = Row(row) * rhs.Column(col);

	return t;
}

Vec4 Mat4::operator*(const Vec4& rhs) const {
	return Vec4(Row(0) * rhs, Row(1) * rhs, Row(2) * rhs, Row(3) * rhs);
}

Mat4 Mat4::operator*(const float rhs) const
{
	Mat4 temp;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp.m[i][j] = m[i][j] * rhs;
		}
	}
	return temp;
}

Vec4 Mat4::Column(int col) const {
	return Vec4(m[0][col], m[1][col], m[2][col], m[3][col]);
}

Vec4 Mat4::Row(int row) const {
	return Vec4(m[row][0], m[row][1], m[row][2], m[row][3]);
}

Mat4 Mat4::Inverse() {
	Mat4 c = CoFactor();
	Mat4 a = c.Transpose();
	return a * (1.0f / Determinant());
}

Mat4 Mat4::Transpose()
{
	Mat4 mat;
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat.m[j][i] = m[i][j];

	return mat;
}



Mat4 Mat4::CoFactor()
{
	Mat4 c;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			c.m[i][j] = (float)pow(-1.0f, i + j) * Minor(i, j).Determinant();
		}
	}

	return c;
}

float Mat4::Determinant() {
	// Using Laplace Expansion in the first row (0th).
	Mat3 col0, col1, col2, col3;
	col0 = Minor(0, 0);
	col1 = Minor(0, 1);
	col2 = Minor(0, 2);
	col3 = Minor(0, 3);
	float det0 = m[0][0] * col0.Determinant();
	float det1 = m[0][1] * col1.Determinant();
	float det2 = m[0][2] * col2.Determinant();
	float det3 = m[0][3] * col3.Determinant();
	return det0 - det1 + det2 - det3;
}

#include <assert.h>
Mat3 Mat4::Minor(int row, int col) {
	// Gets rid of memory overrun warnings below.
	assert(row >= 0 && row <= 3);
	assert(col >= 0 && col <= 3);

	Mat3 minor{};
	int ai = 0, aj = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == row || j == col) continue;
			minor.m[i > row ? i - 1 : i][j > col ? j - 1 : j] = m[i][j];
		}
	}
	
	return minor;
}

std::string Mat4::ToString() const {
	std::string s;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			s += std::to_string(m[i][j]) + "///";
		}
		s += "\n";
	}
	return s;
}
#pragma endregion


