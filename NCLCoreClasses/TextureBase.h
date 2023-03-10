/**
 * @file   TextureBase.h
 * @brief  Utility class for encapsulating texture functionality.
 * 
 * @author Rich Davidson
 * @author Stuart Lewis
 * @date   February 2023
 */
#pragma once
constexpr unsigned int SHADOWSIZE = 4096;

namespace NCL::Rendering {
	enum class TextureType {
		ColourR8,
		ColourR16,
		ColourRGB8,
		ColourRGBA8,
		ColourRGBF,
		ColourRGB16,
		ColourRGB16F,
		ColourRGBA16F,
		ColourRGB32F,
		ColourRGBA32F,
		Depth,
		Shadow,
		Stencil,
	};
	enum class PixelDataFormat {
		Red,
		RG,
		RGB,
		RGBA,
		DepthComponent,
		DepthStencil,
	};
	enum class PixelDataType {
		UnsignedByte,
		Byte,
		UnsignedShort,
		Short,
		UnsignedInt,
		Int,
		HalfFloat,
		Float,
	};
	enum class BufferType {
		ColourAttachment,
		DepthAttachment,
		StencilAttachment,
	};

	enum class MinFilter {
		Nearest,
		Linear,
		NearestMipmapNearest,
		LinearMipmapNearest,
		NearestMipmapLinear,
		LinearMipmapLinear,
	};
	enum class MagFilter {
		Nearest,
		Linear,
	};

	enum class EdgeWrap {
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
	};

	class TextureBase {
	public:
		virtual ~TextureBase();

		inline TextureType GetType() {
			return type;
		}
		inline BufferType GetBufferType() {
			return bufferType;
		}

		/**
		 * @brief Resize the texture.
		 * @brief This will set the contents of the texture to an undefined
		 * state. Old pixels will not be preserved.
		 * @brief Texture must first be bound.
		 */
		virtual void Resize(unsigned int width, unsigned int height) = 0;
		/**
		 * @brief Fill the texture with the provided data. Data is assumed to match
		 * the bounds of the image exactly.
		 * @brief Texture must first be bound.
		 * 
		 * @param data Source data to upload.
		 * @param format Channel information of the source data.
		 * @param type Data type of the source data.
		 */
		virtual void Upload(void* data, PixelDataFormat format = PixelDataFormat::RGBA, PixelDataType type = PixelDataType::UnsignedByte) = 0;

		/**
		 * @brief Bind to texture buffer.
		 */
		virtual void Bind() const = 0;
		/**
		 * @brief Set the active texture and bind to texture buffer.
		 *
		 * @param slot Slot (offset from GL_TEXTURE0) to bind to.
		 */
		virtual void Bind(int slot) const = 0;
		/**
		 * @brief Set the active texture, bind to texture buffer, and set the
		 * specified sampler uniform to use this slot.
		 *
		 * @param slot    Slot (offset from GL_TEXTURE0) to bind to.
		 * @param uniform Sampler uniform location to set the slot of.
		 */
		virtual void Bind(int slot, int uniform) const = 0;
		/**
		 * @brief Unbind the texture buffer.
		 */
		virtual void Unbind() const = 0;

		/**
		 * @brief Set how the texture handles being viewed at different scales.
		 * @brief Texture must first be bound.
		 * @brief Default is Nearest for both, except for shadow textures,
		 * which default to Linear.
		 * 
		 * @param minFilter Rule to apply when viewing the texture up close.
		 * @param magFilter Rule to apply when viewing the texture from afar.
		 */
		virtual void SetFilters(MinFilter minFilter, MagFilter magFilter) = 0;
		/**
		 * @brief Set the edge wrap method for when a pixel is sampled outside
		 * the bounds of the texture.
		 * @brief Texture must first be bound.
		 * @brief Default is ClampToEdge.
		 */
		virtual void SetEdgeWrap(EdgeWrap edgeWrap) = 0;
	protected:
		TextureBase(TextureType type);
	private:
		TextureType type;
		BufferType bufferType;
	};
}