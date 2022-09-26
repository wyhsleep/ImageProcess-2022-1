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

    if (bitcount == 8)
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
 
	if (bitcount == 8)
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
    char readpath[] = "0501bw.bmp";
    char writepath[] = "out0501bw.bmp";
    readBmp(readpath);
    unsigned char *outpBmpBuf = (unsigned char *)malloc(sizeof(unsigned char) * lineByte *height);
	int L = pow(2,bitcount);
    int size = height * width;
	double Pr_k[256] = {0};
	double Tr_k[256] = {0};
	for (int k = 0; k < L; k++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width;j++)
			{
				if (*(pBmpBuf+i*lineByte+j) == k)
				{
					Pr_k[k] = Pr_k[k]+ 1;
				}
			}
		}
		Pr_k[k] = Pr_k[k]/ size;
	}

	for (int k = 0; k < L; k++)
	{
		for (int i = 0; i < k+1; i++)
		{
			Tr_k[k] = Tr_k[k]+(L-1)*Pr_k[i];
		}
		Tr_k[k] = ceil(Tr_k[k]);
	}

	for (int k = 0; k < L; k++)
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width;j++)
			{
				if (*(pBmpBuf+i*lineByte+j) == k)
				{
					*(outpBmpBuf+i*lineByte+j) = (int)(Tr_k[k]);
				}
			}
		}
		Pr_k[k] = Pr_k[k]/ size;
	}

	saveBmp(writepath, outpBmpBuf);

	free(pBmpBuf);
    free(outpBmpBuf);
	if (bitcount == 8)
	{
		free(pColorTable);
	}
	return 0;
}