#pragma once
#include "../Plugins/PlayStation4/PS4RendererBase.h"
#include "../Plugins/PlayStation4/PS4Camera.h"

namespace NCL {
	namespace PS4 {
		class ExampleRenderer :
			public PS4RendererBase
		{
		public:
			ExampleRenderer(PS4Window* window);
			~ExampleRenderer();

			void Update(float dt)	 override;
			void CreateCamera(PS4Input* i) { camera = new PS4Camera(i); };

		protected:
			void	RenderFrame()	override;

			void DrawRenderObject(RenderObject* o);

			float rotation;
			float translation;
			RenderObject* defaultObject[2];
			PS4Camera* camera;

			//default data
			PS4Shader*	defaultShader;
			PS4Mesh*	defaultMesh;
			PS4Texture* defaultTexture;

			//		
			NCL::Maths::Matrix4*	viewProjMat;
			Gnm::Buffer	cameraBuffer;
		};
	}
}

