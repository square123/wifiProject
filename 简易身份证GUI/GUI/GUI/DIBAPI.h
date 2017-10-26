//dibapi.h

#include <afxwin.h>
#ifndef _INC_DIBAPI
#define _INC_DIBAPI

//DIB句柄
DECLARE_HANDLE(HDIB);

//DIB常量
#define PALVERSION 0x300

//判断是否是Win3.0的DIB
#define IS_WIN30_DIB(lpbi)   ((*(LPDWORD)(lpbi))==sizeof(BITMAPINFOHEADER))

//计算矩形区域的宽度
#define RECTWIDTH(lpRect)    ((lpRect)->right-(lpRect)->left)

//计算矩形区域的高度
#define RECTHEIGHT(lpRect)   ((lpRect)->bottom-(lpRect)->top)

//在计算图像大小时，采用公式：biSizeImage=biWidth'×biHeight.
//是biWidth',而不是biWidth,这里的biWidth'必须是4的整数倍，表示大于或等于
//biWidth的，离4最近的整数倍。WIDTHBYTES就是用来计算biWidth'
#define WIDTHBYTES(bits)   (((bits)+31)/32*4)
//#define WIDTHBYTES(bits)   ((bits)/32*4)

//函数原型

//读取bmp文件
HDIB WINAPI ReadDIBFile(CFile& file);

//返回DIB宽度
DWORD WINAPI DIBWidth(LPSTR lpDIB);

//返回DIB高度
DWORD WINAPI DIBHeight(LPSTR lpDIB);

//计算DIB调色板颜色数目
WORD WINAPI DIBNumColors(LPSTR lpbi);

//返回DIB调色板大小
WORD WINAPI PaletteSize(LPSTR lpbi);

//返回DIB图像象素起始位置
LPSTR WINAPI FindDIBBits(LPSTR lpbi);

//绘制DIB对象
BOOL WINAPI PaintDIB(	HDC      hDC,
					 LPRECT   lpDCRect,
					 HDIB     hDIB,
					 LPRECT   lpDIBRect,
					 CPalette *pPal);

//清楚屏幕
void WINAPI ClearAll(CDC* pDC,CRect dcRect);

//在屏幕上显示位图
void WINAPI DisplayDIB(CDC* pDC, HDIB hDIB,CRect rcDest);

//将DIB保存到指定文件中
BOOL WINAPI SaveDIB (HDIB hDib, CFile& file);

//将24位彩色图像转换为灰度图像
BOOL WINAPI ConvertToGray(LPSTR lpDIB);

//中值滤波
BOOL WINAPI MedianFilter(	LPSTR lpDIBBits,
						 LONG lWidth,
						 LONG lHeight,
						 int iFilterH,
						 int iFilterW,
						 int iFilterMX,
						 int iFilterMY);

//获取中值滤波的中值
unsigned char WINAPI GetMedianNum(unsigned char * bArray,
								  int iFilterLen);

//消除黑边
BOOL WINAPI ClearBlackBorder(	HDIB   hDIB,
							 double dBlackWidth,
							 double dGrayThreshold,
							 double dAverGray);

//消除黑色背景区域图像
BOOL WINAPI ClearBackGround(HDIB   hDIB,
							double dCenterH,
							double dCenterV,
							double dMaxR,
							double fK,
							double dAverGray);



//复制DIB句柄
HGLOBAL WINAPI CopyHandle(HGLOBAL h);

#endif//!_INC_DIBAPI