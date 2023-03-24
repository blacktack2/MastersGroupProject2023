#ifdef _ORBIS
#include "PS4Renderer.h"
#include "../Plugins/PlayStation4/PS4Mesh.h"
#include "../Plugins/PlayStation4/PS4Shader.h"
#include "../Plugins/PlayStation4/PS4Frame.h"
#include "../Plugins/PlayStation4/PS4Texture.h"
#include "../Common/RenderObject.h"
#include "../Common/Debug.h"

using namespace NCL;
using namespace Rendering;
using namespace PS4;


PS4Renderer::PS4Renderer(GameWorld& world) : PS4RendererBase((PS4Window*)Window::GetWindow()), gameWorld(world) {

	viewProjMat = (Matrix4*)onionAllocator->allocate(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	*viewProjMat = Matrix4();


	cameraBuffer.initAsConstantBuffer(viewProjMat, sizeof(Matrix4));
	cameraBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK

	//CreatePaintTexture(1, 1, nullptr);

	defaultBumpTex = PS4::PS4Texture::LoadTextureFromFile("DefaultBump.gnf");
}

NCL::PS4::PS4Renderer::~PS4Renderer()
{
	delete defaultBumpTex;
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

	//gameWorld.OperateOnContents([&](GameObject* gameObject) {
	//	const CollisionVolume* volume = gameObject->GetBoundingVolume();
	//	if (!volume || volume->layer != CollisionLayer::PaintAble) {
	//		return;
	//	}

	//	CSC8503::RenderObject* renderObj = gameObject->GetRenderObject();
	//	if (!renderObj || !renderObj->GetPaintTexture()) {
	//		return;
	//	}

	//	
	//	const auto& target = renderObj->GetPaintTarget();
	//	currentGFXContext->setRenderTargetMask(0xF);
	//	currentGFXContext->setRenderTarget(0, &target);

	//	currentGFXContext->setupScreenViewport(0, 0, target.getWidth(), target.getHeight(), 0.5f, 0.5f);
	//	currentGFXContext->setScreenScissor(0, 0, target.getWidth(), target.getHeight());
	//	currentGFXContext->setWindowScissor(0, 0, target.getWidth(), target.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
	//	currentGFXContext->setGenericScissor(0, 0, target.getWidth(), target.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);

	//	for (const auto& collision : renderObj->GetPaintCollisions()) {

	//		Matrix4* modelMat = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	//		*modelMat = renderObj->GetTransform()->GetGlobalMatrix();

	//		Gnm::Buffer modelBuffer;
	//		modelBuffer.initAsConstantBuffer(modelMat, sizeof(Matrix4));
	//		modelBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK

	//		

	//		PaintData* paintData = (PaintData*)currentGFXContext->allocateFromCommandBuffer(sizeof(PaintData), Gnm::kEmbeddedDataAlignment4);
	//		memcpy(&paintData->position, &collision.center.array, sizeof(float) * 3);
	//		memcpy(&paintData->colour, &collision.colour.array, sizeof(float) * 3);
	//		//memcpy(&data->colour, &collision.colour.array, sizeof(float) * 3);
	//		paintData->size = collision.radius;

	//		Gnm::Buffer paintBuffer;
	//		paintBuffer.initAsConstantBuffer(paintData, sizeof(PaintData));
	//		paintBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is O

	//		int objDataIndex = paintShader->GetConstantVertexBufferIndex("RenderObjectData");
	//		currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, objDataIndex, 1, &modelBuffer);

	//		int paintDataIndex = paintShader->GetConstantPixelBufferIndex("PaintData");
	//		currentGFXContext->setConstantBuffers(Gnm::kShaderStagePs, paintDataIndex, 1, &paintBuffer);

	//		paintShader->SubmitShaderSwitch(*currentGFXContext);

	//		((PS4Mesh*)renderObj->GetMesh())->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
	//	}

	//	currentGFXContext->waitForGraphicsWrites(
	//		target.getBaseAddress256ByteBlocks(),
	//		(target.getSliceSizeInBytes() * 1) >> 8,
	//		Gnm::kWaitTargetSlotCb0,
	//		Gnm::kCacheActionWriteBackAndInvalidateL1andL2,
	//		Gnm::kExtendedCacheActionFlushAndInvalidateCbCache,
	//		Gnm::kStallCommandBufferParserDisable
	//	);
	//	
	//	Gnm::Texture apiTex;
	//	apiTex.initFromRenderTarget(&target, false);
	//	((PS4Texture*)renderObj->GetPaintTexture())->SetAPITexture(apiTex);
	//	renderObj->ClearPaintCollisions();
	//});


	SetRenderBuffer(currentPS4Buffer, true, true, true);


	//Primitive Setup State
	Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceBack);
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
		if (!(i->GetShader()))
			defaultShader->SubmitShaderSwitch(*currentGFXContext);
		currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &((PS4Texture*)(*i).GetDefaultTexture())->GetAPITexture());
		//PS4Texture* paintTex = (PS4Texture*)(*i).GetPaintTexture();
		//paintTex = paintTex ? paintTex : defaultPaintTex;
		//currentGFXContext->setTextures(Gnm::kShaderStagePs, 1, 1, &paintTex->GetAPITexture());
		
		PS4Texture* bumpTex = (PS4Texture*)((*i).GetBumpTexture());
		bumpTex = (bumpTex!=nullptr) ? bumpTex : defaultBumpTex;
		currentGFXContext->setTextures(Gnm::kShaderStagePs, 1, 1, &(bumpTex)->GetAPITexture());

		currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
		DrawRenderObject(i);
	}
	//Debug text
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
	currentGFXContext->setPrimitiveSetup(primitiveSetup);
	
	const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
	
	if (!strings.empty()) {
	//if (true) {
		PS4Mesh* debugMesh = new PS4Mesh();
		PS4Texture* debugtexture = (PS4Texture*)Debug::GetDebugFont()->GetTexture();
		currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &(debugtexture)->GetAPITexture());
		currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);
		debugTextPos.clear();
		debugTextColours.clear();
		debugTextUVs.clear();

		for (auto s : strings) {
			//float size = 20.0f;
			//std::cout << s.data<<std::endl;
			Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, s.size, debugTextPos, debugTextUVs, debugTextColours);
			
		}
		/*std::string test = "Hello World";
		Vector2 testPos = Vector2(50, 50);
		Vector4 testColor = Vector4(1, 1, 1, 1);

		Debug::GetDebugFont()->BuildVerticesForString(test, testPos, testColor, 20.0f, debugTextPos, debugTextUVs, debugTextColours);*/
		debugMesh->SetVertexPositions(debugTextPos);
		debugMesh->SetVertexColours(debugTextColours);
		debugMesh->SetVertexTextureCoords(debugTextUVs);
		

		debugMesh->UploadDebugTextToGPU();

		Matrix4* viewProjMatrix = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
		*viewProjMatrix = Matrix4::Orthographic(0.0, 100.0f, 100, 0, -1.0f, 1.0f);

		Gnm::Buffer viewProjMatBuffer;
		viewProjMatBuffer.initAsConstantBuffer(viewProjMatrix, sizeof(Matrix4));
		viewProjMatBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK
		int objIndex = debugTextShader->GetConstantVertexBufferIndex("ShaderConstants");
		currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, objIndex, 1, &viewProjMatBuffer);
		debugTextShader->SubmitShaderSwitch(*currentGFXContext);
		
		debugMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
		delete debugMesh;
	}
	else {
		//std::cout << "Empty debug strings" << std::endl;
	}
	const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();

	if (!lines.empty()) {
		//if (true) {
		PS4Mesh* debugMesh = new PS4Mesh();
		/*PS4Texture* debugtexture = (PS4Texture*)Debug::GetDebugFont()->GetTexture();
		currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &(debugtexture)->GetAPITexture());
		currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);*/
		std::vector<Vector3> positions = std::vector<Vector3>(lines.size() * 2, Vector3());
		std::vector<Vector4> colours = std::vector<Vector4>(lines.size() * 2, Vector4());
		for (size_t i = 0; i < lines.size(); i++) {
			const Debug::DebugLineEntry& entry = lines[i];
			positions[i * 2] = entry.start;
			positions[i * 2 + 1] = entry.end;
			colours[i * 2] = entry.colourA;
			colours[i * 2 + 1] = entry.colourB;
		}
		debugMesh->SetVertexPositions(positions);
		debugMesh->SetVertexColours(colours);

		debugMesh->UploadDebugLinesToGPU();

		int objIndex = debugTextShader->GetConstantVertexBufferIndex("ShaderConstants");
		currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, objIndex, 1, &cameraBuffer);
		debugLinesShader->SubmitShaderSwitch(*currentGFXContext);

		debugMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
		delete debugMesh;
	}
	else {
		//std::cout << "Empty debug strings" << std::endl;
	}




	currentFrame->EndFrame();
}

void NCL::PS4::PS4Renderer::UpdateViewProjectionMatrix(PS4Camera* camera)
{
	/**viewProjMat = Matrix4();*/
	//*viewProjMat = Matrix4::Perspective(1.0f, 1000.0f, (float)currentWidth / (float)currentHeight, 45.0f) *Matrix4::Translation(Vector3(0,0,-2));
	*viewProjMat = Matrix4::Perspective(0.1f, 1000.0f, (float)currentWidth / (float)currentHeight, 45.0f) * camera->BuildViewMatrix();
}

void NCL::PS4::PS4Renderer::CreatePaintTexture(unsigned int width, unsigned int height, CSC8503::RenderObject* object)
{
	//sce::Gnm::RenderTarget colourTarget;

	//Gnm::RenderTargetSpec spec;
	//spec.init();
	//spec.m_width = width;
	//spec.m_height = height;
	//spec.m_numSamples = Gnm::kNumSamples1;
	//spec.m_numFragments = Gnm::kNumFragments1;
	//spec.m_colorFormat = Gnm::kDataFormatB8G8R8A8Unorm;

	//GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_colorTileModeHint, GpuAddress::kSurfaceTypeColorTargetDisplayable, spec.m_colorFormat, 1);

	//int32_t success = colourTarget.init(&spec);


	//if (success != SCE_GNM_OK) {
	//	bool a = true;
	//}

	//const Gnm::SizeAlign colourAlign = colourTarget.getColorSizeAlign();

	//void* colourMemory = stackAllocators[MEMORY_GARLIC].allocate(colourAlign);

	//Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size,
	//	"Paint", Gnm::kResourceTypeRenderTargetBaseAddress, 0);

	//colourTarget.setAddresses(colourMemory, NULL, NULL);
	////Clearing the colour target i.e texture

	//Gnm::Texture apiTex;
	//apiTex.initFromRenderTarget(&colourTarget, false);

	//currentGFXContext->setRenderTargetMask(0xF);
	//currentGFXContext->setRenderTarget(0, &colourTarget);

	//currentGFXContext->setupScreenViewport(0, 0, colourTarget.getWidth(), colourTarget.getHeight(), 0.5f, 0.5f);
	//currentGFXContext->setScreenScissor(0, 0, colourTarget.getWidth(), colourTarget.getHeight());
	//currentGFXContext->setWindowScissor(0, 0, colourTarget.getWidth(), colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
	//currentGFXContext->setGenericScissor(0, 0, colourTarget.getWidth(), colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);

	//sce::Vectormath::Scalar::Aos::Vector4 clearColour(1.0f, 0.0f, 0.0f, 1.0f);
	//SurfaceUtil::clearRenderTarget(*currentGFXContext, &colourTarget, clearColour);



	//PS4Texture* tex = new PS4Texture();
	//tex->SetAPITexture(apiTex);

	//if (object) {
	//	object->SetPaintTexture(tex, colourTarget);
	//} else {
	//	defaultPaintTex = tex;
	//	paintTarget = colourTarget;
	//}
}

void NCL::PS4::PS4Renderer::DrawRenderObject(const CSC8503::RenderObject* o)
{
	Matrix4* modelMat = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	*modelMat = o->GetTransform()->GetGlobalMatrix();

	Gnm::Buffer modelMatBuffer;
	modelMatBuffer.initAsConstantBuffer(modelMat, sizeof(Matrix4));
	modelMatBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK

	Vector4* colour = (Vector4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vector4), Gnm::kEmbeddedDataAlignment4);
	*colour = o->GetColour();
	Gnm::Buffer objectColourBuffer;
	objectColourBuffer.initAsConstantBuffer(colour, sizeof(float) * 4);
	objectColourBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is O

	Vector3* cameraPos = (Vector3*)currentGFXContext->allocateFromCommandBuffer(sizeof(Vector3), Gnm::kEmbeddedDataAlignment4);
	*cameraPos = gameWorld.GetMainCamera()->GetPosition();
	Gnm::Buffer cameraPosBuffer;
	cameraPosBuffer.initAsConstantBuffer(cameraPos, sizeof(float) * 3);
	cameraPosBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is O


	PS4Shader* realShader = (PS4Shader*)defaultShader;
	PS4Mesh* realMesh = (PS4Mesh*)o->GetMesh();

	int objIndex = realShader->GetConstantVertexBufferIndex("RenderObjectData");
	int camIndex = realShader->GetConstantVertexBufferIndex("CameraData");
	int cameraPosIndex = realShader->GetConstantVertexBufferIndex("CameraPositionData");
	int colourIndex = realShader->GetConstantVertexBufferIndex("Colour");


	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, objIndex, 1, &modelMatBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, camIndex, 1, &cameraBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, cameraPosIndex, 1, &cameraPosBuffer);
	currentGFXContext->setConstantBuffers(Gnm::kShaderStageVs, colourIndex, 1, &objectColourBuffer);

	

	realShader->SubmitShaderSwitch(*currentGFXContext);
	realMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
}

void NCL::PS4::PS4Renderer::Update(float dt)
{

}

#endif //_ORBIS