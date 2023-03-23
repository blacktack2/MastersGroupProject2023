#pragma once
#ifdef _ORBIS
#include "../../Common/TextureBase.h"
#include <string>
#include <gnm\texture.h>
#include "PS4MemoryAware.h"
namespace NCL::PS4 {
	class PS4Texture :
		public Rendering::TextureBase, public PS4MemoryAware
	{
	public:
		friend class PS4RendererBase;

		PS4Texture();
		~PS4Texture();

		static PS4Texture* LoadTextureFromFile(const std::string& filename);

		void SetAPITexture(const sce::Gnm::Texture& tex) {
			apiTexture = tex;
		}

		const sce::Gnm::Texture&  GetAPITexture() {
			return apiTexture;
		}

	protected:
		sce::Gnm::Texture apiTexture;
	};
}
#endif