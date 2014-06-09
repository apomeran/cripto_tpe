#ifndef __common__
#define __common__

#include <stdlib.h>

struct data {
	size_t len;
	unsigned char* bytes;
};
typedef unsigned short int WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

#pragma pack(push,1)
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagIMAGE{
	BITMAPFILEHEADER file_header;
	BITMAPINFOHEADER info_header;
	unsigned char* data;
} BITMAPIMAGE;
#pragma pack(pop)
#endif
