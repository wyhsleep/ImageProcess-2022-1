#include <stdio.h>
#include "bmphdr.h"
#include <stdlib.h>
#include <math.h>
#include "fft.c"
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

double GLPF(int u,int v,int D_0)
{   
    double D_uv = sqrt((u-width)*(u-width)+(v-height)*(v-height));
    double H_uv = exp(-1*D_uv*D_uv/(2*D_0*D_0));
    return H_uv;
}

int main()
{
    char readpath[] = "lena256-pepper&salt.bmp";
    char writepath[] = "outlena256-pepper&salt.bmp";
    readBmp(readpath);
    int tmplineByte = (width*2 * bitcount / 8 + 3) / 4 * 4;
    unsigned char *tmp = (unsigned char *)malloc(sizeof(unsigned char) * tmplineByte *2*height);
	int L = pow(2,bitcount);
    int size = height * width;
    double re,im,temp,real;
    double*repBmpBuf = malloc(512*512*sizeof(double));
	double*impBmpBuf = malloc(512*512*sizeof(double));
	double*refft_1 = malloc(512*512*sizeof(double));
	double*imfft_1 = malloc(512*512*sizeof(double));
	double*refft_2 = malloc(512*512*sizeof(double));
	double*imfft_2 = malloc(512*512*sizeof(double));
// 0 填充
	for(int m = 0;m<2;m++)
	{
		for(int n = 0;n<2;n++)
		{
			for(int i = 0;i < height;i++)
   			{
				for(int j = 0;j < width;j++)
				{	
					if(m == 0 && n==0)
					(*(repBmpBuf+2*height*(i+m*height)+j+n*width))= *(pBmpBuf+i*lineByte+j)*pow(-1,(i+j));
					else
					(*(repBmpBuf+2*height*(i+m*height)+j+n*width))= 0;
					
					(*(impBmpBuf+2*height*(i+m*height)+j+n*width))= 0;
				}
			}    
		}
	}

//FFT
	for(int i = 0;i<2*height;i++)
	{
		double retmp[512],imtmp[512],reffttmp[512],imffttmp[512];
		for (int j=0;j<2*width;j++)
		{
			retmp[j] = *(repBmpBuf+2*height*i+j);
			imtmp[j] = *(impBmpBuf+2*height*i+j);
		}

		fft(retmp,imtmp,512,9,reffttmp,imffttmp);

		for (int j=0;j<2*width;j++)
		{
			*(refft_1+2*height*i+j) = reffttmp[j];
			*(imfft_1+2*height*i+j) = imffttmp[j];
		}
	}

	for(int i = 0;i<2*height;i++)
	{
		double retmp[512],imtmp[512],reffttmp[512],imffttmp[512];
		for (int j=0;j<2*width;j++)
		{
			retmp[j] = *(refft_1+2*height*j+i);
			imtmp[j] = *(imfft_1+2*height*j+i);
		}

		fft(retmp,imtmp,512,9,reffttmp,imffttmp);
	
		for (int j=0;j<2*width;j++)
		{
			*(refft_2+2*height*j+i) = reffttmp[j];
			*(imfft_2+2*height*j+i) = imffttmp[j];
		}

	}
//滤波

	for(int i = 0;i < 2*height;i++)
    {
        for(int j = 0;j < 2*width;j++)
        {
            *(refft_2+i*2*height+j) = (*(refft_2+i*2*height+j))*GLPF(i,j,40);
			*(imfft_2+i*2*height+j) = (*(imfft_2+i*2*height+j))*GLPF(i,j,40);
        }
    }  

//IFFT
	for(int i = 0;i<2*height;i++)
	{
		double retmp[512],imtmp[512],reffttmp[512],imffttmp[512];
		for (int j=0;j<2*width;j++)
		{
			retmp[j] = *(refft_2+2*height*i+j);
			imtmp[j] = -1*(*(imfft_2+2*height*i+j));
		}

		fft(retmp,imtmp,512,9,reffttmp,imffttmp);

		for (int j=0;j<2*width;j++)
		{
			*(refft_1+2*height*i+j) = reffttmp[j];
			*(imfft_1+2*height*i+j) = imffttmp[j];
		}
	}

	for(int i = 0;i<2*height;i++)
	{
		double retmp[512],imtmp[512],reffttmp[512],imffttmp[512];
		for (int j=0;j<2*width;j++)
		{
			retmp[j] = *(refft_1+2*height*j+i);
			imtmp[j] = *(imfft_1+2*height*j+i);
		}

		fft(retmp,imtmp,512,9,reffttmp,imffttmp);

		for (int j=0;j<2*width;j++)
		{
			*(refft_2+2*height*j+i) = reffttmp[j]/(512*512);
			*(imfft_2+2*height*j+i) = imffttmp[j]/(512*512);
		}

	}

	for(int i = 0;i < height;i++)
   	{
		for(int j = 0;j < width;j++)
		{	
			*(tmp+i*lineByte+j) = *(refft_2+2*height*i+j)*pow(-1,i+j);
		}
	}
	
	saveBmp(writepath, tmp);

	free(pBmpBuf);
    free(tmp);
	if (bitcount == 8)
	{
		free(pColorTable);
	}
	return 0;
}
