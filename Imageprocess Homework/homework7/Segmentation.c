#include <stdio.h>
#include "bmphdr.h"
#include <stdlib.h>
#include <math.h>
#include <stdlib.h>
#define size_max 49 
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
    char readpath[] = "yeast-cells.bmp";
    char writepath[] = "outyeast-cells.bmp";
    readBmp(readpath);
    unsigned char *tmp = (unsigned char *)malloc(sizeof(unsigned char) * lineByte *height);
    unsigned char *outpBmpBuf = (unsigned char *)malloc(sizeof(unsigned char) * lineByte *height);
    int T1 = 127;
    int T2 = 0;
    int Delta_T = 5;
    int sum_1 = 0;
    int sum_2 = 0;
    int num_1 = 0;
    int num_2 = 0;
    int mean_1 = 0;
    int mean_2 = 0;
    while(T1 - T2 > Delta_T)
    {
        T2 = T1;
        sum_1 = 0;
        sum_2 = 0;
        num_1 = 0;
        num_2 = 0;
        mean_1 = 0;
        mean_2 = 0;
        for(int i =0;i<width;i++)
        {
            for(int j =0;j<height;j++)
            {
                if(*(pBmpBuf+i*lineByte+j) > T1)
                {
                    sum_1 += *(pBmpBuf+i*lineByte+j);
                    num_1 += 1;
                    *(outpBmpBuf+i*lineByte+j) = 255;
                }
                else
                {
                    sum_2 += *(pBmpBuf+i*lineByte+j);
                    num_2 += 1;
                    *(outpBmpBuf+i*lineByte+j) = 0;
                }
            }
        }
        mean_1 = sum_1/num_1;
        mean_2 = sum_2/num_2;
        T1 = (mean_1+mean_2)/2;
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
