// OGLES2RenderFactory.h
// KlayGE OpenGL ES2��Ⱦ������ ͷ�ļ�
// Ver 3.10.0
// ��Ȩ����(C) ������, 2010
// Homepage: http://klayge.sourceforge.net
//
// 3.10.0
// ���ν��� (2010.1.22)
//
// �޸ļ�¼
//////////////////////////////////////////////////////////////////////////////////

#ifndef _OGLES2RENDERFACTORYINTERNAL_HPP
#define _OGLES2RENDERFACTORYINTERNAL_HPP

#pragma once

#include <KlayGE/PreDeclare.hpp>
#include <KlayGE/RenderFactory.hpp>

namespace KlayGE
{
	class OGLES2RenderFactory : public RenderFactory
	{
	public:
		OGLES2RenderFactory();

		std::wstring const & Name() const;

		TexturePtr MakeTexture1D(uint32_t width, uint32_t numMipMaps, uint32_t array_size,
				ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint, ElementInitData* init_data);
		TexturePtr MakeTexture2D(uint32_t width, uint32_t height, uint32_t numMipMaps, uint32_t array_size,
				ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint, ElementInitData* init_data);
		TexturePtr MakeTexture3D(uint32_t width, uint32_t height, uint32_t depth, uint32_t array_size,
				uint32_t numMipMaps, ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint, ElementInitData* init_data);
		TexturePtr MakeTextureCube(uint32_t size, uint32_t numMipMaps, uint32_t array_size,
				ElementFormat format, uint32_t sample_count, uint32_t sample_quality, uint32_t access_hint, ElementInitData* init_data);

		FrameBufferPtr MakeFrameBuffer();

		RenderLayoutPtr MakeRenderLayout();
		GraphicsBufferPtr MakeVertexBuffer(BufferUsage usage, uint32_t access_hint, ElementInitData* init_data, ElementFormat fmt = EF_Unknown);
		GraphicsBufferPtr MakeIndexBuffer(BufferUsage usage, uint32_t access_hint, ElementInitData* init_data, ElementFormat fmt = EF_Unknown);

		QueryPtr MakeOcclusionQuery();
		QueryPtr MakeConditionalRender();

		RenderViewPtr Make1DRenderView(Texture& texture, int array_index, int level);
		RenderViewPtr Make2DRenderView(Texture& texture, int array_index, int level);
		RenderViewPtr Make2DRenderView(Texture& texture, int array_index, Texture::CubeFaces face, int level);
		RenderViewPtr Make2DRenderView(Texture& texture, int array_index, uint32_t slice, int level);
		RenderViewPtr MakeCubeRenderView(Texture& texture, int array_index, int level);
		RenderViewPtr Make3DRenderView(Texture& texture, int array_index, uint32_t first_slice, uint32_t num_slices, int level);
		RenderViewPtr MakeGraphicsBufferRenderView(GraphicsBuffer& gbuffer, uint32_t width, uint32_t height, ElementFormat pf);
		RenderViewPtr Make2DDepthStencilRenderView(uint32_t width, uint32_t height, ElementFormat pf,
			uint32_t sample_count, uint32_t sample_quality);
		RenderViewPtr Make1DDepthStencilRenderView(Texture& texture, int array_index, int level);
		RenderViewPtr Make2DDepthStencilRenderView(Texture& texture, int array_index, int level);
		RenderViewPtr Make2DDepthStencilRenderView(Texture& texture, int array_index, Texture::CubeFaces face, int level);
		RenderViewPtr Make2DDepthStencilRenderView(Texture& texture, int array_index, uint32_t slice, int level);
		RenderViewPtr MakeCubeDepthStencilRenderView(Texture& texture, int array_index, int level);
		RenderViewPtr Make3DDepthStencilRenderView(Texture& texture, int array_index, uint32_t first_slice, uint32_t num_slices, int level);

		ShaderObjectPtr MakeShaderObject();

	private:
		RenderEnginePtr DoMakeRenderEngine();

		RasterizerStateObjectPtr DoMakeRasterizerStateObject(RasterizerStateDesc const & desc);
		DepthStencilStateObjectPtr DoMakeDepthStencilStateObject(DepthStencilStateDesc const & desc);
		BlendStateObjectPtr DoMakeBlendStateObject(BlendStateDesc const & desc);
		SamplerStateObjectPtr DoMakeSamplerStateObject(SamplerStateDesc const & desc);

	private:
		OGLES2RenderFactory(OGLES2RenderFactory const &);
		OGLES2RenderFactory& operator=(OGLES2RenderFactory const &);
	};
}

#endif			// _OGLES2RENDERFACTORYINTERNAL_HPP