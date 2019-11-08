//////////////////////////////////////////////////////////////////////////////////////////////////////
/*
幻影游戏引擎, 2009-2016, Phantom Game Engine, http://www.aixspace.com

Design Writer :   赵德贤 Dexian Zhao
Email: yuzhou_995@hotmail.com
Copyright 2009-2016 Zhao Dexian


-------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////
#include "PhantomDevice.h"
#include "PhantomManager.h"
#include "PhantomFile.h"
#include "encrypt.h"
#include "png.h"
#include "jpeg_c/jpeglib.h"
#ifdef WIN32_OS
extern	VOID	ExportPngs(const char* szFile, Phantom::Texture* tex);
#endif
namespace Phantom{
	void jpeg_error_exit(j_common_ptr cinfo){
		//cinfo->err->output_message(cinfo);
	}
	BOOL	LoadJpeg(ImageBuffer& ret, const void* dataPtr, int dataLength)
	{
#ifndef EXPORT_ENGINE
		JSAMPARRAY lineBuf;
		struct jpeg_decompress_struct cinfo;
		struct jpeg_error_mgr err_mgr;
		int bytesPerPix;
		u_char *retBuf;
		cinfo.err = jpeg_std_error(&err_mgr);
		err_mgr.error_exit = jpeg_error_exit;
		jpeg_create_decompress(&cinfo);
		jpeg_mem_src(&cinfo, (unsigned char*)dataPtr, dataLength);
		jpeg_read_header(&cinfo, 1);
		cinfo.do_fancy_upsampling = 0;
		cinfo.do_block_smoothing = 0;
		jpeg_start_decompress(&cinfo);
		ret.width = cinfo.output_width;
		ret.height = cinfo.output_height;
		bytesPerPix = cinfo.output_components;
		lineBuf = cinfo.mem->alloc_sarray((j_common_ptr) & cinfo, JPOOL_IMAGE, (ret.width * bytesPerPix), 1);
		retBuf = (u_char *) malloc_s(3 * (ret.width * ret.height));
		if (NULL == retBuf) {
			perror(NULL);
			return false;
		}
		if (3 == bytesPerPix) {
			int y;
			for (y = 0; y < cinfo.output_height; ++y) {
				jpeg_read_scanlines(&cinfo, lineBuf, 1);
				memcpy((retBuf + y * ret.width * 3), lineBuf[0], 3 * ret.width);
			}
		} else if (1 == bytesPerPix) {
			unsigned int col;
			int lineOffset = (ret.width * 3);
			int lineBufIndex;
			int x;
			int y;
			for (y = 0; y < cinfo.output_height; ++y) {
				jpeg_read_scanlines(&cinfo, lineBuf, 1);
				lineBufIndex = 0;
				for (x = 0; x < lineOffset; ++x) {
					col = lineBuf[0][lineBufIndex];
					retBuf[(lineOffset * y) + x] = col;
					++x;
					retBuf[(lineOffset * y) + x] = col;
					++x;
					retBuf[(lineOffset * y) + x] = col;
					++lineBufIndex;
				}
			}
		} else {
			return false;
		}
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);
		ret.bpp = ret.PixelFormat_RGB;
		ret.pixels = retBuf;
#endif
		return true;
	}
#ifndef EXPORT_ENGINE
	void	PngReadCallback(png_structp structPtr, png_bytep data, png_size_t length);

	struct	PngReader
	{
		png_structp		structPtr;
		png_infop		infoPtr;
		int				pngDataLength;
		unsigned char*	pngDataPtr;
		int				pngPointer;
        //static PngReader* s_curReader;
		PngReader()
		{
			pngDataLength	=	0;
			pngDataPtr		=	0;
			pngPointer		=	0;
			structPtr	=	0;
			infoPtr		=	0;
		}
		~PngReader()
		{
            if(infoPtr)
                png_destroy_info_struct(structPtr, &infoPtr);
			if(structPtr)
				png_destroy_read_struct(&structPtr, (infoPtr) ? &infoPtr : 0, 0);
		}
		BOOL		Load(ImageBuffer& ret, const void* dataPtr, int dataLength, BOOL bDivUsage)
		{
			ret.FreePixels();
			//
			png_byte        header[8]   = {0}; 
			unsigned char * pImateData  = 0;
			if(dataLength < 8)
				return false;
			memcpy_s(header, dataPtr, 8);
			if(png_sig_cmp(header, 0, 8))
				return false;
			if(structPtr)
				png_destroy_read_struct(&structPtr, (infoPtr) ? &infoPtr : 0, 0);
			structPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
			if(!structPtr)
				return false;
			infoPtr = png_create_info_struct(structPtr);
			if(!infoPtr)
				return false;
			pngPointer			=	0;
			pngDataPtr			=	(unsigned char*)dataPtr;
			pngDataLength		=	dataLength;
            //s_curReader         =   this;
			png_set_read_fn(structPtr, this, PngReadCallback);
			png_read_png(structPtr, infoPtr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_GRAY_TO_RGB, 0);
			int colorType;
			int nBPP;
			unsigned int nWidth, nHeight;
			png_get_IHDR(structPtr, infoPtr, &nWidth, &nHeight, &nBPP, &colorType, 0, 0, 0);
			if(colorType == PNG_COLOR_TYPE_PALETTE)
			{
				png_set_packing(structPtr);
				png_set_palette_to_rgb(structPtr); //Expand data to 24-bit RGB or 32-bit RGBA if alpha available.
			}
			int bytesPerPixel = 3;
			if (colorType == PNG_COLOR_TYPE_GRAY && nBPP < 8)
			{
				//png_set_gray_1_2_4_to_8(structPtr);
				bytesPerPixel = 4;
			}
			else if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			{
				png_set_gray_to_rgb(structPtr);
				bytesPerPixel = 4;
			}
			else if (nBPP == 16)
			{
				png_set_strip_16(structPtr);
				bytesPerPixel = 4;
			}
			else if (colorType == PNG_COLOR_TYPE_RGB_ALPHA)
			{
				bytesPerPixel = 4;
			}
			if(bytesPerPixel == 4)//colorType & PNG_COLOR_MASK_ALPHA)
			{
				ret.bpp	=	ImageBuffer::PixelFormat_RGBA;
				bytesPerPixel = 4;
			}
			else
				ret.bpp	=	ImageBuffer::PixelFormat_RGB;
			int nDiv	=	g_manager.GetTextureDiv();
			if(!bDivUsage)
				nDiv	=	1;
			ret.nDiv	=	nDiv;
			if(g_manager.GetTextureDivAsFile())
			{
				LogInfo("system->%d %d, AsFile Div2 %d", nWidth, nHeight, ret.nDiv);
				nDiv	=	1;
			}
			ret.width	=	nWidth / nDiv;
			ret.height	=	nHeight / nDiv;
			ret.pixels	=	(unsigned char*)malloc_s(ret.width * ret.height * bytesPerPixel * sizeof(unsigned char));
			if(!ret.pixels)
				return false;
			//
			png_bytep * rowPointers = png_get_rows(structPtr, infoPtr);
			int bytesPerRow = nWidth * bytesPerPixel;
			//
			if(ret.bpp == ImageBuffer::PixelFormat_RGBA)
			{
				int nWPixel	=	nDiv * 4;
				int index	=	0;
				for(unsigned int i = 0; i < nHeight; i+=nDiv)
				{
					for(int j = 0; j < bytesPerRow; j += nWPixel)
					{
						ret.pixels[index++] = rowPointers[i][j + 0];
						ret.pixels[index++] = rowPointers[i][j + 1];
						ret.pixels[index++] = rowPointers[i][j + 2];
						ret.pixels[index++] = rowPointers[i][j + 3];
//						assert(index <= (ret.width * ret.height * 4));
					}
				}
			}
			else
			{
				int nWPixel	=	nDiv * 3;
				int index	=	0;
				assert(bytesPerRow == 3 * nWidth);
				for(unsigned int i = 0; i < nHeight; i+=nDiv)
				{
					for(int j = 0; j < bytesPerRow; j += nWPixel)
					{
						ret.pixels[index++] = rowPointers[i][j + 0];
						ret.pixels[index++] = rowPointers[i][j + 1];
						ret.pixels[index++] = rowPointers[i][j + 2];
						assert(index <= (ret.width * ret.height * 3));
					}
				}
				//	memcpy_s(ret.pixels + j * bytesPerRow, rowPointers[j], bytesPerRow);
			}
			return true;
		}
	};
    //PngReader* PngReader::s_curReader = 0;
	void	PngReadCallback(png_structp structPtr, png_bytep data, png_size_t length)
	{
		PngReader* r = (PngReader*)structPtr->io_ptr;
		//PngReader* r = PngReader::s_curReader;//(PngReader*)png_get_io_ptr(structPtr);
		if((int)(r->pngPointer + length) <= r->pngDataLength)
		{
			memcpy_s(data, r->pngDataPtr + r->pngPointer, length);
			r->pngPointer	+=	length;
		}
	}
	VOID	LoadPng(const char* s)
	{
		StreamBase* dataPtr	=	open_s(s, true);
		PngReader r;
		ImageBuffer img;
		if(!r.Load(img, dataPtr->getMemoryPtr(), dataPtr->size(), false))
		{
		}
	}
#endif
//	DynamicMesh::~DynamicMesh()
//	{
//		Destroy();
//	}
//#ifdef DIRECT_X
//	VOID		DynamicMesh::SetPos(unsigned int index, const float3& pos)
//	{
//		if(m_pos >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		float3* v = (float3*)(begin + m_pos);
//		*v = pos;
//		if(this->m_bIs2DMode)
//			g_manager.SetupScreenPos(*v);
//	}
//	VOID		DynamicMesh::SetNormal(unsigned int index, const float3& normal)
//	{
//		if(m_normal >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		float3* v = (float3*)(begin + m_normal);
//		*v = normal;
//	}
//	VOID		DynamicMesh::SetDiffuse(unsigned int index, unsigned int diffuse)
//	{
//		if(m_diffuse >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		unsigned int* v = (unsigned int*)(begin + m_diffuse);
//		*v = diffuse;
//	}
//	VOID		DynamicMesh::SetSpecular(unsigned int index, unsigned int specular)
//	{
//		if(m_specular >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		unsigned int* v = (unsigned int*)(begin + m_specular);
//		*v = specular;
//	}
//	VOID		DynamicMesh::SetUV(unsigned int index, const float2& pos)
//	{
//		if(m_uv >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		float2* v = (float2*)(begin + m_uv);
//		*v = pos;
//	}
//	VOID		DynamicMesh::SetUV2(unsigned int index, const float2& pos)
//	{
//		if(m_uv2 >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		float2* v = (float2*)(begin + m_uv2);
//		*v = pos;
//	}
//	VOID		DynamicMesh::SetUV3(unsigned int index, const float2& pos)
//	{
//		if(m_uv3 >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		float2* v = (float2*)(begin + m_uv3);
//		*v = pos;
//	}
//	VOID		DynamicMesh::SetUV4(unsigned int index, const float2& pos)
//	{
//		if(m_uv4 >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		float2* v = (float2*)(begin + m_uv4);
//		*v = pos;
//	}
//	VOID		DynamicMesh::SetUV5(unsigned int index, const float2& pos)
//	{
//		if(m_uv5 >= 255 || index >= m_nNumVertices)
//			return;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		float2* v = (float2*)(begin + m_uv5);
//		*v = pos;
//	}
//	BOOL					DynamicMesh::GetPos(unsigned int index, float3& ret)
//	{
//		if(m_pos >= 255 || index >= m_nNumVertices)
//			return false;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(float3*)(begin + m_pos);
//		return true;
//	}
//	BOOL					DynamicMesh::GetNormal(unsigned int index, float3& ret)
//	{
//		if(m_normal >= 255 || index >= m_nNumVertices)
//			return false;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(float3*)(begin + m_normal);
//		return true;
//	}
//	BOOL					DynamicMesh::GetDiffuse(unsigned int index, unsigned int& ret)
//	{
//		if(m_diffuse >= 255 || index >= m_nNumVertices)
//			return 0;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(unsigned int*)(begin + m_diffuse);
//		return true;
//	}
//	BOOL					DynamicMesh::GetSpecular(unsigned int index, unsigned int& ret)
//	{
//		if(m_specular >= 255 || index >= m_nNumVertices)
//			return 0;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(unsigned int*)(begin + m_specular);
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV(unsigned int index, float2& ret)
//	{
//		if(m_uv >= 255 || index >= m_nNumVertices)
//			return false;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(float2*)(begin + m_uv);
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV2(unsigned int index, float2& ret)
//	{
//		if(m_uv2 >= 255 || index >= m_nNumVertices)
//			return false;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(float2*)(begin + m_uv2);
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV3(unsigned int index, float2& ret)
//	{
//		if(m_uv3 >= 255 || index >= m_nNumVertices)
//			return false;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(float2*)(begin + m_uv3);
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV4(unsigned int index, float2& ret)
//	{
//		if(m_uv4 >= 255 || index >= m_nNumVertices)
//			return false;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(float2*)(begin + m_uv4);
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV5(unsigned int index, float2& ret)
//	{
//		if(m_uv5 >= 255 || index >= m_nNumVertices)
//			return false;
//		const char* begin = m_vertices.GetPtr() + m_nStrideSize * index;
//		ret = *(float2*)(begin + m_uv5);
//		return true;
//	}
//
//#else
//	VOID		DynamicMesh::SetPos(unsigned int index, const float3& pos)
//	{
//		if(m_pos >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_posV.size() == 0)m_posV.SetArrayCount(m_nNumVertices);
//		m_posV[index]	=	pos;
//		if(this->m_bIs2DMode)
//			g_manager.SetupScreenPos(m_posV[index]);
//	}
//	VOID		DynamicMesh::SetNormal(unsigned int index, const float3& normal)
//	{
//		if(m_normal >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_normalV.size() == 0)m_normalV.SetArrayCount(m_nNumVertices);
//		m_normalV[index]	=	normal;
//	}
//	VOID		DynamicMesh::SetDiffuse(unsigned int index, unsigned int diffuse)
//	{
//		if(m_diffuse >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_diffuseV.size() == 0)m_diffuseV.SetArrayCount(m_nNumVertices);
//		m_diffuseV[index]	=	diffuse;
//	}
//	VOID		DynamicMesh::SetSpecular(unsigned int index, unsigned int specular)
//	{
//		if(m_specular >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_specularV.size() == 0)m_specularV.SetArrayCount(m_nNumVertices);
//		m_specularV[index]	=	specular;
//	}
//	VOID		DynamicMesh::SetUV(unsigned int index, const float2& pos)
//	{
//		if(m_uv >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_uvV.size() == 0)m_uvV.SetArrayCount(m_nNumVertices);
//		m_uvV[index]	=	pos;
//	}
//	VOID		DynamicMesh::SetUV2(unsigned int index, const float2& pos)
//	{
//		if(m_uv2 >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_uv2V.size() == 0)m_uv2V.SetArrayCount(m_nNumVertices);
//		m_uv2V[index]	=	pos;
//	}
//	VOID		DynamicMesh::SetUV3(unsigned int index, const float2& pos)
//	{
//		if(m_uv3 >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_uv3V.size() == 0)m_uv3V.SetArrayCount(m_nNumVertices);
//		m_uv3V[index]	=	pos;
//	}
//	VOID		DynamicMesh::SetUV4(unsigned int index, const float2& pos)
//	{
//		if(m_uv4 >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_uv4V.size() == 0)m_uv4V.SetArrayCount(m_nNumVertices);
//		m_uv4V[index]	=	pos;
//	}
//	VOID		DynamicMesh::SetUV5(unsigned int index, const float2& pos)
//	{
//		if(m_uv5 >= 255 || index >= m_nNumVertices)
//			return;
//		if(m_uv5V.size() == 0)m_uv5V.SetArrayCount(m_nNumVertices);
//		m_uv5V[index]	=	pos;
//	}
//	BOOL					DynamicMesh::GetPos(unsigned int index, float3& ret)
//	{
//		if(m_pos >= 255 || index >= m_nNumVertices)
//			return false;
//		ret = m_posV[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetNormal(unsigned int index, float3& ret)
//	{
//		if(m_normal >= 255 || index >= m_nNumVertices)
//			return false;
//		ret = m_normalV[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetDiffuse(unsigned int index, unsigned int& ret)
//	{
//		if(m_diffuse >= 255 || index >= m_nNumVertices)
//			return 0;
//		ret = m_diffuseV[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetSpecular(unsigned int index, unsigned int& ret)
//	{
//		if(m_specular >= 255 || index >= m_nNumVertices)
//			return 0;
//		ret = m_specularV[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV(unsigned int index, float2& ret)
//	{
//		if(m_uv >= 255 || index >= m_nNumVertices)
//			return false;
//		ret = m_uvV[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV2(unsigned int index, float2& ret)
//	{
//		if(m_uv2 >= 255 || index >= m_nNumVertices)
//			return false;
//		ret = m_uv2V[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV3(unsigned int index, float2& ret)
//	{
//		if(m_uv3 >= 255 || index >= m_nNumVertices)
//			return false;
//		ret = m_uv3V[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV4(unsigned int index, float2& ret)
//	{
//		if(m_uv4 >= 255 || index >= m_nNumVertices)
//			return false;
//		ret = m_uv4V[index];
//		return true;
//	}
//	BOOL					DynamicMesh::GetUV5(unsigned int index, float2& ret)
//	{
//		if(m_uv5 >= 255 || index >= m_nNumVertices)
//			return false;
//		ret = m_uv5V[index];
//		return true;
//	}
//#endif
//	inline	unsigned short	AddPtr(unsigned short& index, BOOL b, int typeSize)
//	{
//		if(b)
//		{
//			int ret	=	index;
//			index	+=	typeSize;
//			return ret;
//		}
//		return 255;
//	}
//	//
//	VOID			DynamicMesh::Destroy()
//	{
//		m_triangles.clear();
//		m_nNumVertices	=	0;
//#ifdef DIRECT_X//directx中无效
//		m_vertices.clear();
//#else
//		m_posV.clear();
//		m_normalV.clear();
//		m_diffuseV.clear();
//		m_specularV.clear();
//		m_uvV.clear();
//		m_uv2V.clear();
//		m_uv3V.clear();
//		m_uv4V.clear();
//		m_uv5V.clear();
//#endif
//		m_nStrideSize	=	0;
//		m_nFVF			=	0;
//		for(int i=0;i<m_attributes.size();i++)
//		{
//			safe_release(m_attributes[i].texPtr);
//		}
//		m_attributes.clear();
//	}
//	VOID			DynamicMesh::Create(unsigned int nFVF, unsigned short numVertices, DrawMode mode, BOOL bIs2DMode)
//	{
//		Destroy();
//		m_bIs2DMode	=	bIs2DMode;
//		unsigned short	index = 0;
//		m_drawMode	=	mode;
//		m_nFVF		=	nFVF;
//		m_pos		=	AddPtr(index, true, sizeof(float3));
//		m_normal	=	AddPtr(index, (m_nFVF & FVF_NORMAL) == FVF_NORMAL, sizeof(float3));
//		m_diffuse	=	AddPtr(index, (m_nFVF & FVF_DIFFUSE) == FVF_DIFFUSE, sizeof(unsigned int));
//		m_specular	=	AddPtr(index, (m_nFVF & FVF_SPECULAR) == FVF_SPECULAR, sizeof(unsigned int));
//		//
//		BOOL t1,t2,t3,t4,t5;
//		t1=t2=t3=t4=t5 = false;
//		if((m_nFVF & FVF_TEX5) == FVF_TEX5)t1=t2=t3=t4=t5=true;
//		if((m_nFVF & FVF_TEX4) == FVF_TEX4)t1=t2=t3=t4=true;
//		if((m_nFVF & FVF_TEX3) == FVF_TEX3)t1=t2=t3=true;
//		if((m_nFVF & FVF_TEX2) == FVF_TEX2)t1=t2=true;
//		if((m_nFVF & FVF_TEX1) == FVF_TEX1)t1=true;
//		m_uv		=	AddPtr(index, t1, sizeof(float2));
//		m_uv2		=	AddPtr(index, t2, sizeof(float2));
//		m_uv3		=	AddPtr(index, t3, sizeof(float2));
//		m_uv4		=	AddPtr(index, t4, sizeof(float2));
//		m_uv5		=	AddPtr(index, t5, sizeof(float2));
//		m_nStrideSize	=	index;
//		SetNumVertices(numVertices);
//		if(m_bIs2DMode){
//			this->m_technique.SetName("tech_colortex1");
//		}
//	}
//	VOID			DynamicMesh::SetTexture(int attrID, Texture* t)
//	{
//		if(attrID < 0 || attrID >= m_attributes.size())return;
//		safe_addref(t);
//		safe_release(m_attributes[attrID].texPtr);
//		m_attributes[attrID].texPtr = t;
//	}
//	int				DynamicMesh::AddAttribute(Texture* texPtr, int beginIndex, int count)//添加多个贴图组合返回attrID
//	{
//		for(int i=0;i<m_attributes.size();i++)
//			if(m_attributes[i].texPtr == texPtr)
//			{
//				return i;
//			}
//		MeshAttribute m;
//		memset(&m, 0, sizeof(m));
//		m.beginIndex = beginIndex;
//		m.count = count;
//		m.texPtr = texPtr;
//		safe_addref(m.texPtr);
//		m_attributes.push_back(&m);
//		return (m_attributes.size() - 1);
//	}
//	VOID			DynamicMesh::SetAttribute(int triangleIndex, int attrID)
//	{
//		if(attrID < 0 || attrID >= m_attributes.size())return;
//		if(m_triangleAttrs.size() == 0)
//		{
//			m_triangleAttrs.SetArrayCount(m_triangles.size());
//		}
//		m_triangleAttrs[triangleIndex] = attrID;
//	}
//	VOID			DynamicMesh::RebuildAttributes()//重新构造渲染列表
//	{
//		if(m_attributes.size() == 0)return;
//		CDynamicArray<Triangle>*		temps = new CDynamicArray<Triangle>[m_attributes.size()];
//		m_trianglesRenders.SetArrayCount(m_triangles.size());
//		for(int i=0;i<m_triangleAttrs.size();i++)
//		{
//			CDynamicArray<Triangle>& t = temps[m_triangleAttrs[i]];
//			t.push_back(&m_triangles[i]);
//		}
//		int begin = 0;
//		for(int i=0;i<m_attributes.size();i++)
//		{
//			int count = 0;
//			CDynamicArray<Triangle>& t = temps[i];
//			m_attributes[i].beginIndex = begin;
//			for(int j=0;j<t.size();j++)
//			{
//				m_trianglesRenders[begin++] = t[j];
//			}
//			m_attributes[i].count = t.size();
//		}
//		assert(begin == m_triangles.size());
//		delete[] temps;
//	}
//	BOOL			DynamicMesh::PickMesh(const float3& orig, const float3& dir, PIntersectData* pOutPick, BOOL bDoubleSide)//点击这个模型判断
//	{
//		float3 v1,v2,v3;
//		if(m_triangles.size() == 0)
//		{
//			switch(this->m_drawMode)
//			{
//			case Phantom::DrawMode_TriangleFan:
//				{
//					GetPos(0, v1);
//					for(int i=1;i<this->m_nNumVertices - 1;i++)
//					{
//						GetPos(i, v2);
//						GetPos(i+1, v3);
//						if(IntersectTriangle(pOutPick, orig, dir, v1, v2, v3, 0, bDoubleSide==TRUE))
//							return true;
//					}
//				}
//				break;
//			case Phantom::DrawMode_TriangleStrip:
//				{
//					for(int i=0;i<(m_nNumVertices - 2);i++)
//					{
//						if((i%2) == 0){
//							GetPos(i, v1);
//							GetPos(i+1, v2);
//							GetPos(i+2, v3);
//						}
//						else
//						{
//							GetPos(i+2, v1);
//							GetPos(i+1, v2);
//							GetPos(i+0, v3);
//						}
//						if(IntersectTriangle(pOutPick, orig, dir, v1, v2, v3, 0, bDoubleSide==TRUE))
//							return true;
//					}
//				}
//				break;
//			case Phantom::DrawMode_Triangles:
//				{
//					for(int i=0;i<(m_nNumVertices/3);i++)
//					{
//						GetPos(i * 3, v1);
//						GetPos(i * 3 + 1, v2);
//						GetPos(i * 3 + 2, v3);
//						if(IntersectTriangle(pOutPick, orig, dir, v1, v2, v3, 0, bDoubleSide==TRUE))
//							return true;
//					}
//				}
//				break;
//			}
//		}
//		else
//		{
//			int numTri = m_triangles.size();
//			for(int i=0;i<numTri;i++)
//			{
//				if(!GetPos(this->m_triangles[i + 0].v1, v1))
//					return false;
//				if(!GetPos(this->m_triangles[i + 1].v2, v2))
//					return false;
//				if(!GetPos(this->m_triangles[i + 2].v3, v3))
//					return false;
//				if(IntersectTriangle(pOutPick, orig, dir, v1, v2, v3, 0, bDoubleSide==TRUE))
//					return true;
//			}
//		}
//		return false;
//	}
//	VOID			DynamicMesh::WriteIndexBuffer(const unsigned short* indices, unsigned int indiceCount)
//	{
//		m_triangles.SetArrayCount(indiceCount / 3);
//		if(indiceCount > 0)
//			memcpy_s(m_triangles.GetPtr(), indices, sizeof(unsigned short) * indiceCount);
//	}
//	VOID			DynamicMesh::SetNumVertices(unsigned short numVertices)
//	{
//		if(this->m_nStrideSize <= 0) 
//			return;
//		this->m_nNumVertices	=	numVertices;
//#ifdef DIRECT_X
//		m_vertices.SetArrayCount(m_nStrideSize * m_nNumVertices);
//#else
//		//m_posV.SetArrayCount(m_nNumVertices);
//		//m_normalV.SetArrayCount(m_nNumVertices);
//		//m_diffuseV.SetArrayCount(m_nNumVertices);
//		//m_specularV.SetArrayCount(m_nNumVertices);
//		//m_uvV.SetArrayCount(m_nNumVertices);
//		//m_uv2V.SetArrayCount(m_nNumVertices);
//		//m_uv3V.SetArrayCount(m_nNumVertices);
//		//m_uv4V.SetArrayCount(m_nNumVertices);
//		//m_uv5V.SetArrayCount(m_nNumVertices);
//
//#endif
//	}
//	VOID			DynamicMesh::SetNumTriangles(unsigned short numTriangles)
//	{
//		m_triangles.SetArrayCount(numTriangles);
//		if(m_triangleAttrs.size() > 0)
//			m_triangleAttrs.SetArrayCount(numTriangles);
//	}
//	VOID			DynamicMesh::SetTriangle(unsigned int index, const Triangle& t)
//	{
//		if(index >= m_triangles.size() || t.v1 >= m_nNumVertices || t.v2 >= m_nNumVertices || t.v3 >= m_nNumVertices)
//			return;
//		m_triangles[index]	=	t;
//	}
//	Triangle*		DynamicMesh::GetTriangle(unsigned int triangleIndex)
//	{
//		if(triangleIndex >= m_triangles.size())
//			return 0;
//		return (Triangle*)&m_triangles[triangleIndex];
//	}
//	VOID				DynamicMesh::Render()	//渲染,使用m_world, m_texturePtr, m_technique
//	{
//		if(m_nFVF == 0)
//			return;
//		if(m_nRenderMode >= 0)
//		{
//			if(!g_manager.SetRenderMode((int)m_nRenderMode))
//				return;
//		}
//		else if(!g_manager.SetRenderMode(m_technique, false, m_technique.name))
//			return;
//		if(m_bIs2DMode){
//			g_manager->SetCullVisible((Phantom::CullMode)m_nCullMode);
//			g_manager.SetWorldMatrix2D(0);
//		}
//		else
//		{
//			g_manager.SetWorldMatrix(&m_world);
//		}
//		color4 c(1,1,1,1);
//		g_manager.GetCurrProgram()->SetDiffuse(c);
//		DrawOnly();
//	}
//	DynamicMesh::DynamicMesh()
//	{
//		m_nRenderMode	=	-1;
//		m_bIs2DMode	=	false;
//		m_nCullMode	=	Phantom::CullMode_CW;
//		this->m_technique.SetName("tech_default");
//		m_world.identity();
//		m_drawMode	=	DrawMode_Points;
//		m_pos		=	255;
//		m_normal	=	255;
//		m_diffuse	=	255;
//		m_specular	=	255;
//		m_uv		=	255;
//		m_uv2		=	255;
//		m_uv3		=	255;
//		m_uv4		=	255;
//		m_uv5		=	255;
//		m_nNumVertices	=	0;
//		m_nFVF			=	0;
//		m_nStrideSize	=	0;
//	}
//
	BOOL IsSizeOK(int s){
		int n = 2;
		for(int i=0;i<16;i++)
		{
			if(n == s)
				return true;
			n=n*2;
		}
		return false;
	}
	ImageBuffer::ImageBuffer()
	{
		width		=	0;
		height		=	0;
		bpp			=	0;
		pixels		=	0;
		alphas		=	0;	//使用与像素级测试的选项
		nDiv		=	1;
	}
	ImageBuffer::~ImageBuffer()
	{
		FreePixels();
	}
	VOID			ImageBuffer::AutoResize(const char* szFile)//自动重新计算贴图大小,这会消耗一定的时间,并给出提示
	{
		if(!IsSizeOK(width) || !IsSizeOK(height))
		{
			LogInfo("system->(%s)%s", szFile, utf8("图片大小最好是2的倍数,比如(2,4,8,16,32,64,128,256,512,1024,2048,4096)等，否则将在运行时消耗更多内存,占用更多载入时间"));
		}
		else
			return;
		if(!pixels)return;
		int nSize = 1;
		for(int i=0;i<16;i++){
			nSize *= 2;
			if(width<=nSize){
				width = nSize;
				break;
			}
		}
		nSize = 1;
		for(int i=0;i<16;i++){
			nSize *= 2;
			if(height<=nSize){
				height = nSize;
				break;
			}
		}
		int bppWidth = 1;
		if(bpp==PixelFormat_RGB)
			bppWidth = 3;
		else if(bpp==PixelFormat_RGBA)
			bppWidth = 4;
		else
			bppWidth = 2;
		unsigned char*	newpixels = (unsigned char*)malloc(bppWidth*width*height);
		memset(newpixels, 0, bppWidth*width*height);
		for(int x=0;x<width;x++){
			if(x>=oriwidth)continue;
			for(int y=0;y<height;y++){
				if(y>=oriheight)continue;
				unsigned char*	src = pixels + (x+y*oriwidth)*bppWidth;
				unsigned char*	dest = newpixels + (x+y*width)*bppWidth;
				for(int i=0;i<bppWidth;i++)
					dest[i] = src[i];
			}
		}
		safe_free(pixels);
		pixels = newpixels;
	}
	VOID			ImageBuffer::FreePixels(BOOL bAlpha)
	{CPUTime(ImageBuffer);
		safe_free(pixels);
		if(bAlpha)
			safe_free(alphas);
	}
	BOOL			ImageBuffer::GenericAlpha()
	{CPUTimeNone();
		if(bpp == PixelFormat_RGB||bpp == PixelFormat_RGB_565)
			return false;
		int w	=	width / 8;
		int h	=	height;
		safe_free(alphas);
		alphas	=	(unsigned char*)malloc_s(w * h * sizeof(unsigned char));
		memset(alphas, 0, w * h * sizeof(unsigned char));
		//
		for(int x=0;x<width;x++)
		{
			for(int y=0;y<height;y++)
			{
				if(bpp==ImageBuffer::PixelFormat_RGBA){
					unsigned char* buf = pixels + (x + y * width) * 4;
					if(buf[3] > 0){
						SetAlpha(x, y);
					}
				}
				else if(bpp==ImageBuffer::PixelFormat_RGBA_4444){
					unsigned char* buf = pixels + (x + y * width) * 2;
#ifdef WIN32_OS
					int tempA = buf[1]/16;
#else
					int tempA = buf[0]%16;
					//tp[0] = a + (b*16);
#endif
					if(tempA > 0){
						SetAlpha(x, y);
					}
				}
			}
		}
		return true;
	}
	unsigned char*						ToSmallRGBA(unsigned char* pixels, int width, int height)
	{
		unsigned char* temp = (unsigned char*)malloc_s(width*height*sizeof(unsigned char)*2);
		for(int x=0;x<width;x++)
		{
			for(int y=0;y<height;y++)
			{
				unsigned char* tp = temp + (x + y * width) * 2;
				unsigned char* sp = pixels + (x + y * width) * 4;
				int r = ((int)sp[0])/16;
				int g = ((int)sp[1])/16;
				int b = ((int)sp[2])/16;
				int a = ((int)sp[3])/16;
				tp[0] = r + (g*16);
				tp[1] = b + (a*16);
			}
		}
		return temp;
	}
	INT				ImageBuffer::GetBufferSize()
	{CPUTime(ImageBuffer);
		if(bpp == PixelFormat_RGBA)
			return this->width*this->height*4;
		else if(bpp == PixelFormat_RGB)
			return this->width*this->height*3;
		else if(bpp == PixelFormat_RGBA_4444 || bpp == PixelFormat_RGB_565)
			return this->width*this->height*2;
		return 0;
	}
	VOID			ImageBuffer::ToSmallRGB()
	{CPUTime(ImageBuffer);
		if(!pixels)
			return;
		unsigned char* temp = (unsigned char*)malloc_s(width*height*sizeof(unsigned char)*2);
		if(bpp == PixelFormat_RGB){
			bpp = PixelFormat_RGB_565;
			for(int x=0;x<width;x++)
			{
				for(int y=0;y<height;y++)
				{
					unsigned short* tp = (unsigned short*)(temp + (x + y * width) * 2);
					unsigned char* sp = pixels + (x + y * width) * 3;
					int r = ((int)sp[0]);
					int g = ((int)sp[1]);
					int b = ((int)sp[2]);
					*tp = (((r >> 3 ) << 11 ) | ((g >> 2 ) << 5 ) | (b >> 3 ));
					//*tp = (b >> 3 ) | ((g >> 2 ) << 5 ) | ((r >> 3 ) << 11);//
//#ifdef WIN32_OS
//					tp[0] = b + (g*16);
//					tp[1] = r + (a*16);
//#else
//					tp[0] = r + (g*16);
//					tp[1] = b + (a*16);
//#endif
				}
			}
		}
		else
		{
			bpp = PixelFormat_RGBA_4444;
			for(int x=0;x<width;x++)
			{
				for(int y=0;y<height;y++)
				{
					unsigned char* tp = temp + (x + y * width) * 2;
					unsigned char* sp = pixels + (x + y * width) * 4;
					int r = ((int)sp[0])/16;
					int g = ((int)sp[1])/16;
					int b = ((int)sp[2])/16;
					int a = ((int)sp[3])/16;
#ifdef WIN32_OS
					tp[0] = b + (g*16);
					tp[1] = r + (a*16);
#else
					tp[0] = a + (b*16);
					tp[1] = g + (r*16);
#endif
				}
			}
		}
		free_s(pixels);
		pixels = temp;
	}

	BOOL			ImageBuffer::SetAlpha(int x, int y)
	{CPUTime(ImageBuffer);
		if(bpp == PixelFormat_RGB || bpp == PixelFormat_RGB_565)
			return false;
		if(x < 0 || y < 0 || x >= width || y >= height)
			return false;
		if(!alphas)
			return false;
		int xx	=	x / 8;
		int yy	=	y;
		int w	=	width / 8;
		alphas[xx + yy * w] |= (1 << (x % 8));
		return true;
	}
	BOOL			ImageBuffer::IsAlpha(int x, int y)//是否透明
	{CPUTime(ImageBuffer);
		if(bpp == PixelFormat_RGB || bpp == PixelFormat_RGB_565)
			return false;
		if(x < 0 || y < 0 || x >= width || y >= height)
			return false;
		if(!alphas)
			return false;
		int xx	=	x / 8;
		int yy	=	y;
		int w	=	width / 8;
		//return (alphas[xx + yy * w] & (1 << (xx % 8))) > 0;
		return (alphas[xx + yy * w] & (1 << (x % 8))) > 0;
	}
	int	GetNearestSize(int n){
		if(n<=8)return 8;
		if(n<=16)return 16;
		if(n<=32)return 32;
		if(n<=64)return 64;
		if(n<=128)return 128;
		if(n<=256)return 256;
		if(n<=512)return 512;
		if(n<=1024)return 1024;
		if(n<=2048)return 2048;
		if(n<=4096)return 4096;
		return 1024;
	}
	RenderText::RenderText()
	{
		m_nFontSize = 9;
		m_nFontStyle = 0;
		g_manager.PushText(this);
		m_realWidth=m_realHeight=0;
		m_imgWidth=m_imgHeight=0;
		m_nTextAlignment = 0;
		m_clip.Set(0,0);
	}
	RenderText::~RenderText()
	{
		g_manager.PopText(this);
	}
	VOID				RenderText::SetFontSize(int nSize)
	{CPUTime(RenderText);
		m_nFontSize = nSize;
	}
	VOID				RenderText::SetFontStyle(int nStyle)
	{CPUTime(RenderText);
		m_nFontStyle = nStyle;
	}
/*
	VOID				RenderText::SetText(const short_t* text)
	{CPUTime(RenderText);
		int len = text?u_strlen(text):0;
		m_text.SetArrayCount(len);
		m_textA.SetArrayCount(len);
		if(len>0)
		{
			if(u_strcmp(text, m_text.GetPtr()) == 0)
				return;
			memcpy(m_text.GetPtr(), text, len*sizeof(short_t));
		}
		if(g_manager.IsDeviceSetup())
			OnResetDevice();
	}*/
    VOID				RenderText::SetFontName(const char* name)
	{
		if(name)
			m_name = name;
		else
			m_name.t[0] = 0;
	}
	VOID				RenderText::SetText(const char* text)
	{CPUTime(RenderText);
		int len = text?strlen(text):0;
		m_text.SetArrayCount(len);
		if(len>0)
			memcpy(m_text.GetPtr(), text, len*sizeof(char));
		if(g_manager.IsDeviceSetup())
			OnResetDevice();
	}
	RenderTextFig::RenderTextFig()
	{
		memset(m_mtrls, 0, sizeof(m_mtrls));
		m_size.Set(1024,1024);
		m_center.Set(0,0);
		m_dtFormat = 0;
		m_bNeedUpdate = false;
		m_numMtrl = 0;
	}
	RenderTextFig::~RenderTextFig()
	{
	}
	BOOL				RenderTextFig::ResetFontFig(RenderTextFig::RTFigRect& fig, short_t ch){CPUTime(RenderTextFig);
		fig.visible = true;
		if(ch == ' '||ch=='\t')
		{
			ch = '_';
			fig.visible = false;
		}
		Font* font	=	g_manager.GetDefaultENFont();
		if(!font)
			return false;
		Font::FontTextInfo* info	=	font->GetTextInfo(ch);
		fig.fontEn = true;
		if(!info)
		{
			Font* cfont	=	g_manager.GetDefaultUnicodeFont();
			if(cfont)
			{
				info	=	cfont->GetTextInfo(ch);
				if(!info)
					info	=	g_manager.GetDefaultENFont()->GetTextInfo('?');
				else
				{
					font = cfont;
					fig.fontEn = false;
					fig.imageIndex = info->imageIndex;
				}
			}
			else
				info	=	g_manager.GetDefaultENFont()->GetTextInfo('?');
			if(!info)
				return false;
		}
		fig.offy	=	info->offY;
		fig.w		=	info->w;
		fig.h		=	info->h+1;
		//fig.x		=	info->x;
		//fig.y		=	info->y;
		fig.channel = info->channel;
		Texture* t	=	font->m_fontImgs[info->imageIndex].t;
		if(!t)
			return false;
		float fTexWidth		=	(float)t->GetWidth();
		float fTexHeight	=	(float)t->GetHeight();
		fig.uv.set((float)info->x / fTexWidth, (float)(info->y-1) / fTexHeight, (float)(info->x+info->w)/fTexWidth, (float)(info->y+info->h+1)/fTexHeight);
		return true;
	}
	VOID				RenderTextFig::SetText(const char* text)//文本内容有改变,设置字体大小和样式之后，必须要重新更新文本才会生效
	{CPUTimeT(RenderTextFig);
		int len = text?strlen(text):0;
		m_text.SetArrayCount(len);
		if(len>0)
			memcpy(m_text.GetPtr(), text, len*sizeof(char));
		m_figs.SetArrayCount(len);
		if(len>0){
			CDynamicArray<short_t> unicode;
			unicode.SetArrayCount(len+1);
			len = Utf8ToWide(m_text.GetPtr(), unicode.GetPtr(), len+1);
			unicode.SetArrayCount(len);
			m_figs.SetArrayCount(len);
			for(int i=0;i<len;i++){
				short_t t = unicode[i];
				RTFigRect& fig = m_figs[i];
				if(t=='\r')
				{
					fig.channel = 102;
					continue;
				}
				else if(t=='\n')
				{
					fig.channel = 101;
					continue;
				}
				fig.color	=	0xffffffff;
				fig.size	=	1;
				fig.visible	=	1;
				if(!ResetFontFig(fig, t)){
					fig.w = 0;
					fig.h = 0;
					fig.visible=false;
				}
			}
		}
		SetFontSize(this->m_nFontSize);
		m_bNeedUpdate = true;
	}
	VOID				RenderTextFig::Update()
	{CPUTimeT(RenderTextFig);
		if(!this->m_bNeedUpdate)return;
		m_bNeedUpdate = false;
		if(m_figs.size()==0){
			m_meshs = 0;
			return;
		}
		unsigned short* indices = new unsigned short[m_figs.size()*2*3+64];
		unsigned char*	attrs = new unsigned char[m_figs.size()*2+64];
		PosNormalColorUV* vertices = new PosNormalColorUV[m_figs.size()*4+64];
		//
#ifdef DIRECT_X_9
		float off	=	0;
		float offT	=	0.5f;
#else
		float off	=	0;
		float offT	=	0;
#endif
		//double fovY	=	g_manager.m_fTan2DFov;
		//double fovX	=	fovY * (double)g_manager.m_bufferWidth / (double)g_manager.m_bufferHeight;
		unsigned int dtFormat = m_dtFormat;
		rect_f drr(0, 0, m_size.cx, m_size.cy);
		int dw = m_size.cx;
		int dh = m_size.cy;
		int tw = m_imgWidth;
		int th = m_imgHeight;
		if(dw!=tw)
		{
			if(dtFormat&Phantom::DrawTextAlign_RIGHT)
				drr.left = drr.right - tw;
			else if(dtFormat&Phantom::DrawTextAlign_CENTER)
				drr.left += (dw-tw)/2;
		}
		if(dh!=th)
		{
			if(dtFormat&Phantom::DrawTextAlign_BOTTOM)
				drr.top = drr.bottom - th;
			else if(dtFormat&Phantom::DrawTextAlign_VCENTER)
				drr.top += (dh-th)/2;
		}
		int maxW = 4096;//
		int maxH = 4096;
		if((dtFormat&DrawTextAlign_SINGLELINE)==0){
			maxW = m_size.cx;
			int numLine = (m_imgWidth / maxW);
			if((m_imgWidth%maxW)>0)
				numLine++;
			drr.top -= (numLine - 1) * m_imgHeight;
			maxH = m_size.cy;
		}
		float3 center(m_center.x, m_center.y, 0);
		float top = drr.top;
		float sleft = drr.left;
		int iPoly = 0;
		m_numMtrl = 0;
		for(int i=0;i<MAX_TEXT_MTRL;i++)
			m_mtrls[i] = 0;
		for(int ifig=0;ifig<m_figs.size();ifig++){
			RTFigRect& fig = m_figs[ifig];
			float figW = (float)fig.w * fig.size;
			float figH = (float)fig.h * fig.size;
			if(fig.channel>2){
				if(fig.channel==101)
				{
					top += m_imgHeight;
					drr.left = sleft;
				}
				continue;
			}
			else if(fig.visible>0){
				drr.right = drr.left + figW;
				if(drr.right > maxW)
				{
					top += m_imgHeight;
					drr.left = sleft;
					drr.right = drr.left + figW;
				}
				drr.top = top + (float)fig.offy * fig.size;
				drr.bottom = drr.top + figH;
				if((int)drr.bottom>maxH)
					break;
				char ucAtt = -1;
				unsigned char iImage = 0;
				if(!fig.fontEn){
					iImage = fig.imageIndex+1;
				}
				for(int i=0;i<m_numMtrl;i++){
					if(m_mtrls[i]==iImage){
						ucAtt = i;
						break;
					}
				}
				if(ucAtt<0){
					ucAtt = m_numMtrl;
					if(m_numMtrl>=MAX_TEXT_MTRL)
						continue;
					m_mtrls[m_numMtrl] = iImage;
					m_numMtrl++;
				}
				unsigned short* ind = indices + iPoly*6;
				int vIndex = iPoly*4;
				ind[0] = vIndex+3;
				ind[1] = vIndex+1;
				ind[2] = vIndex+0;
				ind[3] = vIndex+3;
				ind[4] = vIndex+2;
				ind[5] = vIndex+1;
				PosNormalColorUV* vert = vertices + vIndex;
				vert[0].p.setxyz(drr.left + off, drr.top + off, 0);
				vert[1].p.setxyz(drr.right + off, drr.top + off, 0);
				vert[2].p.setxyz(drr.right + off, drr.bottom + off, 0);
				vert[3].p.setxyz(drr.left + off, drr.bottom + off, 0);
				//for(int i=0;i<4;i++)
				//{
				//	float3& p	=	vert[i].p;
				//	p -= center;
				//	//if(world)
				//	//{
				//	//	p.transform(p, *world);
				//	//}
				//	p.x	=	((p.x / (float)g_manager.m_bufferWidth) - 0.5f) * fovX;
				//	p.y	=	(0.5f - (p.y / (float)g_manager.m_bufferHeight)) * fovY;
				//	p.z	=	0;
				//}
				rect_f& texRc = fig.uv;
				vert[0].tu	=	texRc.left;
				vert[1].tu	=	texRc.right;
				vert[2].tu	=	texRc.right;
				vert[3].tu	=	texRc.left;
				vert[0].tv	=	texRc.top;
				vert[1].tv	=	texRc.top;
				vert[2].tv	=	texRc.bottom;
				vert[3].tv	=	texRc.bottom;
				float fInvW = 1.0f / (float)m_size.cx;
				float fInvH = 1.0f / (float)m_size.cy;
				for(int i=0;i<4;i++)
				{
					vert[i].p -= center;
					vert[i].p.x *= fInvW;
					vert[i].p.y *= fInvH;
					vert[i].diffuse = fig.color;
				}
				unsigned char* att = attrs + iPoly*2;
				for(int i=0;i<2;i++){
					att[i] = ucAtt;
				}
				static	float3	texChannel[3] = {float3(1,0,0), float3(0,1,0), float3(0,0,1)};
				for(int i=0;i<4;i++)
					vert[i].n = texChannel[fig.channel];
				drr.left += figW;
				iPoly++;
			}
		}
		if(iPoly>0){
			MeshPtr temp;
			g_manager.CreateMesh(temp);
			m_meshs = temp;
			m_meshs->Create(iPoly*4, iPoly*2, PosNormalColorUV::FVF);
			m_meshs->WriteVertexBuffer(vertices, iPoly*4, sizeof(PosNormalColorUV));
			m_meshs->WriteIndexBuffer((unsigned short*)indices, iPoly*2*3, attrs);
			m_meshs->Commit();
		}
		delete[] attrs;
		delete[] indices;
		delete[] vertices;
	}
	unsigned int		g_uvScaleIndex = 0;
	void				AddUVScaleIndex()
	{
		g_uvScaleIndex++;
	}
	BOOL				RenderTextFig::DrawText2(const color4& c, const matrix4x4* world, int nHighlight, const float4& uvScale)
	{CPUTimeT(RenderTextFig);
		if(!world)return false;
		Program* currProg = 0;
		currProg = g_manager.SetRenderMode(Phantom::RenderMode_TextChannel, false);
		static _shader_var var_uv_matrix, var_uv_det, var_uv_scale;
		currProg->SetMatrix(var_uv_matrix, world, "sUVMatrix");
		static unsigned int s_renderIdx = 0;
		if(s_renderIdx != g_manager.m_renderIdx){
			s_renderIdx = g_manager.m_renderIdx;
			double fovY	=	g_manager.m_fTan2DFov;
			double fovX	=	fovY * (double)g_manager.m_bufferWidth / (double)g_manager.m_bufferHeight;
			float4 screenDet(g_manager.m_bufferWidth, g_manager.m_bufferHeight, fovX, fovY);
			currProg->SetFloat4(var_uv_det, screenDet, "sScreenDet");
		}
		static unsigned int s_uvScaleIndex = 0;
		if(g_uvScaleIndex != s_uvScaleIndex)
		{
			s_uvScaleIndex = g_uvScaleIndex;
			currProg->SetFloat4(var_uv_scale, uvScale, "sUIScale");
		}
		currProg->SetDiffuse(c);
		g_manager.SetWorldMatrix2D(0);
		for(int i=0;i<m_numMtrl;i++){
			if(m_mtrls[i]==0)
				currProg->SetTexture(TextureIndex_0, g_manager.GetDefaultENFont()->m_fontImgs[0]);
			else if(g_manager.GetDefaultUnicodeFont()){
				currProg->SetTexture(TextureIndex_0, g_manager.GetDefaultUnicodeFont()->m_fontImgs[m_mtrls[i]-1]);
			}
			m_meshs->RenderSubset(currProg, i);
		}
		if(nHighlight > 0)
		{
			for(int hi=0;hi<nHighlight;hi++)
			{
				g_manager->SetBlendMode(Phantom::BlendMode_SRCALPHA, Phantom::BlendMode_ONE);
				for(int i=0;i<m_numMtrl;i++){
					if(m_mtrls[i]==0)
						currProg->SetTexture(TextureIndex_0, g_manager.GetDefaultENFont()->m_fontImgs[0]);
					else if(g_manager.GetDefaultUnicodeFont()){
						currProg->SetTexture(TextureIndex_0, g_manager.GetDefaultUnicodeFont()->m_fontImgs[m_mtrls[i]-1]);
					}
					m_meshs->RenderSubset(currProg, i);
				}
			}
		}
		return true;
	}
	BOOL				RenderTextFig::DrawTexts(const Rect& screen, unsigned int dtFormat, const color4& c, const matrix4x4* world, int nHighlight)
	{CPUTimeT(RenderTextFig);
		int sw = screen.GetWidth();
		int sh = screen.GetHeight();
		this->SetInsideSize(Size(sw, sh), Pixel(0, 0), dtFormat);
		this->Update();
		if(m_meshs==0)
			return false;
		matrix4x4 mat;
		if(world)
			mat = *world;
		else{
			matrixScaling(&mat, screen.GetWidth(), screen.GetHeight(), 0);//mat.identity();
		}
		mat._41 = screen.left;
		mat._42 = screen.top;
		float4 uvScale(1,1,0,0);
		AddUVScaleIndex();
		return DrawText2(c, &mat, nHighlight, uvScale);
	}
	BOOL				RenderTextFig::DrawSprite2D(BOOL bEnabled, const color4& c, const matrix4x4* world, int nHighlight, BOOL bDlgMatrix)
	{CPUTime(RenderTextFig);
		//
        if(bDlgMatrix)
            g_manager->SetCullVisible(CullMode_Double);
        else
            g_manager->SetCullVisible(CullMode_CW);
		this->Update();
		if(m_meshs==0)
			return false;
		float4 uvScale(g_manager.m_uiScale.x,g_manager.m_uiScale.y,g_manager.m_uiOffset.x,g_manager.m_uiOffset.y);
		return DrawText2(c, world, nHighlight, uvScale);
	}
	VOID				RenderTextFig::SetTextSize(unsigned short index, float fSize)
	{
	}
	VOID				RenderTextFig::SetTextColor(unsigned short index, unsigned int color)
	{
	}
	VOID				RenderTextFig::SetTextVisible(unsigned short index, BOOL bVisible)
	{
	}
	VOID				RenderTextFig::SetTextAlpha(unsigned short index, float fAlpha)
	{
	}
	VOID				RenderTextFig::SetInsideSize(const Size& size, const Pixel& center/*中心点基于屏幕的*/, unsigned int dtFormat)
	{CPUTime(RenderTextFig);
		if(m_size.cx==size.cx&&m_size.cy==size.cy&&m_center.x==center.x&&m_center.y==center.y&&m_dtFormat==dtFormat)
			return;
		m_bNeedUpdate = true;
		m_size = size;
		m_center = center;
		m_dtFormat = dtFormat;
	}
	VOID				RenderTextFig::SetFontSize(int nSize)
	{CPUTime(RenderTextFig);
		if(!g_manager.GetDefaultENFont())return;
		m_bNeedUpdate = true;
		this->m_imgWidth = 0;
		this->m_imgHeight = 0;
		this->m_nFontSize = nSize;
		float fWidth = 0;
		for(int i=0;i<m_figs.size();i++){
			float fPixelSize = g_manager.GetDefaultENFont()->GetPixelSize();
			RTFigRect& fig = m_figs[i];
			if(!fig.fontEn && g_manager.GetDefaultUnicodeFont())
				fPixelSize = g_manager.GetDefaultUnicodeFont()->GetPixelSize();
			fig.size	=	(float)nSize / fPixelSize;
			fWidth += (float)m_figs[i].w * fig.size;
			int height = (float)m_figs[i].h * fig.size;
			m_imgHeight = getmax(height, m_imgHeight);
		}
		m_imgWidth = (int)fWidth;
		m_imgHeight++;
	}


	unsigned int		Texture::s_maxImageBuffer = 0;//最大图片数据大小
	BOOL				Texture::s_bSmallImage = false;
	Texture::Texture()
	{
		m_basedir		=	0;
		m_nLoadType		=	0;
		m_bGenMipmap	=	false;
		m_bGenAlpha		=	false;
		m_filePtr		=	0;
		m_loadThread	=	false;
	}
	Texture::~Texture()
	{
		safe_release(m_basedir);
		//LogInfo("system->free texture %s", this->m_fileName.t);
		Destroy();
	}
	void			Texture::Destroy()
	{CPUTime(Texture);
		//close_s(&m_dataPtr);
		m_imgBuffer.FreePixels();
	}

	Texture::ImageFormat	AutoGetFormat(const char* szFile)
	{
		FileName f = szFile;
		f.setIsFile();
		FileName dir, file, ext;
		f.splitpath(&dir, &file, &ext);
		if(ext == ".png")
			return Texture::ImageFormat_Png;
		else if(ext == ".jpg")
			return Texture::ImageFormat_Jpg;
		return Texture::ImageFormat_Unknown;
	}
	TextureRect*	Texture::GetImageMode(const char* szMode)
	{CPUTime(Texture);
		for(int i=0;i<m_rects.size();i++)
			if(m_rects[i].name == szMode)
				return &m_rects[i];
		return 0;
	}
	extern	GenuineGUID g_phantomGUID;
	char*			Texture::OpenFile(int& size)
	{CPUTime(Texture);
		if(m_filePtr)
			close_s(&m_filePtr);
		if(m_basedir)
			m_filePtr = m_basedir->open(m_fileName.t);
		else
			m_filePtr =	open_s(m_fileName, true);
		if(!m_filePtr)
		{
			LogInfo("ImageErr:3:%s", this->m_fileName.t);
			return 0;
		}
		FileName ext;
		m_fileName.splitpath(0, 0, &ext);
		streamRead sr(m_filePtr->getMemoryPtr(), m_filePtr->size());
		char head[4] = {'l','o','c','i'};
		char readh[4];
		BOOL bLock = true;
		sr.read(readh, sizeof(readh));
		for(int i=0;i<4;i++)
			if(head[i] != readh[i])
			{
				bLock = false;
				break;
			}
		if(bLock)
		{
			int version = 1;
			sr >> version;
			int flags = version & 0xffff0000;
			version = version & 0x0000ffff;
			int tableBegin;
			sr >> tableBegin;
			RandomVal rnd;
			EncryptVal val1,val2;
			sr >> rnd;
			sr >> val1;
			if(tableBegin>0)
				sr >> val2;
			if(!DecryptRandomVal(rnd, g_phantomGUID))
			{
				LogInfo("ImageErr:%s", this->m_fileName.t);
				return 0;
			}
			if(!Phantom::DecryptData(m_filePtr->getMemoryPtr() + sr.m_point, 128, rnd, val1))
			{
				LogInfo("ImageErr:1:%s", this->m_fileName.t);
				return 0;
			}
			if(tableBegin > 0){
				if(!Phantom::DecryptData(m_filePtr->getMemoryPtr() + sr.m_point + tableBegin, m_filePtr->size() - tableBegin - sr.m_point, rnd, val2))
				{
					LogInfo("ImageErr:2:%s(%d/%d/%d)", this->m_fileName.t, tableBegin, m_filePtr->size() - tableBegin - sr.m_point, sr.m_point + tableBegin);
					return 0;
				}
				char * szTable = m_filePtr->getMemoryPtr() + tableBegin + sr.m_point;
				LoadRects(szTable, m_filePtr->size() - tableBegin - sr.m_point);
			}
			else
				tableBegin = m_filePtr->size() - sr.m_point;
			size = tableBegin;
			return m_filePtr->getMemoryPtr() + sr.m_point;
		}
		size = m_filePtr->size();
		return m_filePtr->getMemoryPtr();
	}
	VOID			Texture::CloseFile()
	{CPUTime(Texture);
		if(m_filePtr)
			close_s(&m_filePtr);
		m_filePtr = 0;
	}
	VOID			Texture::LoadRects(const char* memoryPtr, int len)
	{CPUTime(Texture);
		const unsigned char* head = (const unsigned char*)memoryPtr;
		if(!(head[0] == 0xef && head[1] == 0xbb && head[2] == 0xbf))
		{
			LogInfo("system->error:%s. rect file(%s) must be a utf8 char code.",utf8("PngMaker之后的rect文件格式必须得是有BOM的utf8格式编码"), this->m_fileName.t);
			return;
		}

		TableTxt* r = new TableTxt();
		if(!r->readFromMemory(memoryPtr+3, len-3))
		{
			delete r;
			return;
		}
		m_rects.clear();
		for(int i=0;i<r->getNumLine();i++)
		{
			const char* szName	=	r->read(i, "name");
			const char* szL		=	r->read(i, "l");
			const char* szT		=	r->read(i, "t");
			const char* szR		=	r->read(i, "r");
			const char* szB		=	r->read(i, "b");
			if(!szName || !szL || !szT || !szR || !szB)
				break;
			const char* szOffX	=	r->read(i, "offx");
			const char* szOffY	=	r->read(i, "offy");
			const char* szOrigW	=	r->read(i, "origW");
			const char* szOrigH	=	r->read(i, "origH");
			TextureRect rect;
			rect.name		=	szName;
			Name file,dir;
			rect.name.splitpath(&dir, &file, 0);
			int len = dir.size();
			for(int j=0;j<len;j++)
			{
				if(dir.t[j] == '\\')
					dir.t[j] = '/';
			}
			//2013-1-3
			if(file == "[1]")
				file = "\"";
			else if(file == "[2]")
				file = "\'";
			else if(file == "[3]")
				file = "|";
			else if(file == "[4]")
				file = "\\";
			else if(file == "[5]")
				file = "/";
			else if(file == "[6]")
				file = "<";
			else if(file == "[7]")
				file = ">";
			else if(file == "[8]")
				file = ";";
			else if(file == "[9]")
				file = ":";
			else if(file == "[10]")
				file = "*";
			else if(file == "[11]")
				file = "?";
			else if(file == "[12]")
				file = ".";
			else if(file == "[13]")
				file = "^";
			else if(file == "[14]")
				file = " ";
			rect.name = dir;
			rect.name += file;

			rect.rc.left	=	atoi(szL);
			rect.rc.top		=	atoi(szT);
			rect.rc.right	=	atoi(szR);
			rect.rc.bottom	=	atoi(szB);
			rect.off.Set(0, 0);
			if(szOffX)
				rect.off.x	=	atoi(szOffX);
			if(szOffY)
				rect.off.y	=	atoi(szOffY);
			if(szOrigW)
				rect.orig.cx	=	atoi(szOrigW);
			else
				rect.orig.cx	=	rect.rc.GetWidth();
			if(szOrigH)
				rect.orig.cy	=	atoi(szOrigH);
			else
				rect.orig.cy	=	rect.rc.GetHeight();
			m_rects.push_back(rect);
		}
		delete r;
	}
	VOID					Texture::GenericAlphas()
	{CPUTime(Texture);
		m_bGenAlpha	=	true;
		if(this->m_bGenAlpha && !m_imgBuffer.alphas)
		{
			if(m_imgBuffer.pixels==0){
				if(!LoadTextureData())
					return;
			}
			if(m_imgBuffer.GenericAlpha())
			{
			}
			m_imgBuffer.FreePixels(false);
		}
	}

	VOID					Texture::OnLoadOk(ImageBuffer& imgBuffer)
	{CPUTimeNone();
		memcpy(&m_imgBuffer, &imgBuffer, sizeof(ImageBuffer));
		memset(&imgBuffer, 0, sizeof(ImageBuffer));
		m_imgBuffer.AutoResize(this->m_fileName.t);
		if((m_nFilter&0x00040000)>0)
			if(m_imgBuffer.GenericAlpha())
				m_bGenAlpha=true;
		this->OnLoadDevice();
	}
	VOID					Texture::LoadTextureData(const char* dataPtr, int dataLength, ImageBuffer& imgBuffer)
	{CPUTimeNone();
#ifndef EXPORT_ENGINE
		int type = ImageFormat_Jpg;
		if(dataPtr[1] == 'P' && dataPtr[2] == 'N' && dataPtr[3] == 'G')
			type = ImageFormat_Png;
		switch(type)
		{
		case ImageFormat_Png:
			{
				PngReader r;
				if(!r.Load(imgBuffer, dataPtr, dataLength, 1))
					return;
			}
			break;
		case ImageFormat_Jpg:
			{
				if(!LoadJpeg(imgBuffer, dataPtr, dataLength))
					return;
			}
			break;
		default:
			break;
		}
		imgBuffer.oriwidth = imgBuffer.width;
		imgBuffer.oriheight = imgBuffer.height;
#endif
	}
	BOOL					Texture::LoadTextureData(BOOL bMy)
	{CPUTime(Texture);
		if(m_loadThread&&bMy){
			//char* data = (char*)malloc(size);
			//memcpy(data, dataPtr, size);
			//g_manager.send(TIDMessage::TID_LoadTexture, data, size, this->GetPtrID().id);
			GetGameMgr()->AddLoadTexture(this);
			return true;
		}
#ifndef EXPORT_ENGINE
		m_imgBuffer.FreePixels();
        int size = 0;
		char* dataPtr = OpenFile(size);
		if(!dataPtr)
			return false;
		if(dataPtr[1] == 'P' && dataPtr[2] == 'N' && dataPtr[3] == 'G')
			m_imageFormat = ImageFormat_Png;
		else
			m_imageFormat = ImageFormat_Jpg;
		//m_loadThread = 0;
		//if(m_loadThread&&bMy){
		//	char* data = (char*)malloc(size);
		//	memcpy(data, dataPtr, size);
		//	g_manager.send(TIDMessage::TID_LoadTexture, data, size, this->GetPtrID().id);
		//}
		//else
		{
			switch(this->m_imageFormat)
			{
			case ImageFormat_Png:
				{
					PngReader r;
					if(!r.Load(m_imgBuffer, dataPtr, size, 1))
					{
						LogInfo("system->error: load texture data failure %s", m_fileName.t);
						CloseFile();
						return false;
					}
				}
				break;
			case ImageFormat_Jpg:
				{
					if(!LoadJpeg(m_imgBuffer, dataPtr, size))
					{
						LogInfo("system->error: load jpeg texture data failure %s", m_fileName.t);
						CloseFile();
						return false;
					}
				}
				break;
			default:
				return false;
			}
		}
		m_imgBuffer.oriwidth = m_imgBuffer.width;
		m_imgBuffer.oriheight = m_imgBuffer.height;
		CloseFile();
		m_imgBuffer.AutoResize(this->m_fileName.t);
		if((m_nFilter&0x00040000)>0)
			if(m_imgBuffer.GenericAlpha())
				m_bGenAlpha=true;
#endif
		return true;
	}
	BOOL			Texture::LoadTextureFromFile(const char* szFile, BOOL bGenMipmap, BOOL bGenAlpha, int nFilter, BOOL bLoadThread, BaseDirectory* basedir)	//从文件读取
	{CPUTime(Texture);
		safe_release(m_basedir);
		m_basedir = basedir;
		safe_addref(m_basedir);
		Destroy();
		InitMyFile(szFile, true, false);
		this->m_loadThread	=	bLoadThread;
		m_nFilter			=	nFilter;
		m_nLoadType			=	0;
		m_fileName			=	szFile;
        m_fileName.setIsFile();
        FileName dir3,file3,ext3;
        m_fileName.splitpath(&dir3,&file3,&ext3);
        m_fileName = dir3.t;
        m_fileName += file3.t;
		if(ext3 == ".jpg")
		{
			m_imageFormat = ImageFormat_Jpg;
			m_fileName += ".jpg";
		}
		else
		{
			m_imageFormat = ImageFormat_Png;
			m_fileName += ".png";
		}
		//close_s(&m_dataPtr);
		//m_dataPtr			=	open_s(szFile, true);
		m_bGenMipmap		=	bGenMipmap;
		m_bGenAlpha			=	bGenAlpha;
		//if(!m_dataPtr)
			//return false;
		m_nLoadType			=	LoadTexture_FromFile;
        LogInfo("system->loadimage(%s)", szFile);
		if(g_manager.IsDeviceSetup())
        {
			OnResetDevice();
        }
		LFileName buf	=	szFile;
		buf.setIsFile();
		LFileName ext;
		LFileName drive, file;
		buf.splitpath(&drive, &file, &ext);
		LFileName path;
		path	=	drive;
		path	+=	file.t;
		path	+=	".rect";
		BOOL bExist	=	false;
		if(fexist(path))
			bExist	=	true;
		if(bExist)
		{
			StreamBase* s	=	0;
			if(m_basedir)
				s = m_basedir->open(path);
			else
				s = open_s(path);
			if(s)
			{
				char* szBuffer = new char[s->size()+1];
				szBuffer[s->size()] = 0;
				int length = s->size();
				if(s->read(szBuffer,s->size()))
				{
#ifdef WIN32_OS
					const unsigned char* head = (const unsigned char*)szBuffer;
					if(!(head[0] == 0xef && head[1] == 0xbb && head[2] == 0xbf))
					{
						LogInfo("system->error:%s. rect file(%s) must be a utf8 char code.",utf8("PngMaker之后的rect文件格式必须得是有BOM的utf8格式编码,将被强制改成utf8,重置即可正常"), this->m_fileName.t);
						CDynamicArray<char> buf;
						Phantom::AsciiToUtf8(buf, szBuffer);
						SaveUtf8(path, buf.GetPtr());
					}
#endif
					LoadRects(szBuffer, s->size());
				}
				close_s(&s);
			}
		}
#ifdef WIN32_OS//分解所有的图片
		//ExportPngs(szFile, this);
#endif
		return true;
	}
	RenderTarget::RenderTarget()
	{
		m_bInit			=	false;
		m_rtFormat		=	0;
		g_manager.PushRT(this);
	}
	RenderTarget::~RenderTarget(){
		g_manager.PopRT(this);
	}
	BOOL			RenderTarget::Create(int width, int height, RTFormat format)
	{CPUTime(RenderTarget);
		m_bInit		=	true;
		m_imgBuffer.width = width;
		m_imgBuffer.height = height;
		m_imgBuffer.nDiv = 1;
		m_rtFormat	=	format;
		if(g_manager.IsDeviceSetup())
			OnResetDevice();
		return true;
	}
	VOID			RenderTarget::Destroy()
	{CPUTime(RenderTarget);
		m_bInit	=	false;
		OnLostDevice();
	}
	DeviceMesh::DeviceMesh()
	{
		m_nStrideSize		=	0;
		m_nNumIndices = 0; m_nNumVertices = 0;
		m_elements.setGrow(0);
	}
	DeviceMesh::~DeviceMesh()
	{
		Destroy();
	}
	int				DeviceMesh::GetElementWidth()
	{CPUTime(DeviceMesh);
		WORD offset			=	0;
		for(int i=0;i<this->m_elements.size();i++)
		{
			if(m_elements[i].type < 0  || m_elements[i].type > VertexElement::VE_TYPE_SHORT4)return false;
			switch(m_elements[i].type)
			{
			case VertexElement::VE_TYPE_FLOAT1:
				offset	+=	sizeof(float);
				break;
			case VertexElement::VE_TYPE_FLOAT2:
				offset	+=	sizeof(float) * 2;
				break;
			case VertexElement::VE_TYPE_FLOAT3:
				offset	+=	sizeof(float) * 3;
				break;
			case VertexElement::VE_TYPE_FLOAT4:
				offset	+=	sizeof(float) * 4;
				break;
			case VertexElement::VE_TYPE_D3DCOLOR:
				offset	+=	sizeof(unsigned int);
				break;
			case VertexElement::VE_TYPE_UBYTE4:
				offset	+=	sizeof(char) * 4;
				break;
			case VertexElement::VE_TYPE_SHORT2:
				offset	+=	sizeof(short) * 2;
				break;
			case VertexElement::VE_TYPE_SHORT4:
				offset	+=	sizeof(short) * 4;
				break;
			default:
				return 0;
			}
		}
		return offset;
	}
	VOID			DeviceMesh::Create(INT nNumVertices, INT nNumFaces, VertexElement* elements, int elementCount)
	{CPUTime(DeviceMesh);
		Destroy();
		SetVertexDecl(elements, elementCount);
		m_nStrideSize	=	GetElementWidth();
		m_nNumIndices	=	nNumFaces * 3;
		m_nNumVertices	=	nNumVertices;
		if(m_nNumVertices > 0)
			m_vertices.SetArrayCount(m_nNumVertices * m_nStrideSize);
		if(m_nNumIndices > 0)
		{
			m_indices.SetArrayCount(m_nNumIndices * sizeof(WORD));
			m_attributes.SetArrayCount(nNumFaces);
		}
	}

	VOID	AddElement(VertexElement* ele, int& eleIndex, int type, int usage, int usageIndex)
	{
		VertexElement& el = ele[eleIndex];
		eleIndex++;
		//
		el.type			=	type;
		el.method		=	0;
		el.usage		=	usage;
		el.usageIndex	=	usageIndex;
	}
	int	FillElemenetByFVF(VertexElement* ele, INT nFVF)
	{
		int	eleIndex	=	0;
		if(nFVF & FVF_XYZ)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_FLOAT3, VertexElement::VE_USAGE_POSITION, 0);
		if(nFVF & FVF_NORMAL)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_FLOAT3, VertexElement::VE_USAGE_NORMAL, 0);
		if(nFVF & FVF_DIFFUSE)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_D3DCOLOR, VertexElement::VE_USAGE_COLOR, 0);
		//
		BOOL t1 = false;
		BOOL t2 = false;
		BOOL t3 = false;
		BOOL t4 = false;
		BOOL t5 = false;
		if((nFVF & FVF_TEX5) == FVF_TEX5)
			t1 = t2 = t3 = t4=t5=true;
		else if((nFVF & FVF_TEX4) == FVF_TEX4)
			t1 = t2 = t3 = t4=true;
		else if((nFVF & FVF_TEX3) == FVF_TEX3)
			t1 = t2 = t3 = true;
		else if((nFVF & FVF_TEX2) == FVF_TEX2)
			t1 = t2 = true;
		else if((nFVF & FVF_TEX1) == FVF_TEX1)
			t1	=	true;
		if(t1)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 0);
		if(t2)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 1);
		if(t3)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 2);
		if(t4)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 3);
		if(t5)
			AddElement(ele, eleIndex, VertexElement::VE_TYPE_FLOAT2, VertexElement::VE_USAGE_TEXCOORD, 4);
		return eleIndex;
	}
	VOID			DeviceMesh::Create(INT nNumVertices, INT nNumFaces, INT nFVF)
	{CPUTime(DeviceMesh);
		VertexElement temp[32];
		int count = FillElemenetByFVF(temp, nFVF);
		Create(nNumVertices, nNumFaces, temp, count);
	}
	VOID			DeviceMesh::SetVertexFVF(INT nFVF)
	{CPUTime(DeviceMesh);
		VertexElement temp[32];
		int count = FillElemenetByFVF(temp, nFVF);
		SetVertexDecl(temp, count);
	}
	VOID			DeviceMesh::SetVertexDecl(VertexElement* elements, int elementCount)
	{CPUTime(DeviceMesh);
		m_elements.SetArrayCount(elementCount);
		if(m_elements.size() > 0)
			memcpy(m_elements.GetPtr(), elements, sizeof(VertexElement) * elementCount);
		this->m_nStrideSize = this->GetElementWidth();
	}
	void			DeviceMesh::Destroy()
	{CPUTime(DeviceMesh);
		m_attributes.clear();
		m_vertices.clear();
		m_indices.clear();
		m_elements.clear();
	}

	VOID			DeviceMesh::WriteVertexBuffer(const void* vertexPtr, int vertexCount, int strideSize)
	{CPUTime(DeviceMesh);
		assert(vertexCount * strideSize == this->m_nNumVertices * this->m_nStrideSize);
		memcpy_s(this->m_vertices.GetPtr(), vertexPtr, m_nNumVertices * this->m_nStrideSize);
	}
	VOID			DeviceMesh::Commit()	//重新刷新设备缓冲区
	{CPUTime(DeviceMesh);
		if(!g_manager.IsDeviceSetup())
			return;
		this->OnResetDevice();
	}
	VOID			DeviceMesh::WriteIndexBuffer(const unsigned short* indexPtr, int indexCount, unsigned char* attrib)
	{CPUTime(DeviceMesh);
		if(m_nNumIndices == 0)return;
		assert(indexCount == this->m_nNumIndices);
		memcpy_s(this->m_indices.GetPtr(), indexPtr, indexCount * sizeof(unsigned short));
		//
		if(attrib)
		{
			m_attributes.SetArrayCount(indexCount / 3);
			if(m_attributes.size() > 0)
				memcpy_s(m_attributes.GetPtr(), attrib, m_attributes.size());
		}
	}
	Device::Device()
	{
		m_nFVF		=	0;
		m_nAlphaMode		=	Phantom::AlphaMode_None;
		m_nSrcBlend			=	Phantom::BlendMode_ZERO;
		m_nDestBlend		=	Phantom::BlendMode_ZERO;
		m_nCullMode			=	Phantom::CullMode_UNKNOWN;
		m_bEnableZ			=	false;
		m_numTriangles		=	0;
		m_fDepth			=	1.0f;
		m_windParameter.dir.setxyz(0, 0, 0);
		m_windParameter.fTime	=	0;
		m_fLODViewDelta			=	1.0f;
		m_bZWriteEnable			=	false;
		m_fLODMinVisible		=	To3DValue(20.0f);
		m_fAlphaContract	=	1.5f;
		this->m_clearColor.setRGBA(0,0,0,0);
	}
	VOID					Device::ResetAllDefault()
	{CPUTime(Device);
		m_nSrcBlend		=	Phantom::BlendMode_UNKNOWN;
		m_nDestBlend	=	Phantom::BlendMode_UNKNOWN;
		m_nCullMode		=	Phantom::CullMode_UNKNOWN;
		m_nAlphaMode	=	AlpahMode_Unknown;
		m_bZWriteEnable	=	-1;
		m_bEnableZ		=	-1;
	}
	Program::Program(int mode)
	{
		m_lastDiffuse = 0;
		m_nRenderMode = mode;
		m_resetID = 0;
		m_bCreated = false;
		m_bFail = false;
		m_nMVP	=	0;
		m_bBlendMode	=	false;
		m_nProgramType	=	0;
		m_bBlendMode	=	0;
		m_renderIdx		=	0;
		m_cameraViewIdx	=	0;
		m_shadowMapIdx	=	0;
		m_toShadowMapIdx=	0;
		m_lastEmissive	=	0;
		m_lastBoneMat	=	0;
		m_lastWorldMat	=	0;
		m_bLightSeted = 0;
		m_bLightDirInv = 0;
	}
	Program::~Program()
	{
	}
	VOID				Program::SetProgramType(INT nType, BOOL bBlendMode)
	{CPUTime(Program);
		m_bBlendMode	=	(bBlendMode==TRUE);
		m_nProgramType	=	nType;
	}
	VOID				Program::SetFileName(const char* szFile, BOOL bBlendMode)
	{CPUTime(Program);
		int len = strlen(szFile);
		m_fileName.resize(len + 1);
		strcpy(m_fileName.GetPtr(), szFile);
		FileName f = szFile;
		f.setIsFile();
		FileName fname;
		f.splitpath(0, &fname, 0);
		m_name			=	fname.t;
		m_nProgramType	=	0;
		m_bBlendMode	=	(bBlendMode==TRUE);
	}
	struct	DefineV
	{
		DefineV(const char* s){name = s; len = name.size();}
		Name			name;
		int				len;
	};
	BOOL				IsText(const char* szSrc, const DefineV& v, int& i)
	{
		if(strnicmp(szSrc, v.name.t, v.len) == 0)
		{
			i	+=	v.len;
			if(szSrc[v.len] == '\r')
				i++;
			return true;
		}
		return false;
	}
	VOID				InsertLine(CDynamicArray<char>& v, int & index, const char* szText)
	{
		int len	=	strlen(szText);
		for(int i=0;i<len;i++)
		{
			v[index++] = szText[i];
		}
		v[index++]	=	'\r';
		v[index++]	=	'\n';
	}
	VOID				Program::OnResetDevice()
	{CPUTime(Program);
		if(m_nProgramType==0 && m_fileName.size()>0){
			StreamBase* b	=	open_s(m_fileName.GetPtr());
			if(b)
			{
				if(!OpenShaderText(b->getMemoryPtr(), b->size()))
				{
					LogInfo("system->error:program(%s)", m_fileName.GetPtr());
				}
				close_s(&b);
			}
		}
		else if(m_nProgramType>0){
			OnCreateShader();
		}
	}
	BOOL				Program::OpenShaderText(const char* szText, int len)
	{CPUTime(Program);
		int type	=	0;
		DefineV DirectX9		(	"/*[DirectX9]*/");
		DefineV DirectX9_End	(	"/*[/DirectX9]*/");
		DefineV OpenGLES_V		(	"/*[OpenglES_Vertex]*/");
		DefineV OpenGLES_V_End	(	"/*[/OpenglES_Vertex]*/");
		DefineV OpenGLES_P		(	"/*[OpenglES_Pixel]*/");
		DefineV OpenGLES_P_End	(	"/*[/OpenglES_Pixel]*/");
		//
		DefineV VInput		(	"/*[Blend]*/");
		DefineV PosNormal	(	"/*[BlendPosNormal]*/");
		//
		m_vsps_directx9.SetArrayCount(len + 5000);
		m_vsps_opengles_v.SetArrayCount(len + 5000);
		m_vsps_opengles_p.SetArrayCount(len + 5000);
		int vsps_directx9	=	0;
		int vsps_opengles_v	=	0;
		int vsps_opengles_p	=	0;
		BOOL bVsps_directx9		=	false;
		BOOL bVsps_opengles_v	=	false;
		BOOL bVsps_opengles_p	=	false;
		//
		int begin	=	0;
		for(int i=0;i<len;i++)
		{
			if(szText[i] == '/' && szText[i + 1] == '*')
			{
				const char* szFirst		=	szText + i;
				if(IsText(szFirst, DirectX9, i))
				{
					bVsps_directx9		=	true;
					if(m_bBlendMode)
					{
						char buf[128];
						sprintf(buf, "matrix sBones[%d];", PROGRAM_BONE_COUNT);
						InsertLine(m_vsps_directx9, vsps_directx9, buf);
					}
				}
				else if(IsText(szFirst, DirectX9_End, i))
					bVsps_directx9		=	false;
				else if(IsText(szFirst, OpenGLES_V, i))
					bVsps_opengles_v	=	true;
				else if(IsText(szFirst, OpenGLES_V_End, i))
					bVsps_opengles_v	=	false;
				else if(IsText(szFirst, OpenGLES_P, i))
					bVsps_opengles_p	=	true;
				else if(IsText(szFirst, OpenGLES_P_End, i))
					bVsps_opengles_p	=	false;
				else if(IsText(szFirst, VInput, i))//添加声明
				{
					if(m_bBlendMode)
					{
						if(bVsps_directx9)
						{
							InsertLine(m_vsps_directx9, vsps_directx9, "float4 vIdx	: COLOR1;");
							InsertLine(m_vsps_directx9, vsps_directx9, "float2 vW0	: TEXCOORD2;");
							InsertLine(m_vsps_directx9, vsps_directx9, "float2 vW1	: TEXCOORD3;");
						}
						else if(bVsps_opengles_v)
						{
							InsertLine(m_vsps_opengles_v, vsps_opengles_v, "attribute vec4	inColor1;");
							InsertLine(m_vsps_opengles_v, vsps_opengles_v, "attribute vec2	inTexcoord2;");
							InsertLine(m_vsps_opengles_v, vsps_opengles_v, "attribute vec2	inTexcoord3;");
							char buf[128];
							sprintf(buf, "uniform mat4	sBones[%d];", PROGRAM_BONE_COUNT);
							InsertLine(m_vsps_opengles_v, vsps_opengles_v, buf);
						}
					}
				}
				else if(IsText(szFirst, PosNormal, i))//添加声明
				{
					if(bVsps_directx9 && m_bBlendMode)
					{
						InsertLine(m_vsps_directx9, vsps_directx9, "float4  fBI		=	D3DCOLORtoUBYTE4(input.vIdx);");
						InsertLine(m_vsps_directx9, vsps_directx9, "float4x4 m0 	= 	sBones[	fBI[0] ]	*	input.vW0.x;");
						InsertLine(m_vsps_directx9, vsps_directx9, "float4x4 m1 	= 	sBones[	fBI[1] ]	*	input.vW0.y;");
						InsertLine(m_vsps_directx9, vsps_directx9, "float4x4 m2 	= 	sBones[	fBI[2] ]	*	input.vW1.x;");
						InsertLine(m_vsps_directx9, vsps_directx9, "float4x4 m3 	= 	sBones[	fBI[3] ]	*	input.vW1.y;");
						InsertLine(m_vsps_directx9, vsps_directx9, "float4 fm1		=	float4(m0[0][0] + m1[0][0] + m2[0][0] + m3[0][0] ,	m0[1][0] + m1[1][0] + m2[1][0] + m3[1][0] ,	m0[2][0] + m1[2][0] + m2[2][0] + m3[2][0] ,	m0[3][0] + m1[3][0] + m2[3][0] + m3[3][0] );");
						InsertLine(m_vsps_directx9, vsps_directx9, "float4 fm2		=	float4(m0[0][1] + m1[0][1] + m2[0][1] + m3[0][1] ,	m0[1][1] + m1[1][1] + m2[1][1] + m3[1][1] ,	m0[2][1] + m1[2][1] + m2[2][1] + m3[2][1] ,	m0[3][1] + m1[3][1] + m2[3][1] + m3[3][1] );");
						InsertLine(m_vsps_directx9, vsps_directx9, "float4 fm3		=	float4(m0[0][2] + m1[0][2] + m2[0][2] + m3[0][2] ,	m0[1][2] + m1[1][2] + m2[1][2] + m3[1][2] ,	m0[2][2] + m1[2][2] + m2[2][2] + m3[2][2] ,	m0[3][2] + m1[3][2] + m2[3][2] + m3[3][2] );");
						InsertLine(m_vsps_directx9, vsps_directx9, "input.Pos		=	float4(dot(input.Pos.xyz,	fm1.xyz)	+	fm1.w,	dot(input.Pos.xyz,	fm2.xyz)	+	fm2.w,	dot(input.Pos.xyz,	fm3.xyz)	+	fm3.w, 1);");
						InsertLine(m_vsps_directx9, vsps_directx9, "input.Normal	=	float3(dot(input.Normal,	fm1.xyz),	dot(input.Normal,	fm2.xyz),	dot(input.Normal,	fm3.xyz));");
					}
					else if(bVsps_opengles_v && m_bBlendMode)
					{
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "ivec4 fBI		=	ivec4(inColor1 * 255.0);");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "mat4 m0 		= 	sBones[	fBI[0] ]	*	inTexcoord2.x;");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "mat4 m1 		= 	sBones[	fBI[1] ]	*	inTexcoord2.y;");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "mat4 m2 		= 	sBones[	fBI[2] ]	*	inTexcoord3.x;");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "mat4 m3 		= 	sBones[	fBI[3] ]	*	inTexcoord3.y;");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "vec4 fm1		=	vec4(m0[0][0] + m1[0][0] + m2[0][0] + m3[0][0] ,	m0[1][0] + m1[1][0] + m2[1][0] + m3[1][0] ,	m0[2][0] + m1[2][0] + m2[2][0] + m3[2][0] ,	m0[3][0] + m1[3][0] + m2[3][0] + m3[3][0] );");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "vec4 fm2		=	vec4(m0[0][1] + m1[0][1] + m2[0][1] + m3[0][1] ,	m0[1][1] + m1[1][1] + m2[1][1] + m3[1][1] ,	m0[2][1] + m1[2][1] + m2[2][1] + m3[2][1] ,	m0[3][1] + m1[3][1] + m2[3][1] + m3[3][1] );");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "vec4 fm3		=	vec4(m0[0][2] + m1[0][2] + m2[0][2] + m3[0][2] ,	m0[1][2] + m1[1][2] + m2[1][2] + m3[1][2] ,	m0[2][2] + m1[2][2] + m2[2][2] + m3[2][2] ,	m0[3][2] + m1[3][2] + m2[3][2] + m3[3][2] );");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "vec4 pos		=	vec4(dot(inPosition.xyz,	fm1.xyz)	+	fm1.w,	dot(inPosition.xyz,	fm2.xyz)	+	fm2.w,	dot(inPosition.xyz,	fm3.xyz)	+	fm3.w, 1);");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "vec3 Normal		=	vec3(dot(inNormal,	fm1.xyz),	dot(inNormal,	fm2.xyz),	dot(inNormal,	fm3.xyz));");
					}
					else if(bVsps_opengles_v && !m_bBlendMode)
					{
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "vec4 pos		=	inPosition;");
						InsertLine(m_vsps_opengles_v, vsps_opengles_v, "vec3 Normal		=	inNormal;");
					}
				}
				else
				{
					i	+=	2;
					while(i < len)
					{
						if(szText[i] == '*' && szText[i + 1] == '/')
						{
							i++;
							break;
						}
						i++;
					}
				}
			}
			else
			{
				if(szText[i] == '/' && szText[i + 1] == '/')
				{
					while(i < len)
					{
						if(szText[i + 1] == '\r' || szText[i + 1] == 0)
							break;
						i++;
					}
					continue;
				}
				//
				if(bVsps_directx9)
					m_vsps_directx9[vsps_directx9++]		=	szText[i];
				else if(bVsps_opengles_v)
					m_vsps_opengles_v[vsps_opengles_v++]	=	szText[i];
				else if(bVsps_opengles_p)
					m_vsps_opengles_p[vsps_opengles_p++]	=	szText[i];
			}
		}
		m_vsps_directx9.SetArrayCount(vsps_directx9);
		m_vsps_opengles_v.SetArrayCount(vsps_opengles_v);
		m_vsps_opengles_p.SetArrayCount(vsps_opengles_p);
		return true;
	}
	void				Program::SetDiffuse(const color4& color)
	{CPUTime(Program);
		unsigned int diffuse = color;
		if(m_lastDiffuse == diffuse)
			return;
		m_lastDiffuse = diffuse;
		if(!m_h_materialDiffuse.is_findFail())
			this->SetFloat4(m_h_materialDiffuse, (const float4&)color, "sDiffuse");
	}
	void				Program::SetCameraValue(const float3& _eye, const float3& _target, const float3& _dir)
	{CPUTime(Program);
	}
	void				Program::SetAmbientColor(const color4& color)
	{CPUTime(Program);
		if(!m_h_materialAmbient.is_findFail())
			this->SetFloat4(m_h_materialAmbient, (const float4&)color, "sAmbient");
	}
	void				Program::SetEmissiveColor(const color4& color)
	{CPUTime(Program);
		//unsigned int emi = color;
		//if(m_lastEmissive == emi)
		//	return;
		//m_lastEmissive = emi;
		if(!m_h_materialEmissive.is_findFail())
			this->SetFloat3(m_h_materialEmissive, (const float3&)color, "sEmissive");
	}
	void				Program::SetSpecular(const color4& color,float fPower)
	{CPUTime(Program);
		if(!m_h_materialSpecular.is_findFail())
			this->SetFloat4(m_h_materialSpecular, (const float4&)color, "sSpecular");
	}
	void				Program::SetElapsedTime(float fElapsedTime)
	{CPUTime(Program);
		if(!m_h_elapsedTime.is_findFail())
			this->SetFloat(m_h_elapsedTime, fElapsedTime, "sElapsedTime");
	}
	void				Program::SetTime(float fTime)
	{CPUTime(Program);
		if(!m_h_time.is_findFail())
			this->SetFloat(m_h_time, fTime, "sTime");
	}
	//
	void				Program::SetSkyColor(const color4& color)
	{CPUTime(Program);
		if(!m_h_skyColor.is_findFail())
			this->SetFloat4(m_h_skyColor, (const float4&)color, "sTime");
	}
	void				Program::SetLightMap(Texture* t)
	{CPUTime(Program);
		this->SetTexture(TextureIndex_1, t);
	}
	void				Program::SetSpecularMap(Texture* t)
	{CPUTime(Program);
		this->SetTexture(TextureIndex_2, t);
	}
	void				Program::SetReflectMap(Texture* t)
	{CPUTime(Program);
		this->SetTexture(TextureIndex_3, t);
	}
	void				Program::SetBillboardMatrix(const matrix4x4* mat)	//设置世界矩阵
	{CPUTime(Program);
		if(!m_h_billboardMat.is_findFail() && mat)
			this->SetMatrix(m_h_billboardMat, mat, "sBillboardMat");
	}
	void				Program::SetNormalMap(Texture* t)
	{CPUTime(Program);
		this->SetTexture(TextureIndex_4, t);
	}
	void				Program::SetScreenMap(Texture* t)
	{CPUTime(Program);
		this->SetTexture(TextureIndex_1, t);
	}
	void				Program::SetWorldMatrix(const matrix4x4* mat, BOOL bHasNormalMap)
	{CPUTime(Program);
		if(m_lastWorldMat==mat)
			return;
		m_lastWorldMat	=	mat;
		if(mat)
		{
			if(!m_h_worldViewTranspose.is_findFail())
			{
				matrix4x4 worldView;
				matrixMultiply(&worldView, mat, &GetCameraPtr()->GetViewMatrix());
				matrix4x4 inv;
				matrixInverse(&inv, 0, &worldView);
				matrixTranspose(&inv, &inv);
				SetMatrix(m_h_worldViewTranspose, &inv, "sWorldViewIT");
			}
			if(bHasNormalMap && !m_h_worldInverse.is_findFail())
			{
				matrix4x4 matInv, worldInverseTranspose;
				matrixInverse(&matInv, 0, mat);
				matrixTranspose(&worldInverseTranspose, &matInv);
				SetMatrix(m_h_worldInverse, &worldInverseTranspose, "sWorldInv");
			}
			if(!m_h_world.is_findFail())
				SetMatrix(m_h_world, mat, "sWorld");
		}
		if(!m_varMVP.is_findFail())
		{
			if(mat)
			{
				matrix4x4 temp;
				matrixMultiply(&temp, mat, &GetCameraPtr()->GetViewProjMatrix());
				SetMatrix(m_varMVP, &temp, "MVP");
			}
			else if(m_nMVP != 1)
			{
				m_nMVP	=	1;
				SetMatrix(m_varMVP, &GetCameraPtr()->GetViewProjMatrix(), "MVP");
			}
		}
	}
	//
	VOID				Program::ForceCommitMatrixToShaders()
	{CPUTime(Program);
		m_renderIdx = 0;
		CommitMatrixToShaders();
	}
	void				Program::SetLight(unsigned char lightCount, float4* posTypelist, float4* colorlist, float4* range_angles)
	{CPUTime(Program);
		if(m_h_lightCount.is_findFail())return;
		if(lightCount>0){
			if(posTypelist)
				this->SetFloat4Ptr(m_h_lightPos, posTypelist, lightCount, "sLightPos");
			if(colorlist)
				this->SetFloat4Ptr(m_h_lightColor, colorlist, lightCount, "sLightColor");
			if(range_angles)
				this->SetFloat4Ptr(m_h_lightRangeAngle, range_angles, lightCount, "sLightRangeA");
		}
		if(m_lastLightCount!=lightCount){
			this->SetInt(m_h_lightCount, m_lastLightCount=lightCount, "sLightCount");
		}
	}
	VOID				Program::CommitToShadowMap()
	{CPUTime(Program);
		ShadowMap& smap = g_manager.m_shadowMap;
		if(m_toShadowMapIdx!=smap.m_shadowUpdateIdx&&smap.GetRenderTarget()){
			m_toShadowMapIdx=smap.m_shadowUpdateIdx;
			if(!m_h_m_shadowEpsilon.is_findFail())
				SetFloat(m_h_m_shadowEpsilon, g_manager.m_shadowEpsilon, "m_shadowEpsilon");
		}
	}
	VOID				Program::CommitShadowMap()
	{CPUTime(Program);
		ShadowMap& smap = g_manager.m_shadowMap;
		if(m_shadowMapIdx!=smap.m_shadowUpdateIdx&&smap.GetRenderTarget()){
			m_shadowMapIdx=smap.m_shadowUpdateIdx;
			float fWidth = smap.GetShadowMapSize();
			if(!m_h_shadowTextureSize.is_findFail())
				SetFloat2(m_h_shadowTextureSize, float2(1.0f / fWidth, 1.0f / fWidth), "shadowTextureSize");
			if(!m_h_ViewToLightProj.is_findFail())
				SetMatrix(m_h_ViewToLightProj, &smap.m_toLightProj, "sViewToLightProj");
			//if(!m_h_m_shadowEpsilon.is_findFail())
				//SetFloat(m_h_m_shadowEpsilon, g_manager.m_shadowEpsilon, "m_shadowEpsilon");
			SetTexture(TextureIndex_5, smap.GetRenderTarget());
		}
	}
	void				Program::SetLightDirInv(BOOL bInv)
	{
		if(!m_bLightSeted)return;
		if(m_bLightDirInv==bInv)return;
		m_bLightDirInv = bInv;
		if(m_bLightDirInv)
			SetFloat3(m_h_lightDirectionView, -g_manager.m_lightDir, "sLightDirect");
		else
			SetFloat3(m_h_lightDirectionView, -g_manager.m_lightDir, "sLightDirect");
	}
	VOID				Program::CommitMatrixToShaders()
	{CPUTime(Program);
		bool bCommitCamera = false;
		if(m_renderIdx!=g_manager.m_renderIdx){
			CPUTimeN(ProgramRIdx,1);
			m_renderIdx=g_manager.m_renderIdx;
			bCommitCamera = true;
			//
			SetElapsedTime(g_manager.GetElapsedTime());
			SetTime(g_manager.GetAppTime());
			if(!m_h_quadScreenSize.is_findFail())
			{
				float2 s(g_manager.GetBufferWidth(), g_manager.GetBufferHeight());
				SetFloat2(m_h_quadScreenSize, s, "sScreenSize");
			}
			this->SetTexture(Phantom::TextureIndex_5, 0);
			if(!m_h_lightCount.is_findFail())
				this->SetInt(m_h_lightCount, m_lastLightCount=0, "sLightCount");
			SetWindParameter(g_manager->m_windParameter);
			m_nMVP			=	0;
			if(!m_h_brightnass.is_findFail())
				SetFloat(m_h_brightnass, g_manager.m_fBrightness, "sBrightness");
			if(!m_h_constrast.is_findFail())
				SetFloat(m_h_constrast, g_manager.m_fContrast, "sContrast");
			if(!m_h_alphaConstrast.is_findFail())
				SetFloat(m_h_alphaConstrast, g_manager->m_fAlphaContract, "sAlphaConstrast");
			if(!m_h_shadowDensity.is_findFail())
				SetFloat(m_h_shadowDensity, g_manager.m_shadowDensity, "shadowDensity");
			if(!m_h_fogColor.is_findFail())
				SetFloat3(m_h_fogColor, g_manager.m_fogColor, "sFogColor");
			if(!m_h_materialAmbient.is_findFail())
				SetFloat3(m_h_materialAmbient, g_manager.m_ambientColor, "sAmbient");
			if(!m_h_fogRange.is_findFail())
				SetFloat2(m_h_fogRange, float2(g_manager.m_fogNear, g_manager.m_fogFar-g_manager.m_fogNear), "sFogRange");
			m_bLightDirInv = false;
			m_bLightSeted = false;
			if(!m_h_lightDirectionViewEnable.is_findFail())
			{
				SetInt(m_h_lightDirectionViewEnable, g_manager.m_lightEnable, "bLightDirect");
				if(g_manager.m_lightEnable){
					m_bLightSeted = true;
					SetFloat3(m_h_lightDirectionView, g_manager.m_lightDir, "sLightDirect");
					SetFloat3(m_h_lightDirectionViewColor, g_manager.m_lightColor, "sLightDirectC");
				}
			}
			m_lastEmissive = 0;
			m_lastBoneMat = 0;
			m_lastWorldMat = 0;
			m_lastDiffuse = 0;
			m_lastLightCount = 0;
		}
		if(bCommitCamera || m_cameraViewIdx!=GetCameraPtr()->m_cameraViewIdx){
			CPUTimeN(ProgramRIdx,2);
			m_lastWorldMat	=	0;
			m_cameraViewIdx = GetCameraPtr()->m_cameraViewIdx;
			const matrix4x4& viewMat			=	GetCameraPtr()->GetViewMatrix();
			const matrix4x4& viewProjMat		=	GetCameraPtr()->GetViewProjMatrix();
			if(!m_h_fogPos.is_findFail())
				SetFloat3(m_h_fogPos, g_manager.GetFogPos(), "sFogPos");
			if(!m_h_cameraEye.is_findFail())
				this->SetFloat3(m_h_cameraEye, GetCameraPtr()->GetEye(), "sCameraEye");
			if(!m_h_cameraTarget.is_findFail())
				this->SetFloat3(m_h_cameraTarget, GetCameraPtr()->GetTarget(), "sCameraTarget");
			if(!m_h_cameraDir.is_findFail())
			{
				float3 dir	=	GetCameraPtr()->GetTarget() - GetCameraPtr()->GetEye();
				this->SetFloat3(m_h_cameraDir, dir, "sCameraDir");
			}
			if(!m_hScreenMatrix.is_findFail())
				SetMatrix(m_hScreenMatrix, &GetCameraPtr()->GetScreenViewProjMat(), "sScreenMat");
			if(!m_h_viewProjection.is_findFail())
				SetMatrix(m_h_viewProjection, &GetCameraPtr()->GetViewProjMatrix(), "sViewProj");
			m_nMVP			=	0;
		}
	}
	void				Program::SetWindParameter(const PWindParameter& wind)
	{CPUTime(Program);
		if(!m_h_windDirection.is_findFail())
			this->SetFloat4(m_h_windDirection, (float4&)wind, "sWind");
	}
	void				Program::SetBoneMatrixs(const matrix4x4* bones, unsigned int numofBone)
	{CPUTime(Program);
		if(m_lastBoneMat==bones)
			return;
		m_lastBoneMat = bones;
		if(!m_h_boneList.is_findFail())
			this->SetMatrixPtr(m_h_boneList, bones, getmin(numofBone, PROGRAM_BONE_COUNT), "sBones");
	}
	//

#define		MAX_FVF_DECL_SIZE	65

	BOOL	LoadMesh(StreamBase* stream, MeshPtr& mesh)
	{
		D3DVertexElement	Declaration[MAX_FVF_DECL_SIZE];
		DWORD	dwVertexDataLength;
		DWORD	dwFaceDataLength;
		DWORD	dwAttributeLength;
		DWORD	dwVertexWidth;
		DWORD	dw32Bits;
		int head	=	_MakeFileHead('m','e','s','h');
		int headRead;
		stream->read(&headRead, sizeof(int));
		if(head != headRead)
		{
			return false;
		}
		DWORD dwRangeRead = 0;
		stream->read(&dwVertexDataLength, sizeof(DWORD));
		stream->read(&dwFaceDataLength, sizeof(DWORD));
		stream->read(&dwAttributeLength, sizeof(DWORD));
		stream->read(&dwVertexWidth, sizeof(DWORD));
		stream->read(&dw32Bits, sizeof(DWORD));
		//
		int nDeclaration;
		stream->read(&nDeclaration, sizeof(int));
		stream->read(Declaration, sizeof(D3DVertexElement) * nDeclaration);
		//
		INT faceCount		=	dwFaceDataLength / (dw32Bits ? sizeof(DWORD) : sizeof(WORD)) / 3;
		char*	pVertex		=	(char*)malloc_s(dwVertexDataLength);
		stream->read(pVertex, dwVertexDataLength);
		char*	pIndex		=	(char*)malloc_s(dwFaceDataLength);
		stream->read(pIndex, dwFaceDataLength);
		//
		int attributeCount			=	dwAttributeLength / sizeof(unsigned char);
		unsigned char* attrTable	=	(unsigned char*)malloc_s( attributeCount );
		stream->read(attrTable, dwAttributeLength);
		//
		//D3DXCreateMesh(faceCount, dwVertexDataLength / dwVertexWidth, dw32Bits ? D3DXMESH_32BIT : 0, Declaration, GetEngineDevice(), &pMesh)))
		VertexElement	elements[MAX_FVF_DECL_SIZE];
		for(int i=0;i<nDeclaration;i++)
		{
			elements[i].method		=	Declaration[i].Method;
			elements[i].type		=	Declaration[i].Type;
			elements[i].usage		=	Declaration[i].Usage;
			elements[i].usageIndex	=	Declaration[i].UsageIndex;
		}
		int vertexCount	=	dwVertexDataLength / dwVertexWidth;
		g_manager.CreateMesh(mesh);
		mesh->Create(vertexCount, faceCount, elements, nDeclaration);
		assert(mesh->GetStrideSize() == dwVertexWidth);
		//mesh->SetVertexDecl(elements, nDeclaration, false);
		mesh->WriteVertexBuffer(pVertex, vertexCount, dwVertexWidth);
		mesh->WriteIndexBuffer((unsigned short*)pIndex, faceCount * 3, attrTable);
		mesh->Commit();
		assert(attributeCount == faceCount);
		//
		free_s(pVertex);
		free_s(pIndex);
		free_s(attrTable);
		return true;
	}

	VOID		SaveMesh(StreamBase* stream, DeviceMesh* pMesh)
	{
		D3DVertexElement	Declaration[MAX_FVF_DECL_SIZE];
		DWORD	dwVertexDataLength		=	pMesh->GetStrideSize() * pMesh->GetNumVertices();
		DWORD	dw32Bits				=	false;//
		DWORD	dwFaceDataLength		=	pMesh->GetNumFaces() * (dw32Bits ? sizeof(DWORD) : sizeof(WORD)) * 3;
		DWORD	dwAttributeLength		=	pMesh->GetNumFaces() * sizeof(unsigned char);
		DWORD	dwVertexWidth			=	pMesh->GetStrideSize();
		//
		int head	=	_MakeFileHead('m','e','s','h');
		stream->write(&head, sizeof(int));
		stream->write(&dwVertexDataLength, sizeof(DWORD));
		stream->write(&dwFaceDataLength, sizeof(DWORD));
		stream->write(&dwAttributeLength, sizeof(DWORD));
		stream->write(&dwVertexWidth, sizeof(DWORD));
		stream->write(&dw32Bits, sizeof(DWORD));
		int nDeclaration		= pMesh->GetNumElements();
		VertexElement* elements = pMesh->GetElementPtr();
		for(int i=0;i<nDeclaration;i++)
		{
			Declaration[i].Method=elements[i].method;
			Declaration[i].Type=elements[i].type;
			Declaration[i].Usage=elements[i].usage;
			Declaration[i].UsageIndex=elements[i].usageIndex;
		}
		stream->write(&nDeclaration, sizeof(int));
		stream->write(Declaration, sizeof(D3DVertexElement) * nDeclaration);
		stream->write(pMesh->GetVertexPtr(), dwVertexDataLength);
		stream->write(pMesh->GetIndices(), dwFaceDataLength);
		stream->write(pMesh->GetAttributePtr(), dwAttributeLength);
	}

};
