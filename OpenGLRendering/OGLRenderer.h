/**
 * @file   OGLRenderer.h
 * @brief  
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
#include "RendererBase.h"

#include "Vector3.h"
#include "Vector4.h"

#ifdef _WIN32
#include "windows.h"
#endif

#include <functional>
#include <string>
#include <vector>

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif

namespace NCL {
	class MeshGeometry;
}

namespace NCL::Maths {
	class Matrix4;
}

namespace NCL::Rendering {
	class OGLMesh;
	class OGLRenderPass;
	class OGLShader;
	class OGLTexture;

	class SimpleFont;

	struct Config;
		
	class OGLRenderer : public RendererBase {
	public:
		OGLRenderer(Window& w);
		~OGLRenderer();

		void OnWindowResize(int width, int height) override;
		bool HasInitialised() const override {
			return initState;
		}

		void ForceValidDebugState(bool newState) {
			forceValidDebugState = newState;
		}

		virtual bool SetVerticalSync(VerticalSyncState s);

		void UpdateViewport(std::pair<size_t, size_t> viewport);
		void UpdateEnableDepthMask(bool enable);
		void UpdateEnableDepthTest(bool enable);
		void UpdateDepthFunc(unsigned int depthFunc);
		void UpdateEnableCullFace(bool enable);
		void UpdateCullFace(unsigned int cullFace);
		void UpdateEnableBlend(bool enable);
		void UpdateBlendFunc(std::pair<unsigned int, unsigned int> blendFunc);
		void UpdatePatchVertices(int patchVertices);

		inline Config& GetConfig() {
			return *config;
		}
	protected:
		void BeginFrame() override;
		void RenderFrame() override;
		void EndFrame() override;
		void SwapBuffers() override;

#ifdef _WIN32
		void InitWithWin32(Window& w);
		void DestroyWithWin32();

		HDC deviceContext;
		HGLRC renderContext;
#endif
		std::vector<OGLRenderPass*> renderPasses;

		Config* config;
	private:
		bool initState;
		bool forceValidDebugState;
	};

	template<typename T>
	struct CallbackWrapper {
		typedef std::function<void(T)> callback_t;
		CallbackWrapper(callback_t callback) : callback(callback) {}

		callback_t callback;
	};

	template<typename T>
	struct ConfigStack {
		ConfigStack(typename CallbackWrapper<T>::callback_t callback) : onUpdate(CallbackWrapper<T>(callback)) {}

		CallbackWrapper<T> onUpdate;
		size_t index = 0;
		std::vector<T> stack{};

		void Push(T element) {
			if (++index >= stack.size()) {
				stack.push_back(element);
				index = stack.size() - 1;
			} else {
				stack[index] = element;
			}
			onUpdate.callback(element);
#ifdef _DEBUG
			if (index > 10) {
				std::cout << "WARNING: Config stack is very large! Remember to call Pop().";
			}
#endif
		}
		void PopTo(size_t to) {
			index = to;
			onUpdate.callback(stack[index]);
		}
		void PopBy(size_t by) {
			index -= by;
			onUpdate.callback(stack[index]);
		}
		void Pop() {
			onUpdate.callback(stack[--index]);
		}
	};

	struct Config {
		friend class OGLRenderer;
		Config(OGLRenderer& parent) :
			parent(parent),
			viewport(std::bind(&OGLRenderer::UpdateViewport, &parent, std::placeholders::_1)),
			enableDepthMask(std::bind(&OGLRenderer::UpdateEnableDepthMask, &parent, std::placeholders::_1)),
			enableDepthTest(std::bind(&OGLRenderer::UpdateEnableDepthTest, &parent, std::placeholders::_1)),
			depthFunc(std::bind(&OGLRenderer::UpdateDepthFunc, &parent, std::placeholders::_1)),
			cullFace(std::bind(&OGLRenderer::UpdateCullFace, &parent, std::placeholders::_1)),
			enableCullFace(std::bind(&OGLRenderer::UpdateEnableCullFace, &parent, std::placeholders::_1)),
			enableBlend(std::bind(&OGLRenderer::UpdateEnableBlend, &parent, std::placeholders::_1)),
			blendFunc(std::bind(&OGLRenderer::UpdateBlendFunc, &parent, std::placeholders::_1)),
			patchVertices(std::bind(&OGLRenderer::UpdatePatchVertices, &parent, std::placeholders::_1)) {}

		OGLRenderer& parent;

		ConfigStack<std::pair<size_t, size_t>> viewport;
		ConfigStack<bool        > enableDepthMask;
		ConfigStack<bool        > enableDepthTest;
		ConfigStack<unsigned int> depthFunc;
		ConfigStack<bool        > enableCullFace;
		ConfigStack<unsigned int> cullFace;
		ConfigStack<bool        > enableBlend;
		ConfigStack<std::pair<unsigned int, unsigned int>> blendFunc;
		ConfigStack<int         > patchVertices;
	};
}
