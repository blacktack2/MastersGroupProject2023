/**
 * @file   Light.cpp
 * @brief  See Light.h
 * 
 * @author Stuart Lewis
 * @date   February 2023
 */
#include "Light.h"

#include "ShaderBase.h"

using namespace NCL::CSC8503;

PointLight::PointLight(const Vector3& position, const Vector4& colour, float radius) : Light(),
position(position), colour(colour), radius(radius) {
}

void PointLight::Upload(ShaderBase& shader) const {
	shader.SetUniformFloat("lightPosition", position.x, position.y, position.z, 1.0f);
	shader.SetUniformFloat("lightColour", colour);
	shader.SetUniformFloat("lightRadius", radius);
	shader.SetUniformFloat("lightAngle", 0.0f);
}

SpotLight::SpotLight(const Vector3& position, const Vector3& direction, const Vector4& colour, float radius, float angle) : Light(),
position(position), direction(direction), colour(colour), radius(radius), angle(angle) {
}

void SpotLight::Upload(ShaderBase& shader) const {
	shader.SetUniformFloat("lightPosition", position.x, position.y, position.z, 1.0f);
	shader.SetUniformFloat("lightColour", colour);
	shader.SetUniformFloat("lightRadius", radius);
	shader.SetUniformFloat("lightAngle", angle);
}

DirectionalLight::DirectionalLight(const Vector3& direction, const Vector4& colour) : Light(),
direction(direction), colour(colour) {
}

void DirectionalLight::Upload(ShaderBase& shader) const {
	shader.SetUniformFloat("lightPosition", Vector4(0.0f));
	shader.SetUniformFloat("lightDirection", direction);
	shader.SetUniformFloat("lightColour", colour);
}
