#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <windows.h>
#include "Jerror.h"

//================================================
typedef struct {
    unsigned char id1;
    unsigned char id2;  //1
//    char id[2];
    int filesize;
    short reserved[2];
    int headersize;     //10
    int infoSize;
    int width;
    int depth;
    short biPlanes;    //22
    short bits;
    int biCompression;   //26
    int biSizeImage;      //30
    int biXPelsPerMeter;   //34
    int biYPelsPerMeter;   //38
    int biClrUsed;
    int biClrImportant;
} _BMPHEAD;
_BMPHEAD Bmpheader;
//_BMPHEAD Bmp256header={66,77,196662,0,0,54,40,256,256,1,24,0,196608,0,0,0,0};
BYTE Bmpbyte[54];

//int bmp_cnt=0;
//char bmp_name[256][256];

BYTE *LoadBmp(char *name,int *xl,int *yl)
{
	FILE *fp;
    int i,xleng,yleng,sparexl;
    BYTE *trbuf;

    if(name[0]==NULL)
	{
		Warning(name,"null file!");
        return(0);
	}
	fp=fopen(name,"rb");
    if(fp==NULL)
	{
		Warning(name," 요파일이 없는디!");
        return(0);
	}
//    fseek(fp,54,SEEK_SET);
    fread(Bmpbyte,54,1,fp);
	if(Bmpbyte[0x1c]!=24)
	{
		fclose(fp);
		Warning(name," 어허 요파일이 24bit가 아니구나!");
		return(0);
	}
	memcpy(&Bmpheader,Bmpbyte,54);
//    fread(&Bmpheader,54,1,fp);
	Bmpheader.width=(Bmpbyte[0x13]<<8)+Bmpbyte[0x12];
	Bmpheader.depth=(Bmpbyte[0x17]<<8)+Bmpbyte[0x16];
    *xl=Bmpheader.width-1;
    *yl=Bmpheader.depth-1;
    xleng=Bmpheader.width;
    yleng=Bmpheader.depth;
//    xleng=640;
//    yleng=480;
    trbuf=(BYTE *)malloc((xleng)*(yleng)*3);
//    fread(trbuf,xleng*yleng*3,1,fp);
    sparexl=((xleng*3+3)&~3);
    for(i=0;i<yleng;i++)
    {
        fread(&trbuf[i*xleng*3],xleng*3,1,fp);
        fseek(fp,sparexl-xleng*3,SEEK_CUR);
    }
	fclose(fp);

    return(trbuf);
}
/*
WORD *LoadBmpInverse(char *name,int *xl,int *yl)
{
	FILE *fp;
    int i,xleng,yleng,sparexl;
    BYTE *trbuf,im[54];
    WORD gop,*hibuf;

    fp=fopen(name,"rb");
    if(fp==NULL)
	{
		Warning(name," 요파일이 없는디!");
        return(0);
	}
//    fseek(fp,54,SEEK_SET);
    fread(Bmpbyte,54,1,fp);
	if(Bmpbyte[0x1c]!=24)
	{
		fclose(fp);
		Warning(name," 어허 요파일이 24bit가 아니구나!");
		return(0);
	}
	memcpy(&Bmpheader,Bmpbyte,54);
//    fread(&Bmpheader,54,1,fp);
	Bmpheader.width=(Bmpbyte[0x13]<<8)+Bmpbyte[0x12];
	Bmpheader.depth=(Bmpbyte[0x17]<<8)+Bmpbyte[0x16];
    *xl=Bmpheader.width-1;
    *yl=Bmpheader.depth-1;
    xleng=Bmpheader.width;
    yleng=Bmpheader.depth;
//    xleng=640;
//    yleng=480;
    trbuf=(BYTE *)malloc((xleng)*(yleng)*3);
//    fread(trbuf,xleng*yleng*3,1,fp);
    sparexl=((xleng*3+3)&~3);
    for(i=0;i<yleng;i++)
    {
        fread(&trbuf[i*xleng*3],xleng*3,1,fp);
        fseek(fp,sparexl-xleng*3,SEEK_CUR);
    }
	fclose(fp);
    return(trbuf);
}
*/
void SaveBmp16(char *name,int xl,int yl,WORD *hibuf)
{
	FILE *fp;
    int i,xleng,yleng,size,sparexl,diff,j;
    BYTE *trbuf;

    memset(Bmpbyte,54,0);
    Bmpbyte[0x0]='B';
    Bmpbyte[0x1]='M';
    Bmpbyte[0x1c]=24;
    Bmpbyte[0x0a]=54;

	Bmpbyte[0x12]=xl&0xff;
	Bmpbyte[0x13]=(xl>>8)&0xff;
	Bmpbyte[0x16]=yl&0xff;
	Bmpbyte[0x17]=(yl>>8)&0xff;
    Bmpbyte[38]=0x12;
    Bmpbyte[39]=0xb;
    Bmpbyte[42]=0x12;
    Bmpbyte[43]=0xb;
    Bmpbyte[26]=1;
    Bmpbyte[14]=40;


    diff=((xl+3)&3);
    sparexl=((xl*3+3)&~3);
    size=sparexl*yl;
	Bmpbyte[0x34]=size&0xff;
	Bmpbyte[0x35]=(size>>8)&0xff;
	Bmpbyte[0x36]=(size>>16)&0xff;
	Bmpbyte[0x37]=(size>>24)&0xff;
    size=sparexl*yl+54;
	Bmpbyte[0x2]=size&0xff;
	Bmpbyte[0x3]=(size>>8)&0xff;
	Bmpbyte[0x4]=(size>>16)&0xff;
	Bmpbyte[0x5]=(size>>24)&0xff;
    Bmpheader.width=xl;
    Bmpheader.depth=yl;
    xleng=Bmpheader.width;
    yleng=Bmpheader.depth;
    trbuf=(BYTE *)malloc((xleng)*(yleng)*3);
    for(i=0;i<yleng;i++)
	{
		for(j=0;j<xl;j++)
		{
			trbuf[(-i+yleng-1)*sparexl+j*3+0]=((hibuf[i*xleng+j]>>0)&31)*8;
			trbuf[(-i+yleng-1)*sparexl+j*3+1]=((hibuf[i*xleng+j]>>5)&31)*8;
			trbuf[(-i+yleng-1)*sparexl+j*3+2]=((hibuf[i*xleng+j]>>10)&31)*8;
		}
	}

    fp=fopen(name,"wb");
    fwrite(Bmpbyte,54,1,fp);
    for(i=0;i<yleng;i++)
	    fwrite(&trbuf[(i)*sparexl],sparexl,1,fp);
	fclose(fp);
    free(trbuf);
}

void SaveBmp24(char *name,int xl,int yl,BYTE *buf)
{
	FILE *fp;
    int i,xleng,yleng,size,sparexl,diff;
    BYTE *trbuf;

    memset(Bmpbyte,54,0);
    Bmpbyte[0x0]='B';
    Bmpbyte[0x1]='M';
    Bmpbyte[0x1c]=24;
    Bmpbyte[0x0a]=54;

	Bmpbyte[0x12]=xl&0xff;
	Bmpbyte[0x13]=(xl>>8)&0xff;
	Bmpbyte[0x16]=yl&0xff;
	Bmpbyte[0x17]=(yl>>8)&0xff;
    Bmpbyte[38]=0x12;
    Bmpbyte[39]=0xb;
    Bmpbyte[42]=0x12;
    Bmpbyte[43]=0xb;
    Bmpbyte[26]=1;
    Bmpbyte[14]=40;


    diff=((xl+3)&3);
    sparexl=((xl*3+3)&~3);
    size=sparexl*yl;
	Bmpbyte[0x34]=size&0xff;
	Bmpbyte[0x35]=(size>>8)&0xff;
	Bmpbyte[0x36]=(size>>16)&0xff;
	Bmpbyte[0x37]=(size>>24)&0xff;
    size=sparexl*yl+54;
	Bmpbyte[0x2]=size&0xff;
	Bmpbyte[0x3]=(size>>8)&0xff;
	Bmpbyte[0x4]=(size>>16)&0xff;
	Bmpbyte[0x5]=(size>>24)&0xff;
    Bmpheader.width=xl;
    Bmpheader.depth=yl;
    xleng=Bmpheader.width;
    yleng=Bmpheader.depth;
    trbuf=(BYTE *)malloc((xleng)*(yleng)*5);
    for(i=0;i<yleng;i++)
		memcpy(&trbuf[(-i+yleng-1)*sparexl],&buf[i*xleng*3],xl*3);
//		memcpy(&trbuf[(yleng-1-i)*sparexl],&buf[i*xleng*3],xl*3);

    fp=fopen(name,"wb");
    fwrite(Bmpbyte,54,1,fp);
//    fwrite(&Bmpheader,54,1,fp);
//    fwrite(xleng,4,1,fp);
//    fwrite(yleng,4,1,fp);
    for(i=0;i<yleng;i++)
	    fwrite(&trbuf[(i)*sparexl],sparexl,1,fp);
	fclose(fp);
    free(trbuf);
}

