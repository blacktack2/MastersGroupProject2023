/**
 * @file   Matrix4.h
 * @brief  Wrapper classes for 4x4 matrix types.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include <iostream>

namespace NCL {
	namespace Maths {
		class Vector3;
		class Vector4;

		class Matrix3;
		
		class Quaternion;

		class Matrix4 {
		public:
			float array[4][4];
		public:
			constexpr Matrix4() {
				ToIdentity();
			}
			explicit constexpr Matrix4(float elements[16]) {
				array[0][0] = elements[0];
				array[0][1] = elements[1];
				array[0][2] = elements[2];
				array[0][3] = elements[3];

				array[1][0] = elements[4];
				array[1][1] = elements[5];
				array[1][2] = elements[6];
				array[1][3] = elements[7];

				array[2][0] = elements[8];
				array[2][1] = elements[9];
				array[2][2] = elements[10];
				array[2][3] = elements[11];

				array[3][0] = elements[12];
				array[3][1] = elements[13];
				array[3][2] = elements[14];
				array[3][3] = elements[15];
			}
			explicit Matrix4(const Matrix3& m3);
			explicit Matrix4(const Quaternion& quat);
			~Matrix4() = default;

			inline constexpr void ToZero() {
				for (unsigned int c = 0; c < 3; c++) {
					for (unsigned int r = 0; r < 3; r++) {
						array[r][c] = 0.0f;
					}
				}
			}
			inline constexpr void ToIdentity() {
				for (unsigned int c = 1; c < 4; c++) {
					for (unsigned int r = 0; r < c; r++) {
						array[c][r] = 0.0f;
						array[r][c] = 0.0f;
					}
				}
				for (unsigned int i = 0; i < 4; i++) {
					array[i][i] = 1.0f;
				}
			}

			Vector3 GetPositionVector() const;
			void SetPositionVector(const Vector3& in);

			Vector4 GetRow(unsigned int row) const;
			void SetRow(unsigned int row, const Vector4& val);

			Vector4 GetColumn(unsigned int column) const;
			void SetColumn(unsigned int column, const Vector4& val);

			Vector4 GetDiagonal() const;
			void SetDiagonal(const Vector4& in);

			inline constexpr Matrix4 Absolute() const {
				Matrix4 m;
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m.array[i][j] = std::abs(array[i][j]);
					}
				}
				return m;
			}

			void    Invert();
			Matrix4 Inverse() const;

			Matrix4 Transposed() const {
				Matrix4 temp = *this;
				temp.Transpose();
				return temp;
			}
			inline constexpr void Transpose() {
				for (unsigned int c = 1; c < 4; c++) {
					for (unsigned int r = 0; r < c; r++) {
						std::swap(array[c][r], array[r][c]);
					}
				}
			}

			/**
			 * @brief Creates a rotation matrix that rotates by 'degrees' around the 'axis'
			 * @brief Analogous to glRotatef.
			 */
			static Matrix4 Rotation(float degrees, const Vector3& axis);
			/**
			 * @brief Creates a scaling matrix (puts the 'scale' vector down the diagonal)
			 * @brief Analogous to glScalef.
			 */
			static Matrix4 Scale(const Vector3& scale);
			/**
			 * @brief Creates a translation matrix (identity, with 'translation'
			 * vector at floats 12, 13, and 14.
			 * @brief Analogous to glTranslatef.
			 */
			static Matrix4 Translation(const Vector3& translation);

			/**
			 * @brief Create a perspective matrix.
			 */
			static Matrix4 Perspective(float znear, float zfar, float aspect, float fov);
			/**
			 * @brief Create an orthographic matrix.
			 */
			static Matrix4 Orthographic(float left, float right, float bottom, float top, float near, float far);

			//Builds a view matrix suitable for sending straight to the vertex shader.
			//Puts the camera at 'from', with 'lookingAt' centered on the screen, with
			//'up' as the...up axis (pointing towards the top of the screen)
			/**
			 * @brief Create a view matrix.
			 * 
			 * @param from      World-space position of the camera.
			 * @param lookingAt World-space position the camera is looking at.
			 */
			static Matrix4 BuildViewMatrix(const Vector3& from, const Vector3& lookingAt, const Vector3& up);

			inline constexpr Matrix4 operator*(const Matrix4& a) const {
				Matrix4 out;
				for (unsigned int c = 0; c < 4; ++c) {
					for (unsigned int r = 0; r < 4; ++r) {
						out.array[c][r] = 0.0f;
						for (unsigned int i = 0; i < 4; ++i) {
							out.array[c][r] += this->array[i][r] * a.array[c][i];
						}
					}
				}
				return out;
			}

			Vector3 operator*(const Vector3& v) const;
			Vector4 operator*(const Vector4& v) const;

			inline friend std::ostream& operator<<(std::ostream& o, const Matrix4& m) {
				o << "Mat4(";
				o << "\t" << m.array[0][0] << "," << m.array[0][1] << "," << m.array[0][2] << "," << m.array[0][3] << "\n";
				o << "\t\t" << m.array[1][0] << "," << m.array[1][1] << "," << m.array[1][2] << "," << m.array[1][3] << "\n";
				o << "\t\t" << m.array[2][0] << "," << m.array[2][1] << "," << m.array[2][2] << "," << m.array[2][3] << "\n";
				o << "\t\t" << m.array[3][0] << "," << m.array[3][1] << "," << m.array[3][2] << "," << m.array[3][3] << " )\n";
				return o;
			}
		};
	}
}