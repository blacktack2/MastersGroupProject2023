/**
 * @file   Matrix2.h
 * @brief  Wrapper classes for 2x2 matrix types.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include <iostream>

namespace NCL {
	namespace Maths {
		class Vector2;

		class Matrix2 {
		public:
			float array[2][2];
		public:
			constexpr Matrix2() {
				ToIdentity();
			}
			explicit constexpr Matrix2(float elements[4]) {
				array[0][0] = elements[0];
				array[0][1] = elements[1];

				array[1][0] = elements[2];
				array[1][1] = elements[3];
			}

			~Matrix2() = default;

			inline constexpr void ToZero() {
				for (int i = 0; i < 2; ++i) {
					for (int j = 0; j < 2; ++j) {
						array[i][j] = 0.0f;
					}
				}
			}
			inline constexpr void ToIdentity() {
				array[0][0] = 1.0f;
				array[0][1] = 0.0f;
				array[1][0] = 0.0f;
				array[1][1] = 1.0f;
			}

			Matrix2& SetRow(unsigned int row, const Vector2& val);
			Vector2 GetRow(unsigned int row) const;

			Matrix2& SetColumn(unsigned int column, const Vector2& val);
			Vector2 GetColumn(unsigned int column) const;

			void SetDiagonal(const Vector2& in);
			Vector2 GetDiagonal() const;

			inline constexpr Matrix2 Absolute() const {
				Matrix2 m;
				for (int i = 0; i < 2; ++i) {
					for (int j = 0; j < 2; ++j) {
						m.array[i][j] = std::abs(array[i][j]);
					}
				}
				return m;
			}

			inline constexpr Matrix2 Transposed() const {
				Matrix2 temp = *this;
				temp.Transpose();
				return temp;
			}

			inline constexpr void Transpose() {
				std::swap(array[0][1], array[1][0]);
			}

			static Matrix2 Rotation(float degrees);

			Vector2 operator*(const Vector2& v) const;

			inline constexpr Matrix2 operator*(const Matrix2& a) const {
				Matrix2 out;
				for (unsigned int c = 0; c < 2; ++c) {
					for (unsigned int r = 0; r < 2; ++r) {
						out.array[c][r] = 0.0f;
						for (unsigned int i = 0; i < 2; ++i) {
							out.array[c][r] += this->array[i][r] * a.array[c][i];
						}
					}
				}
				return out;
			}

			inline friend std::ostream& operator<<(std::ostream& o, const Matrix2& m) {
				o << "Mat2(";
				o << "\t" << m.array[0][0] << "," << m.array[0][1] << "\n";
				o << "\t\t" << m.array[1][0] << "," << m.array[1][1] << "\n";
				return o;
			}
		};
	}
}
