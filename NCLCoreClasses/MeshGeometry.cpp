/**
 * @file   MeshGeometry.cpp
 * @brief  See MeshGeometry.h.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#include "MeshGeometry.h"

#include "Assets.h"

#include "Maths.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <fstream>
#include <string>

using namespace NCL;
using namespace NCL::Maths;

enum class GeometryChunkTypes {
	VPositions = 1 << 0,
	VNormals = 1 << 1,
	VTangents = 1 << 2,
	VColors = 1 << 3,
	VTex0 = 1 << 4,
	VTex1 = 1 << 5,
	VWeightValues = 1 << 6,
	VWeightIndices = 1 << 7,
	Indices = 1 << 8,
	JointNames = 1 << 9,
	JointParents = 1 << 10,
	BindPose = 1 << 11,
	BindPoseInv = 1 << 12,
	Material = 1 << 13,
	SubMeshes = 1 << 14,
	SubMeshNames = 1 << 15,
	BindPoseIndices = 1 << 16,
	BindPoseStates = 1 << 17,
};

void ReadTextInts(std::ifstream& file, std::vector<Vector2i>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector2i temp{};
		file >> temp.x;
		file >> temp.y;
		element.emplace_back(temp);
	}
}

void ReadTeReadTextIntsxtFloats(std::ifstream& file, std::vector<Vector3i>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector3i temp{};
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		element.emplace_back(temp);
	}
}

void ReadTextInts(std::ifstream& file, std::vector<Vector4i>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector4i temp{};
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		file >> temp.w;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, std::vector<Vector2>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector2 temp{};
		file >> temp.x;
		file >> temp.y;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, std::vector<Vector3>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector3 temp{};
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		element.emplace_back(temp);
	}
}

void ReadTextFloats(std::ifstream& file, std::vector<Vector4>& element, int numVertices) {
	for (int i = 0; i < numVertices; ++i) {
		Vector4 temp{};
		file >> temp.x;
		file >> temp.y;
		file >> temp.z;
		file >> temp.w;
		element.emplace_back(temp);
	}
}

void ReadIndices(std::ifstream& file, std::vector<unsigned int>& elements, int numIndices) {
	for (int i = 0; i < numIndices; ++i) {
		unsigned int temp;
		file >> temp;
		elements.emplace_back(temp);
	}
}

void ReadIntegerArray(std::ifstream& file, std::vector<int>& into) {
	int count = 0;
	file >> count;
	for (int i = 0; i < count; ++i) {
		int r = 0;
		file >> r;
		into.push_back(r);
	}
}

void ReadBindposes(std::ifstream& file, std::vector<SubMeshPoses>& bindPoses) {
	int poseCount = 0;
	file >> poseCount;

	for (int i = 0; i < poseCount; ++i) {
		SubMeshPoses m{};
		file >> m.start;
		file >> m.count;
		bindPoses.emplace_back(m);
	}
}

MeshGeometry::MeshGeometry() {
	primType = GeometryPrimitive::Triangles;
}

MeshGeometry::MeshGeometry(const std::string& filename) {
	primType = GeometryPrimitive::Triangles;
	std::ifstream file(Assets::MESHDIR + filename);

	std::string filetype;
	file >> filetype;

	if (filetype != "MeshGeometry") {
		std::cout << __FUNCTION__ << " File is not a MeshGeometry file!"<<std::endl;
		return;
	}

	int fileVersion;
	file >> fileVersion;

	if (fileVersion != 1) {
		std::cout << __FUNCTION__ << " MeshGeometry file has incompatible version!" << std::endl;
		return;
	}

	int numMeshes   = 0;
	int numVertices = 0;
	int numIndices  = 0;
	int numChunks   = 0;

	file >> numMeshes;
	file >> numVertices;
	file >> numIndices;
	file >> numChunks;

	for (int i = 0; i < numChunks; ++i) {
		int chunkType = (int)GeometryChunkTypes::VPositions;

		file >> chunkType;

		switch ((GeometryChunkTypes)chunkType) {
			case GeometryChunkTypes::VPositions :ReadTextFloats(file, positions, numVertices) ; break;
			case GeometryChunkTypes::VColors    : ReadTextFloats(file, colours, numVertices)  ; break;
			case GeometryChunkTypes::VNormals   : ReadTextFloats(file, normals, numVertices)  ; break;
			case GeometryChunkTypes::VTangents  : ReadTextFloats(file, tangents, numVertices) ; break;
			case GeometryChunkTypes::VTex0      : ReadTextFloats(file, texCoords, numVertices); break;
			case GeometryChunkTypes::Indices    : ReadIndices(file, indices, numIndices)      ; break;

			case GeometryChunkTypes::VWeightValues  : ReadTextFloats(file, skinWeights, numVertices); break;
			case GeometryChunkTypes::VWeightIndices : ReadTextInts(file, skinIndices, numVertices)  ; break;
			case GeometryChunkTypes::JointNames     : ReadJointNames(file)                          ; break;
			case GeometryChunkTypes::JointParents   : ReadJointParents(file)                        ; break;
			case GeometryChunkTypes::BindPose       : ReadRigPose(file, bindPose)                   ; break;
			case GeometryChunkTypes::BindPoseInv    : ReadRigPose(file, inverseBindPose)            ; break;
			case GeometryChunkTypes::SubMeshes      : ReadSubMeshes(file, numMeshes)                ; break;
			case GeometryChunkTypes::SubMeshNames   : ReadSubMeshNames(file, numMeshes)             ; break;

			case GeometryChunkTypes::BindPoseIndices : ReadIntegerArray(file, bindPoseIndices); break;
			case GeometryChunkTypes::BindPoseStates  : ReadBindposes(file, bindPoseStates)    ; break;

		}
	}
}

enum class GeometryChunkData {
	dFloat,
	dShort,
	dByte,
};

bool MeshGeometry::HasTriangle(unsigned int i) const {
	int triCount = 0;
	if (GetIndexCount() > 0) {
		triCount = GetIndexCount() / 3;
	} else {
		triCount = GetVertexCount() / 3;
	}
	return i < (unsigned int)triCount;
}


bool MeshGeometry::GetVertexIndicesForTri(size_t i, size_t& a, size_t& b, size_t& c) const {
	if (!HasTriangle(i)) {
		return false;
	}
	if (GetIndexCount() > 0) {
		a = indices[(i * 3)];
		b = indices[(i * 3) + 1];
		c = indices[(i * 3) + 2];
	} else {
		a = (i * 3);
		b = (i * 3) + 1;
		c = (i * 3) + 2;
	}
	return true;
}

bool MeshGeometry::GetTriangle(unsigned int i, Vector3& va, Vector3& vb, Vector3& vc) const {
	size_t a, b, c;
	if (!GetVertexIndicesForTri(i, a, b, c)) {
		return false;
	}

	va = positions[a];
	vb = positions[b];
	vc = positions[c];

	return true;
}


bool MeshGeometry::GetNormalForTri(unsigned int i, Vector3& n) const {
	Vector3 a, b, c;
	if (!GetTriangle(i, a, b, c)) {
		return false;
	}

	Vector3 ba = b - a;
	Vector3 ca = c - a;

	n = Vector3::Cross(ba, ca);
	n.Normalise();

	return true;
}

int MeshGeometry::GetIndexForJoint(const std::string& name) const {
	for (int i = 0; i < jointNames.size(); ++i) {
		if (jointNames[i] == name) {
			return i;
		}
	}
	return -1;
}

void MeshGeometry::CalculateInverseBindPose() {
	inverseBindPose.resize(bindPose.size());

	for (int i = 0; i < bindPose.size(); ++i) {
		inverseBindPose[i] = bindPose[i].Inverse();
	}
}

void MeshGeometry::ReadRigPose(std::ifstream& file, std::vector<Matrix4>& into) {
	int matCount;
	file >> matCount;

	for (int m = 0; m < matCount; ++m) {
		Matrix4 mat{};
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				file >> mat.array[i][j];
			}
		}
		into.emplace_back(mat);
	}
}

void MeshGeometry::ReadJointParents(std::ifstream& file) {
	int jointCount;
	file >> jointCount;

	for (int i = 0; i < jointCount; ++i) {
		int id;
		file >> id;
		jointParents.emplace_back(id);
	}
}

void MeshGeometry::ReadJointNames(std::ifstream& file) {
	int jointCount;
	file >> jointCount;
	std::string jointName;
	std::getline(file, jointName);

	for (int i = 0; i < jointCount; ++i) {
		std::string jointName;
		std::getline(file, jointName);
		jointNames.emplace_back(jointName);
	}
}

void MeshGeometry::ReadSubMeshes(std::ifstream& file, int count) {
	for (int i = 0; i < count; ++i) {
		SubMesh m{};
		file >> m.start;
		file >> m.count;
		subMeshes.emplace_back(m);
	}
}

void MeshGeometry::ReadSubMeshNames(std::ifstream& file, int count) {
	std::string scrap;
	std::getline(file, scrap);

	for (int i = 0; i < count; ++i) {
		std::string meshName;
		std::getline(file, meshName);
		subMeshNames.emplace_back(meshName);
	}
}

bool MeshGeometry::ValidateMeshData() {
	if (GetPositionData().empty()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " does not have any vertex positions!" << std::endl;
		return false;
	}
	if (!GetTextureCoordData().empty() && GetTextureCoordData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect texture coordinate attribute count!" << std::endl;
		return false;
	}
	if (!GetColourData().empty() && GetColourData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect colour attribute count!" << std::endl;
		return false;
	}
	if (!GetNormalData().empty() && GetNormalData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect normal attribute count!" << std::endl;
		return false;
	}
	if (!GetTangentData().empty() && GetTangentData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect tangent attribute count!" << std::endl;
		return false;
	}

	if (!GetSkinWeightData().empty() && GetSkinWeightData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect skin weight attribute count!" << std::endl;
		return false;
	}
	if (!GetSkinIndexData().empty() && GetSkinIndexData().size() != GetVertexCount()) {
		std::cout << __FUNCTION__ << " mesh " << debugName << " has an incorrect skin index attribute count!" << std::endl;
		return false;
	}
	return true;
}
