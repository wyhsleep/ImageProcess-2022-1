#include <stdio.h>
#include "bmphdr.h"
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
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
 
	BITMAPINFOHEADER infoHead = infoHeader;           //将打开的bmpinfoheader数据复制给新的bmp
    infoHead.biSizeImage = lineByte*infoHead.biHeight;
    infoHead.biHeight = height;
    infoHead.biWidth = width;
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
    char readpath[] = "binary-square-distorted.bmp";
    char writepath[] = "out-binary-square-distorted.bmp";
    readBmp(readpath);
    unsigned char *outpBmpBuf = (unsigned char *)malloc(lineByte *height);
	unsigned char *tmp_erosion = (unsigned char *)malloc(lineByte *height);
	int unit[5][5] = {{0,0,255,0,0},{0,0,255,0,0},{255,255,255,255,255},{0,0,255,0,0},{0,0,255,0,0}};
	int sum = 0;
	unsigned char high = 255;
	// erosion
    for(int i = 0;i < width;i++)
	{	
		for(int j = 0;j < height;j++)
		{	
			*(tmp_erosion+i*lineByte+j) = 0;
			{
				sum = 0;
				for(int m = -2;m < 3;m++)
				{
					for(int n = -2;n <3;n++)
					{	
						sum = sum + (*(pBmpBuf+(m+i)*lineByte+n+j))*(unit[2+m][2+n]);
					}
					if(sum == 255*255*9)
					{
						*(tmp_erosion+i*lineByte+j) = 255;
					}
				}
			}
		}
	}
	for(int i = 0;i<width;i++)
	{
		for(int j = 0;j<height;j++)
		{
			*(outpBmpBuf+i*lineByte+j) = abs((*(tmp_erosion+i*lineByte+j))-(*(pBmpBuf+i*lineByte+j)));
		}
	}
	saveBmp(writepath, outpBmpBuf);
	free(pBmpBuf);
    free(outpBmpBuf);
	free(tmp_erosion);
	printf("%d",bitcount);
	return 0;
}
