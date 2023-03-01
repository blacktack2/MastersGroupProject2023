#include "Billboard.h"

#include "Maths.h"

Billboard::Billboard(Vector3 position) {
	this->position = position;
	modelTransform = Matrix4();

	quad = new OGLMesh();
	quad->SetVertexPositions({
		Vector3(-0.1, 0, -1),
		Vector3(-0.1, -0.1, -1),
		Vector3(0.1, -0.1, -1),
		Vector3(0.1, 0, -1),
		});
	quad->SetVertexTextureCoords({
		Vector2(0, 1),
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		});
	quad->SetVertexIndices({ 0, 1, 2, 2, 3, 0 });
	quad->SetVertexColours({
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		Vector4(1, 1, 1, 1),
		});
	quad->UploadToGPU();

	billboardTexture = (OGLTexture*)OGLTexture::RGBATextureFromFilename("defaultpause.jpg");

	billboardShader = new OGLShader("menuVertex.vert", "menuFragment.frag");

	billboardShader->Bind();

	billboardShader->SetUniformInt("difuseTex", 0);

	billboardShader->Unbind();
}

void Billboard::update(Vector3 playerPosition) {

	Vector3 directionFromPlayer = position - playerPosition;
	float theta{ atan2f(directionFromPlayer.y, directionFromPlayer.x) };
	float distance2D{ sqrt(
		directionFromPlayer.x * directionFromPlayer.x + directionFromPlayer.y * directionFromPlayer.y
	) };
	float phi{ atan2f(-directionFromPlayer.z, distance2D) };

	modelTransform = Matrix4();
	modelTransform = Matrix4::Translation(position);
	modelTransform = modelTransform
		* Quaternion::EulerAnglesToQuaternion(0.0f, 0.0f, theta)
		* Quaternion::EulerAnglesToQuaternion(0.0f, phi, 0.0f);
}