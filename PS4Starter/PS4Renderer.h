#pragma once
#include "../Plugins/PlayStation4/PS4RendererBase.h"
#include "../Plugins/PlayStation4/PS4Camera.h"
#include "../Common/GameWorld.h"
#include "../Common/MeshGeometry.h"
#include "../Common/ShaderBase.h"

namespace NCL {
	namespace PS4 {
		class PS4Renderer : public PS4RendererBase {
		public:
			PS4Renderer(GameWorld& world);
			~PS4Renderer() {};

			MeshGeometry* LoadMesh(const std::string& name);
			TextureBase* LoadTexture(const std::string& name);
			ShaderBase* LoadShader(const std::string& vertex, const std::string& fragment);

			void BuildObjectList();

			void RenderFrame() override;

			void UpdateViewProjectionMatrix(PS4Camera* camera);

			void DrawRenderObject(const CSC8503::RenderObject* o);

			void Update(float dt) override;

			void SetDefaultShader(PS4Shader* s) { defaultShader = s; }

		protected:
			GameWorld& gameWorld;

			vector<const CSC8503::RenderObject*>activeObjects;

			

			PS4Shader* defaultShader;
			NCL::Maths::Matrix4* viewProjMat;
			Gnm::Buffer	cameraBuffer;
			
		};
	}
}