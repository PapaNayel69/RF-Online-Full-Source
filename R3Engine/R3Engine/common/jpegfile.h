#ifndef __JPEG_FILE_H__
#define __JPEG_FILE_H__


#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)


BYTE * JpegFileToRGB(char* fileName,			// path to image
						   UINT *width,					// image width in pixels
						   UINT *height);				// image height

////////////////////////////////////////////////////////////////
// write a JPEG file from a 3-component, 1-byte per component buffer

BOOL ARGBToJpegFile(char* fileName,				// path
						BYTE *dataBuf,					// RGB buffer
						UINT pitch,						// ±Ê¿Ã¥Ÿ 
						UINT width,						// pixels
						UINT height,					// rows
						BOOL color,						// TRUE = RGB
														// FALSE = Grayscale
						int quality);					// 0 - 100
BOOL RGBToJpegFile(char* fileName,				// path
						BYTE *dataBuf,					// RGB buffer
						UINT width,						// pixels
						UINT height,					// rows
						BOOL color,						// TRUE = RGB
														// FALSE = Grayscale
						int quality);					// 0 - 100


////////////////////////////////////////////////////////////////
// fetch width / height of an image

BOOL GetJPGDimensions(char* fileName,			// path
							UINT *width,				// pixels
							UINT *height);

////////////////////////////////////////////////////////////////
//	utility functions
//	to do things like DWORD-align, flip, convert to grayscale, etc.
//

////////////////////////////////////////////////////////////////
// allocates a DWORD-aligned buffer, copies data buffer
// caller is responsible for delete []'ing the buffer

BYTE * MakeDwordAlignedBuf(BYTE *dataBuf,			// input buf
								 UINT widthPix,				// input pixels
								 UINT height,				// lines
								 UINT *uiOutWidthBytes);	// new width bytes


////////////////////////////////////////////////////////////////
// if you have a DWORD aligned buffer, this will copy the
// RGBs out of it into a new buffer. new width is widthPix * 3 bytes
// caller is responsible for delete []'ing the buffer

BYTE *RGBFromDWORDAligned(BYTE *inBuf,				// input buf
								UINT widthPix,				// input size
								UINT widthBytes,			// input size
								UINT height);

////////////////////////////////////////////////////////////////
// vertically flip a buffer - for BMPs
// in-place

// note, this routine works on a buffer of width widthBytes: not a 
// buffer of widthPixels.
BOOL VertFlipBuf(BYTE * inbuf,						// input buf
				   UINT widthBytes,							// input width bytes
				   UINT height);							// height

// NOTE :
// the following routines do their magic on buffers with a whole number
// of pixels per data row! these are assumed to be non DWORD-aligned buffers.

////////////////////////////////////////////////////////////////
// convert RGB to grayscale	using luminance calculation
// in-place

BOOL MakeGrayScale(BYTE *buf,						// input buf 
					UINT widthPix,							// width in pixels
					UINT height);							// height

////////////////////////////////////////////////////////////////
// swap Red and Blue bytes
// in-place

BOOL BGRFromRGB(BYTE *buf,							// input buf
				UINT widthPix,								// width in pixels
				UINT height);								// lines

#endif