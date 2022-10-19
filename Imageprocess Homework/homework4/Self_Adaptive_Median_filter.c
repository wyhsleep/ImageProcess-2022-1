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

int i_cmp (const void* e1, const void* e2)
{
	return *((int*)e1) - *((int*)e2);   
}

void Median(unsigned char* pBmpBuf,unsigned char* tmp,int S_max)
{   
    for(int i = 0;i < height;i++)
   	{
		for(int j = 0;j < width;j++)
		{	    
                int size = 9;
                int median;
                int temp1[size];
                while (size<S_max||size == S_max)
                {
                    int k = 0;
                    int z_xy = *(pBmpBuf+i*lineByte+j);
                    median = ceil(size/2);
                    int temp1[size];
                    for(int m = i-floor(sqrt(size)/2); m<i+ceil(sqrt(size)/2);m++)
                    {
                        for(int n = j-floor(sqrt(size)/2); n<j+ceil(sqrt(size)/2);n++)
                        {   
                            if(m < 0 || n < 0)
                                temp1[k] = 0;
                            else
                                temp1[k] = *(pBmpBuf+m*lineByte+n);
                            k++;
                        }
                    }
                    qsort(temp1,size,sizeof(int),i_cmp);
                    if(temp1[0] < temp1[median] && temp1[median] < temp1[size-1])
                    {
                        if(temp1[0] < z_xy && z_xy < temp1[size-1])
                            *(tmp+i*lineByte+j) = z_xy;
                        else
                            *(tmp+i*lineByte+j) = temp1[median];
                        break;
                    }
                    else
                        size = pow(sqrt(size)+2,2);
                }
                if(size > S_max)
                    *(tmp+i*lineByte+j) = temp1[median];

		}
	}
}

void depulicate(unsigned char* pBmpBuf,unsigned char* tmp)
{
    for(int i =0;i<width;i++)
    {
        for(int j =0;j<height;j++)
        {
            *(tmp+i*lineByte+j) = *(pBmpBuf+i*lineByte+j);
        }
    }
}
int main()
{
    char readpath[] = "girl256-pepper&salt.bmp";
    char writepath[] = "outgirl256-pepper&salt.bmp";
    readBmp(readpath);
    unsigned char *tmp = (unsigned char *)malloc(sizeof(unsigned char) * lineByte *height);
    unsigned char *outpBmpBuf = (unsigned char *)malloc(sizeof(unsigned char) * lineByte *height);
    int iteration_num = 3;
    for (int i = 0;i<iteration_num;i++)
    {
        Median(pBmpBuf,outpBmpBuf,size_max);
        depulicate(outpBmpBuf,pBmpBuf);
    }


	saveBmp(writepath, outpBmpBuf);
	free(pBmpBuf);
    free(tmp);
    free(outpBmpBuf);
	if (bitcount == 8)
	{
		free(pColorTable);
	}
	return 0;
}
