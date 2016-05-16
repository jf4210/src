#include "stdafx.h"
#include "bmp2ipl.h"

/**************************************************************************************************
*                                                                                                 *
*  NOTE:                                                                                          *
*    Only use the "8 bit, 1 or 3 channels" image, the BitMap use LowerLeft （底左）,              *
*    the IplImage use TopLeft （顶左）                                                            *
*  IplImage:                                                                                      *
*    nChannels = 1 or 3       number of channels                                                  *
*    depth = IPL_DEPTH_8U     pixel depth (8 bit), IPL_DEPTH_8U=8                                 *
*    dataOrder = 0            交叉存取颜色通道                                                    *
*    origin = IPL_ORIGIN_TL   图像数据保存形式，IPL_ORIGIN_BL底左结构, IPL_ORIGIN_TL顶左结构      *
*    align = 4                行数据对齐方式，保证下一行数据从整4字节位置开始                     *
*    width                                                                                        *
*    height                                                                                       *
*    widthStep                图像数据行大小，单位字节                                            *
*    imageSize                图像数据大小(=height*widthStep)，单位字节                           *
*    imageData                指向图像数据区，char *                                              *
*  BITMAPINFOHEADER:                                                                              *
*    biSize                   Specifies the number of bytes required by the structure             *
*    biWidth                                                                                      *
*    biHeight(>0)             biHeight>0底左结构；biHeight<0顶左结构                              *
*    biPlanes = 1             Specifies the number of planes for the target device. This value    *
*                             must be set to 1.                                                   *
*    biBitCount =8 or 24      bits-per-pixel,8-(pixelDepth=8,channels=1),                         *
*                             24-(pixelDepth=8,channels=3)                                        *
*    biCompression = BI_RGB   An uncompressed format                                              *
*    biSizeImage              Specifies the size, in bytes, of the image.This may be set to zero  *
*                             for BI_RGB bitmaps.                                                 *
*    biXPelsPerMeter = 0                                                                          *
*    biYPelsPerMeter = 0                                                                          *
*    biClrUsed = 0                                                                                *
*    biClrImportant = 0                                                                           *
*                                                                                                 *
***************************************************************************************************/
BMP::BMP(const BMP & img)
{
// 	if (!IsSupport(img)) {
// 		BMP();
// 		return;
// 	}

	//biHeader = img.biHeader;
	PBITMAPINFOHEADER  pBmpH = (PBITMAPINFOHEADER)&img.biHeader;
	memcpy(&biHeader, pBmpH, sizeof(BITMAPINFOHEADER));
	biHeader.biSizeImage = ImgSize();
	bool isLowerLeft = biHeader.biHeight>0;
	//int rowSize=0;
	if (!isLowerLeft)  biHeader.biHeight = -biHeader.biHeight;

	if (bmpData != NULL) delete[] bmpData;
	bmpData = new unsigned char[biHeader.biSizeImage];
	//memcpy(bmpData, img.bmpData, img.biHeader.biSizeImage);
	CopyData((char *)bmpData, (char*)img.bmpData, biHeader.biSizeImage,
		!isLowerLeft, biHeader.biHeight);
}

BMP::BMP(const IplImage &img) {
// 	if (!IsSupport(img)) {
// 		BMP();
// 		return;
// 	}
	bool isTopLeft = (img.origin == IPL_ORIGIN_TL);

	biHeader.biSize = sizeof(BITMAPINFOHEADER);
	biHeader.biWidth = img.width;
	biHeader.biHeight = img.height;
	biHeader.biPlanes = 1;
	biHeader.biBitCount = img.depth * img.nChannels;
	biHeader.biCompression = BI_RGB;
	biHeader.biSizeImage = img.imageSize;
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biClrUsed = 0;
	biHeader.biClrImportant = 0;

	if (bmpData != NULL) delete[] bmpData;
	bmpData = new unsigned char[img.imageSize];
	//memcpy(bmpData, img.ImageData, img.imageSize);
	CopyData((char*)bmpData, (char*)img.imageData, img.imageSize,
		isTopLeft, img.height);
	/*int i,j;
	CvScalar s;
	for(i=0;i<img.width;i++)
	for(j=0;j<img.height;j++){
	s=cvGet2D(&img,i,j);

	}
	*/
}

BMP::BMP(int width, int height, int bitCount) {
	if (bitCount != 8 && bitCount != 24) return;

	biHeader.biSize = sizeof(BITMAPINFOHEADER);
	biHeader.biWidth = width;
	biHeader.biHeight = height;
	biHeader.biPlanes = 1;
	biHeader.biBitCount = bitCount;
	biHeader.biCompression = BI_RGB;
	biHeader.biSizeImage = ImgSize();
	biHeader.biXPelsPerMeter = 0;
	biHeader.biYPelsPerMeter = 0;
	biHeader.biClrUsed = 0;
	biHeader.biClrImportant = 0;

	if (bmpData != NULL) delete[] bmpData;
	bmpData = new unsigned char[biHeader.biSizeImage];
	Clear();
}

// dest:         the destination image
// dataByteSize: the Source image
// height:       source image height
void BMP::CopyData(char *dest, const char *src, int dataByteSize,
	bool isConvert, int height) {
	char * p = dest;
	if (!isConvert) {
		memcpy(dest, src, dataByteSize);
		return;
	}
	if (height <= 0) return;
	//int height = dataByteSize/rowByteSize;
	int rowByteSize = dataByteSize / height;
	src = src + dataByteSize - rowByteSize;
	for (int i = 0; i<height; i++) {
		memcpy(dest, src, rowByteSize);
		dest += rowByteSize;
		src -= rowByteSize;
	}
}

IplImage * BMP::BMP2Ipl() {
//	if (!IsSupport(*this)) return NULL;

	IplImage *iplImg;
	int height;
	bool isLowerLeft = biHeader.biHeight>0;
	height = (biHeader.biHeight>0) ? biHeader.biHeight : -biHeader.biHeight;
	iplImg = cvCreateImage(cvSize(biHeader.biWidth, height), IPL_DEPTH_8U, biHeader.biBitCount / 8);
	//iplImg = cvCreateImageHeader( cvSize(biHeader.biWidth, height), IPL_DEPTH_8U, biHeader.biBitCount / 8);

	//cvSetData(iplImg,(char*)bmpData,biHeader.biSizeImage/height);
	CopyData(iplImg->imageData, (char*)bmpData, biHeader.biSizeImage,
		isLowerLeft, height);
	/*int i,j;
	CvScalar s;
	int channels=biHeader.biBitCount / 8;
	int step=(biHeader.biWidth*channels+3) & -4;
	int loc=0;
	for(i=0;i<iplImg->height;i++){
	for(j=0;j<iplImg->width;j++){
	loc=i*step + j*channels;
	s.val[0]=bmpData[loc];
	if(channels==3){
	s.val[1]=bmpData[loc+1];
	s.val[2]=bmpData[loc+2];
	}
	cvSet2D(iplImg,i,j,s);
	}
	}*/
	return iplImg;
}

void BMP::Clear() {
	if (bmpData == NULL) return;
	memset(bmpData, 0, ImgSize());
}

void BMP::ReSize(int newW, int newH) {
	biHeader.biWidth = newW;
	biHeader.biHeight = newH;
	biHeader.biSizeImage = ImgSize();
	if (bmpData != NULL) delete[] bmpData;
	bmpData = new unsigned char[biHeader.biSizeImage];
	Clear();
}

bool BMP::CreateImage(const BITMAPINFOHEADER &bih) {
	memset(&biHeader, 0, sizeof(BITMAPINFOHEADER));
	delete[] bmpData;
	bmpData = NULL;

	memcpy(&biHeader, &bih, sizeof(BITMAPINFOHEADER));
	biHeader.biSizeImage = ImgSize();
	bmpData = new unsigned char[biHeader.biSizeImage];
	if (bmpData == NULL) return false;
	else{
		Clear();
		return true;
	}
}