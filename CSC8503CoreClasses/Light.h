/**
 * @file   Light.h
 * @brief  
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

#include <memory>

namespace NCL::Rendering {
	class ShaderBase;
}

using namespace NCL::Rendering;
using namespace NCL::Maths;

namespace NCL::CSC8503 {
	class Light {
	public:
		virtual ~Light() = default;

		virtual void Upload(ShaderBase& shader) const = 0;

		inline const Matrix4& GetShadowMatrix() const {
			return shadowMatrix;
		}
	protected:
		Matrix4 shadowMatrix;
	};

	class PointLight : public Light {
	public:
		PointLight(const Vector3& position, const Vector4& colour, float radius);
		virtual ~PointLight() = default;

		void Upload(ShaderBase& shader) const override;

		inline void SetPosition(const Vector3& position) {
			this->position = position;
			UpdateShadowMatrix();
		}
		inline const Vector3& GetPosition() {
			return position;
		}

		inline void SetColour(const Vector4& colour) {
			this->colour = colour;
		}
		inline const Vector4& GetColour() {
			return colour;
		}

		inline void SetRadius(float radius) {
			this->radius = radius;
		}
		inline float GetRadius() {
			return radius;
		}
	private:
		inline void UpdateShadowMatrix() {
			shadowMatrix = Matrix4::Perspective(1.0f, 100.0f, 1.0f, 170.0f) *
						   Matrix4::BuildViewMatrix(position, Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f));
		}

		Vector3 position;
		Vector4 colour;
		float radius;
	};

	class SpotLight : public Light {
	public:
		SpotLight(const Vector3& position, const Vector3& direction, const Vector4& colour, float radius, float angle);
		virtual ~SpotLight() = default;

		void Upload(ShaderBase& shader) const override;

		inline void SetPosition(const Vector3& position) {
			this->position = position;
			UpdateShadowMatrix();
		}
		inline const Vector3& GetPosition() {
			return position;
		}

		inline void SetDirection(const Vector3& direction) {
			this->direction = direction;
		}
		inline const Vector3& GetDirection() {
			return direction;
		}

		inline void SetColour(const Vector4& colour) {
			this->colour = colour;
		}
		inline const Vector4& GetColour() {
			return colour;
		}

		inline void SetRadius(float radius) {
			this->radius = radius;
		}
		inline float GetRadius() {
			return radius;
		}

		inline void SetAngle(float angle) {
			this->angle = angle;
		}
		inline float GetAngle() {
			return angle;
		}
	private:
		inline void UpdateShadowMatrix() {
			shadowMatrix = Matrix4::Perspective(1.0f, 100.0f, 1.0f, 170.0f) *
						   Matrix4::BuildViewMatrix(position, Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f));
		}

		Vector3 position;
		Vector3 direction;
		Vector4 colour;
		float radius;
		float angle;
	};

	class DirectionalLight : public Light {
	public:
		DirectionalLight(const Vector3& direction, const Vector4& colour);
		virtual ~DirectionalLight() = default;

		void Upload(ShaderBase& shader) const override;

		inline void SetDirection(const Vector3& direction) {
			this->direction = direction;
			UpdateShadowMatrix();
		}
		inline const Vector3& GetDirection() {
			return direction;
		}

		inline void SetColour(const Vector4& colour) {
			this->colour = colour;
		}
		inline const Vector4& GetColour() {
			return colour;
		}
	private:
		inline void UpdateShadowMatrix() {
			shadowMatrix = Matrix4::Orthographic(-100.0f, 100.0f, -100.0f, 100.0f, -1.0f, 500.0f) *
						   Matrix4::BuildViewMatrix(direction * 250.0f, Vector3(0.0f), Vector3(0.0f, 1.0f, 0.0f));
		}

		Vector3 direction;
		Vector4 colour;
	};
}
