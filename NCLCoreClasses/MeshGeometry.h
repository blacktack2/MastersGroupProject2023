/**
 * @file   MeshGeometry.h
 * @brief  Utility class for encapsulating general mesh geometry.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   March 2023
 */
#pragma once
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#include <optional>
#include <vector>
#include <string>

using namespace NCL::Maths;

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

	/**
	 * @brief Utility class for encapsulating general mesh geometry.
	 */
	class MeshGeometry {
	public:
		virtual ~MeshGeometry() = default;

		/**
		 * @brief Generate and initialize the mesh data.
		 */
		virtual void Initilize() = 0;

		virtual void Draw() = 0;
		virtual void Draw(unsigned int subLayer) = 0;

		inline GeometryPrimitive GetPrimitiveType() const {
			return primType;
		}
		inline void SetPrimitiveType(GeometryPrimitive type) {
			primType = type;
		}

		inline unsigned int GetVertexCount() const {
			return (unsigned int)positions.size();
		}

		inline unsigned int GetIndexCount()  const {
			return (unsigned int)indices.size();
		}

		inline unsigned int GetJointCount() const {
			return (unsigned int)jointNames.size();
		}

		inline unsigned int GetSubMeshCount() const {
			return (unsigned int)subMeshes.size();
		}

		inline const std::optional<SubMesh> GetSubMesh(unsigned int i) const {
			return i >= subMeshes.size() ? std::optional<SubMesh>() : subMeshes[i];
		}

		inline void AddSubMesh(int startIndex, int indexCount, int baseVertex) {
			subMeshes.emplace_back(SubMesh{baseVertex, indexCount, startIndex});
		}

		int GetIndexForJoint(const std::string &name) const;

		inline const std::vector<Matrix4>& GetBindPose() const {
			return bindPose;
		}
		inline const std::vector<Matrix4>& GetInverseBindPose() const {
			return inverseBindPose;
		}

		void SetJointNames(std::vector<std::string>& jointNames) {
			this->jointNames = jointNames;
		}
		void SetJointParents(std::vector<int>& jointParents) {
			this->jointParents = jointParents;
		}
		void SetBindPose(std::vector<Matrix4>& bindPose) {
			this->bindPose = bindPose;
		}
		void SetInverseBindPose(std::vector<Matrix4>& inverseBindPose) {
			this->inverseBindPose = inverseBindPose;
		}
		void CalculateInverseBindPose();

		bool GetTriangle(unsigned int i, Vector3& a, Vector3& b, Vector3& c) const;
		bool GetNormalForTri(unsigned int i, Vector3& n) const;
		bool HasTriangle(unsigned int i) const;

		inline const std::vector<Vector3>& GetPositionData() const {
			return positions;
		}
		inline const std::vector<Vector2>& GetTextureCoordData() const {
			return texCoords;
		}
		inline const std::vector<Vector4>& GetColourData() const {
			return colours;
		}
		inline const std::vector<Vector3>& GetNormalData() const {
			return normals;
		}
		inline const std::vector<Vector4>& GetTangentData() const {
			return tangents;
		}

		inline const std::vector<Vector4>& GetSkinWeightData() const {
			return skinWeights;
		}
		inline const std::vector<Vector4i>& GetSkinIndexData() const {
			return skinIndices;
		}

		inline const std::vector<int>& GetJointParents() const {
			return jointParents;
		}

		const std::vector<unsigned int>& GetIndexData() const { return indices; }

		inline void SetVertexPositions(const std::vector<Vector3>& newVerts) {
			positions = newVerts;
		}
		inline void SetVertexTextureCoords(const std::vector<Vector2>& newTex) {
			texCoords = newTex;
		}
		inline void SetVertexColours(const std::vector<Vector4>& newColours) {
			colours = newColours;
		}
		inline void SetVertexNormals(const std::vector<Vector3>& newNorms) {
			normals = newNorms;
		}
		inline void SetVertexTangents(const std::vector<Vector4>& newTans) {
			tangents = newTans;
		}
		inline void SetVertexIndices(const std::vector<unsigned int>& newIndices) {
			indices = newIndices;
		}

		inline void SetVertexSkinWeights(const std::vector<Vector4>& newSkinWeights) {
			skinWeights = newSkinWeights;
		}
		inline void SetVertexSkinIndices(const std::vector<Vector4i>& newSkinIndices) {
			skinIndices = newSkinIndices;
		}

		inline void SetDebugName(const std::string& debugName) {
			this->debugName = debugName;
		}

		virtual void UploadToGPU() = 0;

		inline const std::vector<int>& GetBindPoseIndices() const {
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
	private:
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

		std::string debugName;
	};
}
