#include <stdio.h>
#include <Windows.h>
#include <math.h>
#include <stdlib.h>
#pragma warning (disable:4996)

void makePadding(double* Y_1,double* Y_2, int width_1,int height_1,int padding)
{
	int width_p = width_1 + padding * 2;
	int height_p = height_1 + padding * 2;
	int size_p = width_p * height_p;

	for (int j = 0; j < height_1; j++) {
		for (int i = 0; i < width_1; i++) {
			Y_2[(j + padding) * width_p + (i + padding)] = Y_1[j * width_1 + i];
		}
	}

	for (int p = 0; p < padding; p++) {
		for (int i = 0; i < width_1; i++) {
			Y_2[p*width_p + (i + padding)] = Y_1[0 * width_1 + i];
		}
	}

	for (int p = 0; p < padding; p++) {
		for (int j = 0; j < height_1; j++) {
			Y_2[(j + padding) * width_p + p] = Y_1[j * width_1];
		}
	}

	for (int p = 0; p < padding; p++) {
		for (int j = 0; j < height_1; j++) {
			Y_2[(j + padding) * width_p + (width_1 + padding) + p] = Y_1[(j + 1) * width_1 - 1];
		}
	}


	for (int p = 0; p < padding; p++) {
		for (int i = 0; i < width_1; i++) {
			Y_2[(p + width_1 + padding)*width_p + (i + padding)] = Y_1[(height_1 - 1) * width_1 + i];
		}
	}

	for (int j = 0; j < padding; j++) {
		for (int i = 0; i < padding; i++) {
			Y_2[j* width_p + (i)] = Y_1[0 * width_1 + 0];
		}
	}

	for (int j = 0; j < padding; j++) {
		for (int i = 0; i < padding; i++) {
			Y_2[j*width_p + (width_1 + padding + i)] = Y_1[1 * width_1 - 1];
		}
	}

	for (int j = 0; j < padding; j++) {
		for (int i = 0; i < padding; i++) {
			Y_2[(height_1 + padding + j)*width_p + (i)] = Y_1[(height_1 - 1) *width_1];
		}
	}

	for (int j = 0; j < padding; j++) {
		for (int i = 0; i < padding; i++) {
			Y_2[(height_1 + padding + j) * width_p + (width_1 + padding + i)] = Y_1[(height_1)* width_1 - 1];
		}
	}
}

// Y_1 : padding + thresholding / Y_2: 원본크기 erosion이미지
void erosion(double* Y_1, double* Y_2, int width_1, int height_1, int padding)
 {
	int width_p = width_1 + padding * 2;
	int height_p = height_1 + padding * 2;
	int size_p = width_p * height_p;
	double ErosionKernel[3][3] = { 255, };
	for (int j = padding; j < height_p - padding; j++)
	{
		for (int i = padding; i < width_p - padding; i++)
		{
			int cnt = 0;
			for (int k = 0; k < 3; k++)
				for (int l = 0; l < 3; l++)
					if(Y_1[(j - padding + k)*width_p + (i - padding) + l]==255)
						cnt++;

			if(cnt == 9)
				Y_2[(j - padding)*width_1 + (i - padding)] = 255;
			else
				Y_2[(j - padding)*width_1 + (i - padding)] = 0;
		}
	}
}

// Y_1 : padding + thresholding / Y_2: 원본크기 dilation이미지
void dilation(double* Y_1, double* Y_2, int width_1, int height_1, int padding)
{
	int width_p = width_1 + padding * 2;
	int height_p = height_1 + padding * 2;
	int size_p = width_p * height_p;

	double* Y_p;
	Y_p = (double*)calloc(width_p *height_p, sizeof(double));

	for (int j = padding; j < height_p - padding; j++)
	{
		for (int i = padding; i < width_p - padding; i++)
		{
			if (Y_1[j*width_p + i] == 255)
			{
				for (int k = 0; k < 3; k++)
					for (int l = 0; l < 3; l++)
						Y_p[(j - padding + k)*width_p + (i - padding) + l] = 255;
			}
		}
	}

	for (int j = padding; j < height_p - padding; j++)
	{
		for (int i = padding; i < width_p - padding; i++)
		{
			Y_2[(j-padding)*width_1 + (i-padding)] = Y_p[j*width_p + i];
		}
	}

	free(Y_p);
}

int main()
{
	BITMAPFILEHEADER bmpFile_1;
	BITMAPINFOHEADER bmpInfo_1;

	FILE *inputFile_1 = NULL;
	inputFile_1 = fopen("AIcenterY.bmp", "rb");
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

	int padding = 2;
	int width_p = width_1 + padding * 2;
	int height_p = height_1 + padding * 2;
	int size_p = width_p * height_p;
	double* Y_2;
	Y_2 = (double*)calloc(width_p *height_p, sizeof(double));
	// 원본
	makePadding(Y_1, Y_2, width_1, height_1, padding);

	double laplace[3][3] =
	{ -1.0, -1.0, -1.0,
	  -1.0, 8.0 ,-1.0,
	  -1.0, -1.0, -1.0 };
	int filterSize = 3;

	double* Y_e;
	Y_e = (double*)calloc(width_1 *height_1, sizeof(double));

	for (int j = padding; j < height_p - padding; j++)
	{
		for (int i = padding; i < width_p - padding; i++)
		{
			double changepixel = 0;
			for (int k = 0; k < filterSize; k++)
				for (int l = 0; l < filterSize; l++)
				{
					changepixel += Y_2[(j - padding + k)*width_p + (i - padding) + l] * laplace[k][l];
				}
			if (changepixel >= 100)
				Y_e[(j - padding)*width_1 + i - padding] = 255;
			else
				Y_e[(j - padding)*width_1 + i - padding] = 0;
			}
		}
	// 엣지영상 + 패딩
	double* Y_ep;
	Y_ep = (double*)calloc(width_p *height_p, sizeof(double));
	makePadding(Y_e, Y_ep, width_1, height_1, padding);

	//엣지 영상에 erosion 또는 dilation 적용
	double* Y_3;

	
	unsigned Y;
	for (int j = 0; j < height_1; j++)
	{
		for (int i = 0; i < width_1; i++)
		{
			Y = (unsigned char)(Y_5[j*width_1 + i] > 255 ? 255 : (Y_5[j*width_1 + i] < 0 ? 0 : Y_5[j*width_1 + i]));
			outputImg[j *stride_1 + 3 * i + 0] = Y;
			outputImg[j*stride_1 + 3 * i + 1] = Y;
			outputImg[j*stride_1 + 3 * i + 2] = Y;
		}
	}

	FILE *outputFile = fopen("dilation_1.bmp", "wb");
	fwrite(&bmpFile_1, sizeof(BITMAPFILEHEADER), 1, outputFile);
	fwrite(&bmpInfo_1, sizeof(BITMAPINFOHEADER), 1, outputFile);
	fwrite(outputImg, sizeof(unsigned char), size_1, outputFile);

	free(Y_1);
	free(Y_2);
	free(Y_e);
	free(Y_ep);

	free(inputImg_1);
	free(outputImg);
	fclose(inputFile_1);
	fclose(outputFile);

	return 0;
}