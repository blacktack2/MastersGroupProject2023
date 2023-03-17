/**
 * @file   Matrix2.cpp
 * @brief  See Matrix2.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "Matrix2.h"

#include "Maths.h"
#include "Vector2.h"

#include <cassert>

using namespace NCL::Maths;

Matrix2& Matrix2::SetRow(unsigned int row, const Vector2& val) {
	assert(row < 2);
	array[0][row] = val.x;
	array[1][row] = val.y;
	return *this;
}

Vector2 Matrix2::GetRow(unsigned int row) const {
	assert(row < 2);
	return Vector2(array[0][row], array[1][row]);
}

Matrix2& Matrix2::SetColumn(unsigned int column, const Vector2& val) {
	assert(column < 2);
	array[column][0] = val.x;
	array[column][1] = val.y;
	return *this;
}

Vector2 Matrix2::GetColumn(unsigned int column) const {
	assert(column < 2);
	return Vector2(array[column][0], array[column][1]);
}

void Matrix2::SetDiagonal(const Vector2& in) {
	array[0][0] = in.x;
	array[1][1] = in.y;
}

Vector2 Matrix2::GetDiagonal() const {
	return Vector2(array[0][0], array[1][1]);
}

Vector2 Matrix2::operator*(const Vector2& v) const {
	Vector2 vec;

	vec.x = v.x * array[0][0] + v.y * array[1][0];
	vec.y = v.x * array[0][1] + v.y * array[1][1];

	return vec;
}

Matrix2 Matrix2::Rotation(float degrees) {
	Matrix2 mat;

	float radians = Maths::DegreesToRadians(degrees);
	float s = sin(radians);
	float c = cos(radians);

	mat.array[0][0] = c;
	mat.array[0][1] = s;
	mat.array[1][0] = -s;
	mat.array[1][1] = c;

	return mat;
}
