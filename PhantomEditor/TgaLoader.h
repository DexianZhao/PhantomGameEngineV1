#pragma once
#include <windows.h> 
#include <stdio.h>                                                                              // Standard I/O header  
#include <atlimage.h>

#pragma pack(push, 1)
struct RGB_Tga
{
	unsigned char	r,g,b;
};
struct BGR_Tga
{
	unsigned char	b,g,r;
};
struct RGBA_Tga
{
	unsigned char	r,g,b,a;
};
struct BGRA_Tga
{
	unsigned char	b,g,r,a;
};
struct ARGB_Tga
{
	unsigned char	a,r,g,b;
};

#pragma pack(pop)

class TextureTga
{
public:
	TextureTga();
	~TextureTga();
	bool loadFromTGA(Phantom::StreamBase* pStream, bool saveImageData=false);
	unsigned char * imageData;
	unsigned int	bpp; 
	unsigned int	width; 
	unsigned int	height; 
	BOOL			ConvertToImage(ATL::CImage& img);
};

struct	MyStream
{
	Phantom::StreamBase*		s;
	int Read(void* v, int length)
	{
		return s->read(v, length);
	}
};

bool LoadUncompressedTGA(TextureTga *, Phantom::StreamBase* pStream);
bool LoadCompressedTGA(TextureTga *, Phantom::StreamBase* pStream);
bool LoadTGA(TextureTga *, Phantom::StreamBase* pStream);
