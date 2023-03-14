/**
 * @file   MeshGeometry.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Vector3.h"

#include <optional>
#include <vector>
#include <string>

namespace NCL {
	namespace Maths {
		class Vector2;
		class Vector3;
		class Vector4;
		class Matrix4;

		class Vector4i;
	}
}
namespace NCL {
	namespace Rendering {
		class RendererBase;
	}
}

using namespace NCL::Maths;
using namespace NCL::Rendering;

namespace NCL {
	enum GeometryPrimitive {
		Points,
		Lines,
		Triangles,
		TriangleFan,
		TriangleStrip,
		Patches
	};

	enum VertexAttribute {
		Positions,
		Colours,
		TextureCoords,
		Normals,
		Tangents,
		JointWeights,
		JointIndices,
		MAX_ATTRIBUTES
	};

	const std::string VertexAttributeNames[] = {
		std::string("Positions"),
		std::string("Colours"),
		std::string("Tex Coords"),
		std::string("Normals"),
		std::string("Tangents"),
		std::string("Joint Weights"),
		std::string("Joint Indices")
	};

	struct SubMesh {
		int start = 0;
		int count = 0;
		int base  = 0;
	};

	struct SubMeshPoses {
		int start;
		int count;
	};

	class MeshGeometry {
	public:
		virtual ~MeshGeometry();

		virtual void Initilize() = 0;

		virtual void Draw() = 0;
		virtual void Draw(unsigned int subLayer) = 0;

		GeometryPrimitive GetPrimitiveType() const {
			return primType;
		}

		void SetPrimitiveType(GeometryPrimitive type) {
			primType = type;
		}

		unsigned int GetVertexCount() const {
			return (unsigned int)positions.size();
		}

		unsigned int GetIndexCount()  const {
			return (unsigned int)indices.size();
		}

		unsigned int GetJointCount() const {
			return (unsigned int)jointNames.size();
		}

		unsigned int GetSubMeshCount() const {
			return (unsigned int)subMeshes.size();
		}

		const std::optional<SubMesh> GetSubMesh(unsigned int i) const {
			return i >= subMeshes.size() ? std::optional<SubMesh>() : subMeshes[i];
		}

		void AddSubMesh(int startIndex, int indexCount, int baseVertex) {
			SubMesh m;
			m.base = baseVertex;
			m.count = indexCount;
			m.start = startIndex;

			subMeshes.push_back(m);
		}

		int GetIndexForJoint(const std::string &name) const;

		const std::vector<Matrix4>& GetBindPose() const {
			return bindPose;
		}
		const std::vector<Matrix4>& GetInverseBindPose() const {
			return inverseBindPose;
		}

		void SetJointNames(std::vector < std::string > & newnames);
		void SetJointParents(std::vector<int>& newParents);
		void SetBindPose(std::vector<Matrix4>& newMats);
		void SetInverseBindPose(std::vector<Matrix4>& newMats);
		void CalculateInverseBindPose();

		bool GetTriangle(unsigned int i, Vector3& a, Vector3& b, Vector3& c) const;
		bool GetNormalForTri(unsigned int i, Vector3& n) const;
		bool HasTriangle(unsigned int i) const;

		const std::vector<Vector3>& GetPositionData()     const { return positions; }
		const std::vector<Vector2>& GetTextureCoordData() const { return texCoords; }
		const std::vector<Vector4>& GetColourData()       const { return colours;   }
		const std::vector<Vector3>& GetNormalData()       const { return normals;   }
		const std::vector<Vector4>& GetTangentData()      const { return tangents; }

		const std::vector<Vector4>& GetSkinWeightData() const { return skinWeights; }
		const std::vector<Vector4i>& GetSkinIndexData() const { return skinIndices; }

		const std::vector<int>& GetJointParents() const {
			return jointParents;
		}

		const std::vector<unsigned int>& GetIndexData() const { return indices; }

		void SetVertexPositions(const std::vector<Vector3>& newVerts);
		void SetVertexTextureCoords(const std::vector<Vector2>& newTex);

		void SetVertexColours(const std::vector<Vector4>& newColours);
		void SetVertexNormals(const std::vector<Vector3>& newNorms);
		void SetVertexTangents(const std::vector<Vector4>& newTans);
		void SetVertexIndices(const std::vector<unsigned int>& newIndices);

		void SetVertexSkinWeights(const std::vector<Vector4>& newSkinWeights);
		void SetVertexSkinIndices(const std::vector<Vector4i>& newSkinIndices);

		void TransformVertices(const Matrix4& byMatrix);

		void RecalculateNormals();
		void RecalculateTangents();

		void SetDebugName(const std::string& debugName);

		virtual void UploadToGPU() = 0;

		const std::vector<int>& GetBindPoseIndices() const {
			return bindPoseIndices;
		}
		bool GetBindPoseState(int subMesh, SubMeshPoses& pose) const {
			if (subMesh < 0 || subMesh >= (int)bindPoseStates.size()) {
				return false;
			}
			pose = bindPoseStates[subMesh];
			return true;
		}
	protected:
		MeshGeometry();
		MeshGeometry(const std::string&filename);

		void ReadRigPose(std::ifstream& file, std::vector<Matrix4>& into);
		void ReadJointParents(std::ifstream& file);
		void ReadJointNames(std::ifstream& file);
		void ReadSubMeshes(std::ifstream& file, int count);
		void ReadSubMeshNames(std::ifstream& file, int count);

		bool GetVertexIndicesForTri(size_t i, size_t& a, size_t& b, size_t& c) const;

		virtual bool ValidateMeshData();

		std::string debugName;
		GeometryPrimitive primType;
		std::vector<Vector3> positions;

		std::vector<Vector2> texCoords;
		std::vector<Vector4> colours;
		std::vector<Vector3> normals;
		std::vector<Vector4> tangents;
		std::vector<unsigned int> indices;

		std::vector<SubMesh>     subMeshes;
		std::vector<std::string> subMeshNames;

		std::vector<Vector4>  skinWeights;
		std::vector<Vector4i> skinIndices;
		std::vector<std::string> jointNames;
		std::vector<int>         jointParents;

		std::vector<Matrix4> bindPose;
		std::vector<Matrix4> inverseBindPose;

		std::vector<int>          bindPoseIndices;
		std::vector<SubMeshPoses> bindPoseStates;
	};
}