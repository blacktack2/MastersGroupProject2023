/**
 * @file   Matrix3.h
 * @brief  Wrapper classes for 3x3 matrix types.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include <iostream>

namespace NCL {
	namespace Maths {
		class Matrix2;
		class Matrix4;
		
		class Vector3;

		class Quaternion;

		class Matrix3 {
		public:
			float array[3][3];
		public:
			constexpr Matrix3() {
				ToIdentity();
			}
			explicit constexpr Matrix3(float elements[9]) {
				array[0][0] = elements[0];
				array[0][1] = elements[1];
				array[0][2] = elements[2];

				array[1][0] = elements[3];
				array[1][1] = elements[4];
				array[1][2] = elements[5];

				array[2][0] = elements[6];
				array[2][1] = elements[7];
				array[2][2] = elements[8];
			}
			explicit Matrix3(const Matrix2& m4);
			explicit Matrix3(const Matrix4& m4);
			explicit Matrix3(const Quaternion& quat);

			~Matrix3() = default;

			inline constexpr void ToZero() {
				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						array[i][j] = 0.0f;
					}
				}
			}
			inline constexpr void ToIdentity() {
				for (unsigned int c = 1; c < 3; c++) {
					for (unsigned int r = 0; r < c; r++) {
						array[c][r] = 0.0f;
						array[r][c] = 0.0f;
					}
				}
				for (unsigned int i = 0; i < 3; i++) {
					array[i][i] = 1.0f;
				}
			}

			Vector3 GetRow(unsigned int row) const;
			Matrix3& SetRow(unsigned int row, const Vector3& val);

			Vector3 GetColumn(unsigned int column) const;
			Matrix3& SetColumn(unsigned int column, const Vector3& val);

			Vector3 GetDiagonal() const;
			void SetDiagonal(const Vector3& in);

			inline constexpr Matrix3 Absolute() const {
				Matrix3 m;
				for (int i = 0; i < 3; ++i) {
					for (int j = 0; j < 3; ++j) {
						m.array[i][j] = std::abs(array[i][j]);
					}
				}
				return m;
			}

			inline constexpr Matrix3 Transposed() const {
				Matrix3 temp = *this;
				temp.Transpose();
				return temp;
			}

			inline constexpr void Transpose() {
				for (unsigned int c = 1; c < 3; c++) {
					for (unsigned int r = 0; r < c; r++) {
						std::swap(array[c][r], array[r][c]);
					}
				}
			}

			/**
			 * @brief Creates a rotation matrix that rotates by 'degrees' around the 'axis'.
			 * @brief Analogous to glRotatef.
			 */
			static Matrix3 Rotation(float degrees, const Vector3& axis);
			/**
			 * @brief Creates a scaling matrix (puts the 'scale' vector down the diagonal).
			 * @brief Analogous to glScalef.
			 */
			static Matrix3 Scale(const Vector3& scale);

			Vector3 ToEuler() const;
			static Matrix3 FromEuler(const Vector3& euler);

			Vector3 operator*(const Vector3& v) const;

			inline constexpr Matrix3 operator*(const Matrix3& a) const {
				Matrix3 out;
				for (unsigned int c = 0; c < 3; ++c) {
					for (unsigned int r = 0; r < 3; ++r) {
						out.array[c][r] = 0.0f;
						for (unsigned int i = 0; i < 3; ++i) {
							out.array[c][r] += this->array[i][r] * a.array[c][i];
						}
					}
				}
				return out;
			}

			inline friend std::ostream& operator<<(std::ostream& o, const Matrix3& m) {
				o << m.array[0][0] << "," << m.array[0][1] << "," << m.array[0][2] << "\n";
				o << m.array[1][0] << "," << m.array[1][1] << "," << m.array[1][2] << "\n";
				o << m.array[2][0] << "," << m.array[2][1] << "," << m.array[2][2] << "\n";
				return o;
			}

			inline friend std::istream& operator>>(std::istream& i, Matrix3& m) {
				char ignore = ' ';
				i >> std::skipws;
				i >> m.array[0][0] >> ignore >> m.array[0][1] >> ignore >> m.array[0][2];
				i >> m.array[1][0] >> ignore >> m.array[1][1] >> ignore >> m.array[1][2];
				i >> m.array[2][0] >> ignore >> m.array[2][1] >> ignore >> m.array[2][2];

				return i;
			}
		};
	}
}