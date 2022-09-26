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

double BiCubic(double x)                           //权值计算函数
{
    double y;
    if(fabs(x) < 1 || fabs(x) == 1)
    {
        y = 1-2.5*pow(fabs(x),2)+1.5*pow(fabs(x),3);
    } 
    else if (1 < fabs(x) && fabs(x) < 2)
    {
        y = 2-4*fabs(x)+2.5*pow(fabs(x),2)-0.5*pow(fabs(x),3);
    }
    else
    {
        y = 0;
    }
    
    return y;
}

int main(){
    int tmpi,tmpj;
    double unit = (512*1.5-1)/(512-1);
    double sum = 0;
    char readpath[] = "lena512.bmp";
    char writepath[] = "outlena768.bmp";
    readBmp(readpath);
    int outwidth = 1.5*width;
    int outheight = 1.5*height;
    int outlineByte = (outwidth * bitcount / 8 + 3) / 4 * 4;
    unsigned char *outpBmpBuf = (unsigned char *)malloc(sizeof(unsigned char) * outlineByte *outheight);
    for(int i = 0;i < outheight;i++)
    {   
        for(int j = 0;j < outwidth;j++)
        {   //计算双三次插值
            tmpi = (i/unit);
            tmpj = (j/unit);
            double u = i/unit - tmpi;
            double v = j/unit - tmpj;
            sum = 0;
            for(int ii = -1;ii < 3;ii++)
            { 
                for(int jj = -1;jj < 3;jj++)
                {   if(tmpi+ii > 0 && tmpj+jj > 0 && tmpi+ii < width && tmpj+jj < height)
                    {
                        sum = sum+BiCubic((double)(ii-u))*BiCubic((double)(jj-v))*(*(pBmpBuf+(tmpi+ii)*lineByte+tmpj+jj));
                    }
                }
            }
            if (sum > 255)
                sum = 255;
            *(outpBmpBuf + i * outlineByte + j) = (int)sum;
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
