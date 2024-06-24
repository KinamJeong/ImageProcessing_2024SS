#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdlib.h>
#pragma warning (disable:4996)

int main()
{
	BITMAPFILEHEADER bmpFile_1;
	BITMAPINFOHEADER bmpInfo_1;

	FILE *inputFile_1 = NULL;
	inputFile_1 = fopen("busanY3.bmp", "rb");
	fread(&bmpFile_1, sizeof(BITMAPFILEHEADER), 1, inputFile_1);
	fread(&bmpInfo_1, sizeof(BITMAPINFOHEADER), 1, inputFile_1);

	int width_1 = bmpInfo_1.biWidth;
	int height_1 = bmpInfo_1.biHeight;
	int size_1 = bmpInfo_1.biSizeImage;
	int bitCnt_1 = bmpInfo_1.biBitCount;
	int stride_1 = (((bitCnt_1 / 8)*width_1) + 3) / 4 * 4;

	//printf("%d %d %d %d %d", width_1, height_1, size_1, bitCnt_1, stride_1);

	unsigned char *inputImg_1, *outputImg;
	inputImg_1 = (unsigned char*)calloc(size_1, sizeof(unsigned char));
	outputImg = (unsigned char*)calloc(size_1, sizeof(unsigned char));
	fread(inputImg_1, sizeof(unsigned char), size_1, inputFile_1);

	// array Y_1에 인풋이미지 1의 픽셀 값 저장
	double *Y_1;
	Y_1 = (double*)calloc(width_1*height_1, sizeof(double));

	for (int j = 0; j < height_1; j++)
	{
		for (int i = 0; i < width_1; i++)
		{
			Y_1[j * width_1 + i] = inputImg_1[j*stride_1 + 3 * i + 0];
		}
	}

	int Hist[256] = { 0 , };
	for (int j = 0; j < height_1; j++)
	{
		for (int i = 0; i < width_1; i++)
		{
			int pixel = Y_1[j * width_1 + i];
			Hist[pixel] += 1;
		}
	}

	//for (int i = 0; i < 256; i++)Hist[i] = Hist[i] /20;

	int sumHist[256] = { 0, };
	sumHist[0] = Hist[0];
	for (int i = 1; i < 256; i++)
	{
		sumHist[i] += sumHist[i - 1] + Hist[i];
	}

	//for (int i = 0; i < 256; i++)sumHist[i] /= 512;
	
	double *Y_3;
	Y_3 = (double*)calloc(width_1*height_1, sizeof(double));
	for (int j = 0; j < height_1; j++)
	{
		for (int i = 0; i < width_1; i++)
		{
			Y_3[j*width_1 + i] = 255 * sumHist[(int)Y_1[j*width_1 + i]] / (width_1*height_1);
		}
	}
	unsigned Y;
	for (int j = 0; j < height_1; j++)
	{
		for (int i = 0; i < width_1; i++)
		{
			Y = (unsigned char)(Y_3[j*width_1 + i] > 255 ? 255 : (Y_3[j*width_1 + i] < 0 ? 0 : Y_3[j*width_1 + i]));
			outputImg[j*stride_1 + 3 * i + 0] = Y;
			outputImg[j*stride_1 + 3 * i + 1] = Y;
			outputImg[j*stride_1 + 3 * i + 2] = Y;
		}
	}


	FILE *outputFile = fopen("HE from busanY3 .bmp", "wb");
	fwrite(&bmpFile_1, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo_1, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(outputImg, sizeof(unsigned char), size_1, outputFile);

	free(Y_1);
	free(Y_3);
	free(inputImg_1);
	free(outputImg);
	fclose(inputFile_1);
	fclose(outputFile);


	return 0;
}