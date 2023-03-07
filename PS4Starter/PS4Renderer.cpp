#ifdef _ORBIS
#include "PS4Renderer.h"
#include "../Plugins/PlayStation4/PS4Mesh.h"
#include "../Plugins/PlayStation4/PS4Shader.h"
#include "../Plugins/PlayStation4/PS4Frame.h"
#include "../Common/RenderObject.h"

using namespace NCL;
using namespace Rendering;
using namespace PS4;


PS4Renderer::PS4Renderer(GameWorld& world) : PS4RendererBase((PS4Window*)Window::GetWindow()), gameWorld(world) {

	viewProjMat = (Matrix4*)onionAllocator->allocate(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	*viewProjMat = Matrix4();


	cameraBuffer.initAsConstantBuffer(viewProjMat, sizeof(Matrix4));
	cameraBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK
}



MeshGeometry* NCL::PS4::PS4Renderer::LoadMesh(const std::string& name)
{
	PS4Mesh* mesh = new PS4Mesh(name);
	mesh->UploadToGPU();
	return mesh;
}

TextureBase* NCL::PS4::PS4Renderer::LoadTexture(const std::string& name)
{
	return PS4Texture::LoadTextureFromFile(name);
}

ShaderBase* NCL::PS4::PS4Renderer::LoadShader(const std::string& vertex, const std::string& fragment)
{
	return PS4Shader::GenerateShader(
		vertex,
		fragment
	);
}

void NCL::PS4::PS4Renderer::BuildObjectList()
{
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](GameObject* o) {
			if (o->IsActive()) {
				const CSC8503::RenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);
}


void NCL::PS4::PS4Renderer::RenderFrame()
{
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	SetRenderBuffer(currentPS4Buffer, true, true, true);

	

	//Primitive Setup State
	Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCcw);
	//primitiveSetup.setPolygonMode()
	currentGFXContext->setPrimitiveSetup(primitiveSetup);

	////Screen Access State
	Gnm::DepthStencilControl dsc;
	dsc.init();
	dsc.setDepthControl(Gnm::kDepthControlZWriteEnable, Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(true);
	currentGFXContext->setDepthStencilControl(dsc);

	Gnm::Sampler trilinearSampler;
	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(Gnm::kMipFilterModeLinear);

	

	for (auto& i : activeObjects) {
		if(!(i->GetShader()))
			defaultShader->SubmitShaderSwitch(*currentGFXContext);
		
		currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &((PS4Texture*)(*i).GetDefaultTexture())->GetAPITexture());
		currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
		DrawRenderObject(i);
	}
	

	currentFrame->EndFrame();
}

void NCL::PS4::PS4Renderer::UpdateViewProjectionMatrix(PS4Camera* camera)
{
	/**viewProjMat = Matrix4();*/
	//*viewProjMat = Matrix4::Perspective(1.0f, 1000.0f, (float)currentWidth / (float)currentHeight, 45.0f) *Matrix4::Translation(Vector3(0,0,-2));
	*viewProjMat = Matrix4::Perspective(0.1f, 1000.0f, (float)currentWidth / (float)currentHeight, 45.0f) * camera->BuildViewMatrix();
}

void NCL::PS4::PS4Renderer::DrawRenderObject(const CSC8503::RenderObject* o)
{
	Matrix4* modelMat = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	*modelMat = o->GetTransform()->GetGlobalMatrix();

	Gnm::Buffer constantBuffer;
	constantBuffer.initAsConstantBuffer(modelMat, sizeof(Matrix4));
	constantBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK

	PS4Shader* realShader = (PS4Shader*)defaultShader;
	PS4Mesh* realMesh = (PS4Mesh*)o->GetMesh();

	int objIndex = realShader->GetConstantBufferIndex("RenderObjectData");
	int camIndex = realShader->GetConstantBufferIndex("CameraData");

	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, objIndex, 1, &constantBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, camIndex, 1, &cameraBuffer);

	realShader->SubmitShaderSwitch(*currentGFXContext);
	realMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
}

void NCL::PS4::PS4Renderer::Update(float dt)
{

}

#endif //_ORBIS