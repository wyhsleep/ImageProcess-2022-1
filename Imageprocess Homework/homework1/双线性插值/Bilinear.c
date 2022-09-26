#include <stdio.h>
#include <stdlib.h>
#include "bmphdr.h"
#include <math.h>

BITMAPINFOHEADER infoHeader;
BITMAPFILEHEADER fileHeader;
RGBQUAD 		*pColorTable;
int width,height,bitcount,lineByte;
unsigned char 	*pBmpBuf;

char *readBmp(char *filepath)
{
    FILE *fp = fopen("lena512.bmp","rb");
    if(fp == NULL)                      //判断文件是否打开成功
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
    int lineByte = (1.5*width * bitcount / 8 + 3) / 4 * 4;
    infoHead.biWidth = 1.5 * width;
    infoHead.biHeight = 1.5 * height;
    infoHead.biSizeImage = lineByte*infoHead.biHeight;
	fwrite(&infoHead, 40, 1, fp);
 
	if (bitcount == 8)
	{
		fwrite(pColorTable,sizeof(RGBQUAD),256,fp);
	}
	
	fwrite(imgBuf, 1.5*height * lineByte, 1, fp);   //存储新图片像素数据
	fclose(fp);
	printf("Successfully save the file");
	return 0;
}


int main(){
    double tmpi,tmpj,tmpx1,tmpx2,tmpy;
    int fx11,fx12,fx21,fx22;
    char readpath[] = "lena512.bmp";
    char writepath[] = "outlena768.bmp";
    readBmp(readpath);
    int outwidth = 1.5*width;
    int outheight = 1.5*height;
    int outlineByte = (outwidth * bitcount / 8 + 3) / 4 * 4;
    double unit = (512*1.5-1)/(512-1);
    unsigned char *outpBmpBuf = (unsigned char *)malloc(sizeof(unsigned char) * outlineByte *outheight);
    for(int i = 0;i < outheight; i++)
    {   
        for(int j = 0;j < outwidth; j++)
        {    // 计算双线性插值
            tmpi = floor(i/unit)*unit;
            tmpj = floor(j/unit)*unit;
            fx11 = *(pBmpBuf +((int)(i/unit))*lineByte+(int)(j/unit));
            fx12 = *(pBmpBuf +((int)(i/unit)+1)*lineByte+(int)(j/unit));
            fx21 = *(pBmpBuf +((int)(i/unit))*lineByte+(int)(j/unit)+1);
            fx22 = *(pBmpBuf +((int)(i/unit)+1)*lineByte+(int)(j/unit)+1);
            tmpx1 = (tmpi+unit-i)/unit*fx11+(i-tmpi)/unit*fx12;
            tmpx2 = (tmpi+unit-i)/unit*fx21+(i-tmpi)/unit*fx22;
            tmpy = (tmpj+unit-j)/unit*tmpx1+(j-tmpj)/unit*tmpx2;        
            *(outpBmpBuf+i*outlineByte+j) = tmpy;
        }
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
