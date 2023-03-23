#ifdef _ORBIS
#pragma once
#include "../Plugins/PlayStation4/PS4RendererBase.h"
#include "../Plugins/PlayStation4/PS4Camera.h"
#include "../Common/GameWorld.h"
#include "../Common/MeshGeometry.h"
#include "../Common/ShaderBase.h"
#include "../Common/TextureBase.h"

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

			void CreatePaintTexture(unsigned int width, unsigned int height, CSC8503::RenderObject* object);

			void DrawRenderObject(const CSC8503::RenderObject* object);

			void Update(float dt) override;

			void SetDefaultShader(PS4Shader* s) { defaultShader = s; }
			//void SetPaintShader(PS4Shader* s) { paintShader = s; }

		protected:

			struct PaintData {
				float position[3];
				float colour[3];
				float size;
			};

			GameWorld& gameWorld;

			vector<const CSC8503::RenderObject*>activeObjects;

			

			PS4Shader* defaultShader;
			//PS4Shader* paintShader;

			//PS4Texture* defaultPaintTex;

			NCL::Maths::Matrix4* viewProjMat;
			Gnm::Buffer	cameraBuffer;
			
			sce::Gnm::RenderTarget paintTarget;
		};
	}
}
#endif //_ORBIS