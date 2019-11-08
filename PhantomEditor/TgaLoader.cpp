#include "StdAfx.h"
#include "TgaLoader.h"


TextureTga::TextureTga()
{
	imageData	=	0;
	bpp			=	0;
	width		=	0;
	height		=	0;
}

TextureTga::~TextureTga()
{
	if(imageData)
		free(imageData);
	imageData	=	0;
}


BOOL	TextureTga::ConvertToImage(ATL::CImage& img)
{
	if(!imageData)
		return false;
	if(!img.Create(width,height, 32, 0))
		return false;
	int nPitch	=	img.GetPitch();
	for(int y=0;y<height;y++)
	{
		for(int x=0;x<width;x++)
		{
			if(bpp == 32)
			{
				BGRA_Tga* rgba	=	(BGRA_Tga*)imageData;
				BGRA_Tga& src	=	rgba[(height - y - 1) * width + x];
				//
				char* bits	=	(char*)img.GetBits();
				char* rgb	=	bits + y * img.GetPitch();
				RGBA_Tga& dest	=	*((RGBA_Tga*)rgb + x);
				dest.r		=	(int)src.r;
				dest.g		=	(int)src.g;
				dest.b		=	(int)src.b;
				dest.a		=	(int)src.a;
			}
			else
			{
				BGR_Tga* rgba	=	(BGR_Tga*)imageData;
				BGR_Tga& src	=	rgba[(height - y - 1) * width + x];
				//
				char* bits	=	(char*)img.GetBits();
				char* rgb	=	bits + y * img.GetPitch();
				RGBA_Tga& dest	=	*((RGBA_Tga*)rgb + x);
				dest.r		=	(int)src.r;
				dest.g		=	(int)src.g;
				dest.b		=	(int)src.b;
				dest.a		=	255;
			}
		}
	}
	return true;
}

unsigned char uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};
unsigned char cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};

struct TGAHeader
{
	unsigned char Header[12];                                                                     // TGA File Header 
};

struct	TGA
{
	unsigned char         header[6];
	unsigned int          bytesPerPixel;
	unsigned int          imageSize;
	unsigned int          temp;
	unsigned int          type;
	unsigned int          Height;
	unsigned int          Width;
	unsigned int          Bpp;
};

int fread(void* v, int length, int count, Phantom::StreamBase* pStream)
{
	return pStream->read(v, length * count);
}

bool LoadTGA(TextureTga * texture, Phantom::StreamBase* pStream)
{
	TGAHeader tgaheader;
	Phantom::StreamBase* fTGA		=	pStream;
	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)
		return false;
	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
		return LoadUncompressedTGA(texture, fTGA);
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
		return LoadCompressedTGA(texture, fTGA);
	return false;
}

bool LoadUncompressedTGA(TextureTga * texture, Phantom::StreamBase* fTGA)
{
	TGA tga;
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
		return false;
	texture->width  = tga.header[1] * 256 + tga.header[0];
	texture->height = tga.header[3] * 256 + tga.header[2];
	texture->bpp    = tga.header[4];
	tga.Width               = texture->width;
	tga.Height              = texture->height;
	tga.Bpp                 = texture->bpp;
	if((texture->width <= 0) || (texture->height <= 0))
	{
		return false;
	}
	if(texture->bpp == 16)
	{
		unsigned short pixels = 0;
		int r = 0, g = 0, b = 0;
		int channels = 3;
		int stride = channels * tga.Width;
		texture->imageData = (unsigned char *)malloc(stride * tga.Height);
		for (int i = 0; i < tga.Width * tga.Height; i++)
		{
			fread (&pixels, sizeof (unsigned short), 1, fTGA);
			b = (pixels & 0x1f) << 3;
			g = ((pixels >> 5) & 0x1f) << 3;
			r = ((pixels >> 10) & 0x1f) << 3;
			texture->imageData[i * 3 + 0] = r;
			texture->imageData[i * 3 + 1] = g;
			texture->imageData[i * 3 + 2] = b;
		}
		return true;
	}
	if((texture->bpp != 24) && (texture->bpp !=32))
		return false;
	tga.bytesPerPixel       = (tga.Bpp / 8);
	tga.imageSize           = (tga.bytesPerPixel * tga.Width * tga.Height);
	texture->imageData      = (unsigned char *)malloc(tga.imageSize);
	if(texture->imageData == NULL)
		return false;
	if(fread(texture->imageData, 1, tga.imageSize, fTGA) != tga.imageSize)
	{ 
		if(texture->imageData != NULL)
			free(texture->imageData);
		return false;
	}
	for(unsigned int cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{ 
		texture->imageData[cswap] ^= texture->imageData[cswap+2] ^= texture->imageData[cswap] ^= texture->imageData[cswap+2];
	}
	return true;                                                                                                                    // Return success 
}

bool LoadCompressedTGA(TextureTga * texture, Phantom::StreamBase* fTGA)
{
	TGA tga;
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
		return false;
	texture->width  = tga.header[1] * 256 + tga.header[0];
	texture->height = tga.header[3] * 256 + tga.header[2];
	texture->bpp    = tga.header[4];
	tga.Width               = texture->width;
	tga.Height              = texture->height;
	tga.Bpp                 = texture->bpp;
	if((texture->width <= 0) || (texture->height <= 0))
		return false;
	tga.bytesPerPixel       = (tga.Bpp / 8);
	int bytesPerPixel		= max(tga.bytesPerPixel, 3);
	tga.imageSize           = (bytesPerPixel * tga.Width * tga.Height);
	texture->imageData      = (unsigned char *)malloc(tga.imageSize);
	if(texture->imageData == NULL)
		return false;
	unsigned int pixelcount       = tga.Height * tga.Width;
	
	unsigned int currentpixel     = 0;
	unsigned int currentbyte      = 0;
	unsigned char * colorbuffer = (unsigned char *)malloc(tga.bytesPerPixel);
	do
	{
		unsigned char chunkheader = 0;
		if(fread(&chunkheader, sizeof(unsigned char), 1, fTGA) == 0)
		{
			if(texture->imageData != NULL)
				free(texture->imageData);
			return false;
		}
		if(chunkheader < 128)
		{
			chunkheader++;
			for(short counter = 0; counter < chunkheader; counter++)
			{ 
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
				{
					if(colorbuffer != NULL)
						free(colorbuffer);
					if(texture->imageData != NULL)
						free(texture->imageData);
					return false;
				}
				// write to memory 
				if(tga.bytesPerPixel == 2)
				{
					unsigned short pixels = *(unsigned short*)colorbuffer;
					unsigned char b = (pixels & 0x1f) << 3;
					unsigned char g = ((pixels >> 5) & 0x1f) << 3;
					unsigned char r = ((pixels >> 10) & 0x1f) << 3;
					texture->imageData[currentbyte          ] = r;
					texture->imageData[currentbyte + 1      ] = g;
					texture->imageData[currentbyte + 2      ] = b;
				}
				else
				{
					texture->imageData[currentbyte          ] = colorbuffer[2];
					texture->imageData[currentbyte + 1      ] = colorbuffer[1];
					texture->imageData[currentbyte + 2      ] = colorbuffer[0];
					if(tga.bytesPerPixel == 4)
					{
						texture->imageData[currentbyte + 3] = colorbuffer[3];
					}
				}
				currentbyte += bytesPerPixel;
				currentpixel++;
				if(currentpixel > pixelcount)
				{
					if(colorbuffer != NULL)
						free(colorbuffer);
					if(texture->imageData != NULL)
						free(texture->imageData);
					return false;
				}
			}
		}
		else
		{
			chunkheader -= 127;
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)
			{
				if(colorbuffer != NULL)
				{
					free(colorbuffer);
				}
				if(texture->imageData != NULL)
				{
					free(texture->imageData);
				}
				return false;
			}
			for(short counter = 0; counter < chunkheader; counter++)
			{
				if(tga.bytesPerPixel == 2)
				{
					unsigned short pixels = *(unsigned short*)colorbuffer;
					unsigned char b = (pixels & 0x1f) << 3;
					unsigned char g = ((pixels >> 5) & 0x1f) << 3;
					unsigned char r = ((pixels >> 10) & 0x1f) << 3;
					texture->imageData[currentbyte          ] = r;
					texture->imageData[currentbyte + 1      ] = g;
					texture->imageData[currentbyte + 2      ] = b;
				}
				else
				{
					texture->imageData[currentbyte          ] = colorbuffer[2];
					texture->imageData[currentbyte + 1      ] = colorbuffer[1];
					texture->imageData[currentbyte + 2      ] = colorbuffer[0];
					if(tga.bytesPerPixel == 4)
						texture->imageData[currentbyte + 3] = colorbuffer[3];
				}
				currentbyte += bytesPerPixel;
				currentpixel++;
				if(currentpixel > pixelcount)
				{
					if(colorbuffer != NULL)
						free(colorbuffer);
					if(texture->imageData != NULL)
						free(texture->imageData);
					return false;
				}
			}
		}
	}
	while(currentpixel < pixelcount);                                                                                                       // Loop while there are still pixels left 
	return true;
} 
//
//bool LoadCompressedTGA(TextureTga * texture, IStream* fTGA)
//{
//	TGA tga;
//	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)
//		return false;
//	texture->width  = tga.header[1] * 256 + tga.header[0];
//	texture->height = tga.header[3] * 256 + tga.header[2];
//	texture->bpp    = tga.header[4];
//	tga.Width               = texture->width;
//	tga.Height              = texture->height;
//	tga.Bpp                 = texture->bpp;
//	if((texture->width <= 0) || (texture->height <= 0))
//		return false;
//	tga.bytesPerPixel       = (tga.Bpp / 8);
//	tga.imageSize           = (tga.bytesPerPixel * tga.Width * tga.Height);
//	//
//	unsigned char rleID = 0;
//    int colorsRead = 0;
//    int	channels = tga.Bpp / 8;
//    int stride = channels * tga.Width;
//    texture->imageData = (unsigned char*)malloc(stride * tga.Height);
//    unsigned char pColors[1024];// = new unsigned char[channels];
//	int i = 0;
//    while (i < tga.Width * tga.Height)
//    {
//      fread (&rleID, sizeof (unsigned char), 1, fTGA);
//      if (rleID < 128)
//      {
//        rleID++;
//        while (rleID)
//        {
//          fread (pColors, sizeof (unsigned char) * channels, 1, fTGA);
//          texture->imageData[colorsRead + 0] = pColors[2];
//          texture->imageData[colorsRead + 1] = pColors[1];
//          texture->imageData[colorsRead + 2] = pColors[0];
//          if (tga.Bpp == 32)
//            texture->imageData[colorsRead + 3] = pColors[3];
//          i++;
//          rleID--;
//          colorsRead += channels;
//        }
//      }
//      else
//      {
//        rleID -= 127;
//        fread (pColors, sizeof (unsigned char) * channels, 1, fTGA);
//        while (rleID)
//        {
//          texture->imageData[colorsRead + 0] = pColors[2];
//          texture->imageData[colorsRead + 1] = pColors[1];
//          texture->imageData[colorsRead + 2] = pColors[0];
//          if (tga.Bpp == 32)
//            texture->imageData[colorsRead + 3] = pColors[3];
//          i++;
//          rleID--;
//          colorsRead += channels;
//        }
//      }
//    }
//	return true;
//} 
