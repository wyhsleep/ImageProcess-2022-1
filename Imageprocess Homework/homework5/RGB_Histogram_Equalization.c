#include <stdio.h>
#include "bmphdr.h"
#include <stdlib.h>
#include <math.h>

BITMAPINFOHEADER infoHeader;
BITMAPFILEHEADER fileHeader;
RGBQUAD 		*pColorTable;
int width,height,bitcount,lineByte;
unsigned char 	*pBmpBuf;

char *readBmp(char *filepath)
{
    FILE *fp = fopen(filepath,"rb");
    if(fp == NULL)                    //判断文件是否打开成功
    {
        printf("Failed to open the file\n");
        return -1;
    }
    unsigned short  fileType; 
    fread(&fileHeader, 1, 14, fp);     //14是fileheader的长度
    fread(&infoHeader, 1, 40, fp);     //40是infoheader的长度
    width = infoHeader.biWidth;        //读取图片大小相关参数
    height = infoHeader.biHeight;
    bitcount = infoHeader.biBitCount;
	lineByte = (width * bitcount / 8 + 3) / 4 * 4;

    if (bitcount == 24)
	{   pColorTable = (RGBQUAD *)malloc(4 * 1024);
		fread(pColorTable, 4, 256, fp);
	}
    pBmpBuf = (unsigned char *)malloc(sizeof(unsigned char) * lineByte * height); //分配内存存储图片像素数据
	fread(pBmpBuf, 1, lineByte * height, fp);
    fclose(fp);
    printf("Successfully open the file\n");
    return pBmpBuf;
}

int saveBmp(char *bmpName, unsigned char *imgBuf)
{
	if(imgBuf == NULL)
	{
		printf("imgBuf is NULL!!\n");
		return -1;
	}

	FILE *fp = fopen(bmpName, "wb");
	if (fp == NULL)
	{
		printf("fopen error...\n");
		return -1;
	}
	
	BITMAPFILEHEADER fileHead = fileHeader;           //将打开的bmpfileheader数据复制给新的bmp
	fwrite(&fileHead, 14, 1, fp);
 
	BITMAPINFOHEADER infoHead = infoHeader;           //将打开的bmpinfoheader数据复制给新的bmp并修改图片大小
    infoHead.biSizeImage = lineByte*infoHead.biHeight;
	fwrite(&infoHead, 40, 1, fp);
 
	if (bitcount == 24)
	{
		fwrite(pColorTable,sizeof(RGBQUAD),256,fp);
	}
	
	fwrite(imgBuf, height * lineByte, 1, fp);   //存储新图片像素数据
	fclose(fp);
	printf("Successfully save the file");
	return 0;
}

int main()
{
    char readpath[] = "0501.bmp";
    char writepath[] = "out0501.bmp";
    readBmp(readpath);
    unsigned char *outpBmpBuf = (unsigned char *)malloc(sizeof(unsigned char) * lineByte *height);
	int L = 256;
    int size = height * width;
	double Pr_k_R[256] = {0};
	double Tr_k_R[256] = {0};
	double Pr_k_G[256] = {0};
	double Tr_k_G[256] = {0};
	double Pr_k_B[256] = {0};
	double Tr_k_B[256] = {0};
	//计算像素的分布

	for (int k = 0; k < L; k++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width;j++)
			{
				if (*(pBmpBuf+i*lineByte+j*3) == k)
				{
					Pr_k_R[k] = Pr_k_R[k]+ 1;
				}
				if (*(pBmpBuf+i*lineByte+j*3+1) == k)
				{
					Pr_k_G[k] = Pr_k_G[k]+ 1;
				}
				if (*(pBmpBuf+i*lineByte+j*3+2) == k)
				{
					Pr_k_B[k] = Pr_k_B[k]+ 1;
				}
			}
		}
		Pr_k_R[k] = Pr_k_R[k]/ size;
		Pr_k_G[k] = Pr_k_G[k]/ size;
		Pr_k_B[k] = Pr_k_B[k]/ size;
	}
	//均衡化
	for (int k = 0; k < L; k++)
	{
		for (int i = 0; i < k+1; i++)
		{
			Tr_k_R[k] = Tr_k_R[k]+(L-1)*Pr_k_R[i];
			Tr_k_G[k] = Tr_k_G[k]+(L-1)*Pr_k_G[i];
			Tr_k_B[k] = Tr_k_B[k]+(L-1)*Pr_k_B[i];
		}
		Tr_k_R[k] = floor(Tr_k_R[k]);
		Tr_k_G[k] = floor(Tr_k_G[k]);
		Tr_k_B[k] = floor(Tr_k_B[k]);
	}
	//分配新像素
	for (int k = 0; k < L; k++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width;j++)
			{
				if (*(pBmpBuf+i*lineByte+3*j) == k)
				{
					*(outpBmpBuf+i*lineByte+3*j) = (int)(Tr_k_R[k]);
				}
				if (*(pBmpBuf+i*lineByte+3*j+1) == k)
				{
					*(outpBmpBuf+i*lineByte+3*j+1) = (int)(Tr_k_G[k]);
				}
				if (*(pBmpBuf+i*lineByte+3*j+2) == k)
				{
					*(outpBmpBuf+i*lineByte+3*j+2) = (int)(Tr_k_B[k]);
				}
			}
		}
		Pr_k_R[k] = Pr_k_R[k]/ size;
		Pr_k_G[k] = Pr_k_G[k]/ size;
		Pr_k_B[k] = Pr_k_B[k]/ size;
	}

	saveBmp(writepath, outpBmpBuf);

	free(pBmpBuf);
    free(outpBmpBuf);
	if (bitcount == 24)
	{
		free(pColorTable);
	}
	return 0;
}