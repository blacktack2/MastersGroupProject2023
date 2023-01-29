#include "TestGameObject.h"

#include "GameObject.h"
#include <glm/gtx/string_cast.hpp>
#include <iostream>

using namespace paintHell::core;

namespace {

	void DisplayMat4(glm::mat4 matrix) {
		for (int col = 0; col < 4; ++col) {
			std::cout << "| ";
			for (int row = 0; row < 4; ++row) {
				std::cout << matrix[row][col] << '\t';
			}
			std::cout << '\n';
		}
	}

	void TestTransformTranslate(Transform& transform) {
		std::cout << "Testing Translation: \n";
		transform.SetParent(nullptr);
		transform.SetRotation(glm::quat(0, 0, 0, 0));
		transform.SetScale(glm::vec3(1, 1, 1));
		transform.SetLocalPosition(glm::vec3(2, 5, 2));

		std::cout << "Test 1/2\n\tExpexted: " << glm::to_string<glm::vec4>(glm::vec4(2, 5, 2, 1));
		std::cout << "\n\tReceived: " << glm::to_string<glm::vec4>(transform.GetLocalPosition());
		std::cout << "\n";
		if (glm::vec4(2, 5, 2, 1) == transform.GetLocalPosition()) std::cout << "Passed\n";
		else {
			std::cout << " Failed\n";
			return;
		}
		std::cout << "Test2/2\n\tExpexted:\n";
		glm::mat4 expexcted = glm::mat4(glm::vec4(1, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(2, 5, 2, 1));
		DisplayMat4(expexcted);
		std::cout << "\tRecieved:\n";
		DisplayMat4(transform.GetLocalMatrix());
		if (expexcted == transform.GetLocalMatrix()) std::cout << "Passed\n";
		else {
			std::cout << " Failed\n";
			return;
		}
	}

	void TestTransformScale(Transform& transform) {
		std::cout << "Testing Scale: \n";
		transform.SetParent(nullptr);
		transform.SetRotation(glm::quat(0, 0, 0, 0));
		transform.SetScale(glm::vec3(2, 4, 3));
		transform.SetLocalPosition(glm::vec3(0, 0, 0));

		std::cout << "Test 1/2\n\tExpexted: " << glm::to_string<glm::vec3>(glm::vec3(2, 4, 3));
		std::cout << "\n\tReceived: " << glm::to_string<glm::vec3>(transform.GetScale());
		std::cout << "\n";
		if (glm::vec3(2, 4, 3) == transform.GetScale()) std::cout << "Passed\n";
		else {
			std::cout << " Failed\n";
			return;
		}
		std::cout << "Test2/2\n\tExpexted:\n";
		glm::mat4 expexcted = glm::mat4(glm::vec4(2, 0, 0, 0), glm::vec4(0, 4, 0, 0), glm::vec4(0, 0, 3, 0), glm::vec4(0, 0, 0, 1));
		DisplayMat4(expexcted);
		std::cout << "\tRecieved:\n";
		DisplayMat4(transform.GetLocalMatrix());
		if (expexcted == transform.GetLocalMatrix()) std::cout << "Passed\n";
		else {
			std::cout << " Failed\n";
			return;
		}
	}

	void TestTransformQuaternion(Transform& transform) {
		std::cout << "Testing Rotation: \n";
		transform.SetParent(nullptr);
		transform.SetRotation(glm::quat(0.5, 0.5, 0.5, 0.5)); //is equal to 120 degree rotation around 1,1,1,1
		transform.SetScale(glm::vec3(1, 1, 1));
		transform.SetLocalPosition(glm::vec3(0, 0, 0));

		std::cout << "Test 1/2\n\tExpexted: " << glm::to_string<glm::quat>(glm::quat(0.5, 0.5, 0.5, 0.5));
		std::cout << "\n\tReceived: " << glm::to_string<glm::quat>(transform.GetRotation());
		std::cout << "\n";
		if (glm::quat(0.5, 0.5, 0.5, 0.5) == transform.GetRotation()) std::cout << "Passed\n";
		else {
			std::cout << " Failed\n";
			return;
		}
		std::cout << "Test2/2\n\tExpexted:\n";
		glm::mat4 expexcted = glm::mat4(glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 1, 0), glm::vec4(1, 0, 0, 0), glm::vec4(0, 0, 0, 1));
		DisplayMat4(expexcted);
		std::cout << "\tRecieved:\n";
		DisplayMat4(transform.GetLocalMatrix());
		if (expexcted == transform.GetLocalMatrix()) std::cout << "Passed\n";
		else {
			std::cout << " Failed\n";
			return;
		}
	}
	void TestTransformCombine(Transform& transform) {
		transform.SetParent(nullptr);
		transform.SetRotation(glm::quat(0.5, 0.5, 0.5, 0.5)); //is equal to 120 degree rotation around 1,1,1,1
		transform.SetScale(glm::vec3(2, 4, 3));
		transform.SetLocalPosition(glm::vec3(2, 5, 8));

		std::cout << "Test1/1\n\tExpexted:\n";
		glm::mat4 expexcted = glm::mat4(glm::vec4(0, 2, 0, 0), glm::vec4(0, 0, 4, 0), glm::vec4(3, 0, 0, 0), glm::vec4(2, 5, 8, 1));
		DisplayMat4(expexcted);
		std::cout << "\tRecieved:\n";
		DisplayMat4(transform.GetLocalMatrix());
		if (expexcted == transform.GetLocalMatrix()) std::cout << "Passed\n";
		else {
			std::cout << " Failed\n";
			return;
		}
	}
}

void paintHell::test::core::TestGameObjectTransform() {
	GameObject objA;
	TestTransformTranslate(objA.GetTransform());
	TestTransformScale(objA.GetTransform());
	TestTransformQuaternion(objA.GetTransform());
	TestTransformCombine(objA.GetTransform());
}