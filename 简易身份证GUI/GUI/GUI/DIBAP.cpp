// ************************************************************************
//  文件名：dibapi.cpp
//
//  公用 DIB(Independent Bitmap) API函数库：
//
//  PaintDIB()          - 绘制DIB对象
//  CreateDIBPalette()  - 创建DIB对象调色板
//  FindDIBBits()       - 返回DIB图像象素起始位置
//  DIBWidth()          - 返回DIB宽度
//  DIBHeight()         - 返回DIB高度
//  DIBNumColors()      - 计算DIB调色板颜色数目
//  CopyHandle()        - 拷贝内存块
//
//  SaveDIB()           - 将DIB保存到指定文件中
//  ReadDIBFile()       - 重指定文件中读取DIB对象
//  NewDIB()            - 根据提供的宽、高、颜色位数来创建一个新的DIB
//
//  PaletteSize()       - 返回DIB调色板大小
//
// ************************************************************************

#include "stdafx.h"
#include "DIBAPI.h"
#include <io.h>
#include <errno.h>

#include <math.h>
#include <direct.h>
//#include <afxwin.h>

/*
 * Dib文件头标志（字符串"BM"，写DIB时用到该常数）
 */
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

/*************************************************************************
 *
 * 函数名称：
 *   ReadDIBFile()
 *
 * 参数:
 *   CFile& file        - 要读取得文件文件CFile
 *
 * 返回值:
 *   HDIB               - 成功返回DIB的句柄，否则返回NULL。
 *
 * 说明:
 *   该函数将指定的文件中的DIB对象读到指定的内存区域中。除BITMAPFILEHEADER
 * 外的内容都将被读入内存。
 *
 *************************************************************************/

HDIB WINAPI ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	HDIB  hDIB;
	LPSTR pDIB;

	// 获取DIB（文件）长度（字节）
	dwBitsSize = file.GetLength();

	// 尝试读取DIB文件头
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		// 大小不对，返回NULL。
		return NULL;
	}

	// 判断是否是DIB对象，检查头两个字节是否是"BM"
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
	{
		// 非DIB对象，返回NULL。
		return NULL;
	}

	// 为DIB分配内存
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		// 内存分配失败，返回NULL。
		return NULL;
	}
	
	// 锁定
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	// 读象素
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )
	{
		// 大小不对。
		
		// 解除锁定
		::GlobalUnlock((HGLOBAL) hDIB);
		
		// 释放内存
		::GlobalFree((HGLOBAL) hDIB);
		
		// 返回NULL。
		return NULL;
	}
	
	// 解除锁定
	::GlobalUnlock((HGLOBAL) hDIB);
	
	// 返回DIB句柄
	return hDIB;
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBWidth()
 *
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 *
 * 返回值:
 *   DWORD              - DIB中图像的宽度
 *
 * 说明:
 *   该函数返回DIB中图像的宽度。对于Windows 3.0 DIB，返回BITMAPINFOHEADER
 * 中的biWidth值；对于其它返回BITMAPCOREHEADER中的bcWidth值。
 *
 ************************************************************************/
DWORD WINAPI DIBWidth(LPSTR lpDIB)
{
	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPINFOHEADER lpbmi;
	
	// 指向BITMAPCOREINFO结构的指针
	LPBITMAPCOREHEADER lpbmc;

	// 获取指针
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// 返回DIB中图像的宽度
	if (IS_WIN30_DIB(lpDIB))
	{
		// 对于Windows 3.0 DIB，返回lpbmi->biWidth
		return lpbmi->biWidth;
	}
	else
	{
		// 对于其它格式的DIB，返回lpbmc->bcWidth
		return (DWORD)lpbmc->bcWidth;
	}
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBHeight()
 *
 * 参数:
 *   LPSTR lpDIB        - 指向DIB对象的指针
 *
 * 返回值:
 *   DWORD              - DIB中图像的高度
 *
 * 说明:
 *   该函数返回DIB中图像的高度。对于Windows 3.0 DIB，返回BITMAPINFOHEADER
 * 中的biHeight值；对于其它返回BITMAPCOREHEADER中的bcHeight值。
 *
 ************************************************************************/

DWORD WINAPI DIBHeight(LPSTR lpDIB)
{
	 // 指向BITMAPINFO结构的指针（Win3.0）
	 LPBITMAPINFOHEADER lpbmi;
	
	 // 指向BITMAPCOREINFO结构的指针
	 LPBITMAPCOREHEADER lpbmc;

	 // 获取指针
	 lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	 lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	 // 返回DIB中图像的宽度
	 if (IS_WIN30_DIB(lpDIB))
	 {
		 // 对于Windows 3.0 DIB，返回lpbmi->biHeight
		 return lpbmi->biHeight;
	 }
	 else
	 {
		 // 对于其它格式的DIB，返回lpbmc->bcHeight
		 return (DWORD)lpbmc->bcHeight;
	 }
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBNumColors()
 *
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 *
 * 返回值:
 *   WORD               - 返回调色板中颜色的种数
 *
 * 说明:
 *   该函数返回DIB中调色板的颜色的种数。对于单色位图，返回2，
 * 对于16色位图，返回16，对于256色位图，返回256；对于真彩色
 * 位图（24位），没有调色板，返回0。
 *
 ************************************************************************/

WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;

	// 对于Windows的DIB, 实际颜色的数目可以比象素的位数要少。
	// 对于这种情况，则返回一个近似的数值。
	
	// 判断是否是WIN3.0 DIB
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;
		
		// 读取dwClrUsed值
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		
		if (dwClrUsed != 0)
		{
			// 如果dwClrUsed（实际用到的颜色数）不为0，直接返回该值
			return (WORD)dwClrUsed;
		}
	}

	// 读取象素的位数
	if (IS_WIN30_DIB(lpbi))
	{
		// 读取biBitCount值
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	}
	else
	{
		// 读取biBitCount值
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
	}
	
	// 按照象素的位数计算颜色数目
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}


/*************************************************************************
 *
 * 函数名称：
 *   PaletteSize()
 *
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 *
 * 返回值:
 *   WORD               - DIB中调色板的大小
 *
 * 说明:
 *   该函数返回DIB中调色板的大小。对于Windows 3.0 DIB，返回颜色数目×
 * RGBQUAD的大小；对于其它返回颜色数目×RGBTRIPLE的大小。
 *
 ************************************************************************/

WORD WINAPI PaletteSize(LPSTR lpbi)
{
	 // 计算DIB中调色板的大小
	 if (IS_WIN30_DIB (lpbi))
	 {
		 //返回颜色数目×RGBQUAD的大小
		 return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
	 }
	 else
	 {
		 //返回颜色数目×RGBTRIPLE的大小
		 return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
	 }
}

/*************************************************************************
 *
 * 函数名称：
 *   FindDIBBits()
 *
 * 参数:
 *   LPSTR lpbi         - 指向DIB对象的指针
 *
 * 返回值:
 *   LPSTR              - 指向DIB图像象素起始位置
 *
 * 说明:
 *   该函数计算DIB中图像象素的起始位置，并返回指向它的指针。
 *
 ************************************************************************/

LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}

/*************************************************************************
 *
 * 函数名称：
 *   PaintDIB()
 *
 * 参数:
 *   HDC hDC            - 输出设备DC
 *   LPRECT lpDCRect    - 绘制矩形区域
 *   HDIB hDIB          - 指向DIB对象的指针
 *   LPRECT lpDIBRect   - 要输出的DIB区域
 *   CPalette* pPal     - 指向DIB对象调色板的指针
 *
 * 返回值:
 *   BOOL               - 绘制成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数主要用来绘制DIB对象。其中调用了StretchDIBits()或者
 * SetDIBitsToDevice()来绘制DIB对象。输出的设备由由参数hDC指
 * 定；绘制的矩形区域由参数lpDCRect指定；输出DIB的区域由参数
 * lpDIBRect指定。
 *
 ************************************************************************/

BOOL WINAPI PaintDIB(	HDC     hDC,
						LPRECT  lpDCRect,
						HDIB    hDIB,
						LPRECT  lpDIBRect,
						CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // BITMAPINFOHEADER指针
	LPSTR    lpDIBBits;           // DIB象素指针
	BOOL     bSuccess=FALSE;      // 成功标志
	HPALETTE hPal=NULL;           // DIB调色板
	HPALETTE hOldPal=NULL;        // 以前的调色板

	// 判断DIB对象是否为空
	if (hDIB == NULL)
	{
		// 返回
		return FALSE;
	}

	// 锁定DIB
	lpDIBHdr  = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	// 找到DIB图像象素起始位置
	lpDIBBits = ::FindDIBBits(lpDIBHdr);

	// 获取DIB调色板，并选中它
	if (pPal != NULL)
	{
		hPal = (HPALETTE) pPal->m_hObject;

		// 选中调色板
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	// 设置显示模式
	::SetStretchBltMode(hDC, COLORONCOLOR);

	// 判断是调用StretchDIBits()还是SetDIBitsToDevice()来绘制DIB对象
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	{
		// 原始大小，不用拉伸。
		bSuccess = ::SetDIBitsToDevice(hDC,                    // hDC
								   lpDCRect->left,             // DestX
								   lpDCRect->top,              // DestY
								   RECTWIDTH(lpDCRect),        // nDestWidth
								   RECTHEIGHT(lpDCRect),       // nDestHeight
								   lpDIBRect->left,            // SrcX
								   (int)DIBHeight(lpDIBHdr) -
									  lpDIBRect->top -
									  RECTHEIGHT(lpDIBRect),   // SrcY
								   0,                          // nStartScan
								   (WORD)DIBHeight(lpDIBHdr),  // nNumScans
								   lpDIBBits,                  // lpBits
								   (LPBITMAPINFO)lpDIBHdr,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
	}
	else
	{
		// 非原始大小，拉伸。
		bSuccess = ::StretchDIBits(hDC,                          // hDC
							   lpDCRect->left,                 // DestX
							   lpDCRect->top,                  // DestY
							   RECTWIDTH(lpDCRect),            // nDestWidth
							   RECTHEIGHT(lpDCRect),           // nDestHeight
							   lpDIBRect->left,                // SrcX
							   lpDIBRect->top,                 // SrcY
							   RECTWIDTH(lpDIBRect),           // wSrcWidth
							   RECTHEIGHT(lpDIBRect),          // wSrcHeight
							   lpDIBBits,                      // lpBits
							   (LPBITMAPINFO)lpDIBHdr,         // lpBitsInfo
							   DIB_RGB_COLORS,                 // wUsage
							   SRCCOPY);                       // dwROP
	}


	// 解除锁定
	::GlobalUnlock((HGLOBAL) hDIB);

	// 恢复以前的调色板
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}
	
	// 返回
	return bSuccess;
	
}

//清除屏幕
void WINAPI ClearAll(CDC* pDC,CRect dcRect)
{
	CRect rect;
	//GetClientRect(&rect);
	rect.left   = dcRect.left;
	rect.top    = dcRect.top;
	rect.right  = dcRect.right;
	rect.bottom = dcRect.bottom;

	CPen pen;
	pen.CreatePen (PS_SOLID, 1, RGB(255,255,255));
	pDC->SelectObject (&pen);
	pDC->Rectangle (&rect);
	::DeleteObject (pen);
}

/*************************************************************************
 *
 * 函数名称：
 *   DisplayDIB()
 *
 * 参数:
 *   CDC* pDC        - 要显示的位图的设备
 *   HDIB hDIB       - 要显示的DIB句柄
 *
 * 说明:
 *   该函数将指定DIB显示到指定的设备中。
 *
 *************************************************************************/

void WINAPI DisplayDIB(CDC* pDC, HDIB hDIB,CRect rcDest)
{
	BYTE* lpDIB=(BYTE*)::GlobalLock((HGLOBAL)hDIB);
	// 获取DIB宽度和高度
	int cxDIB =  ::DIBWidth((char*) lpDIB);
	int cyDIB =  ::DIBHeight((char*)lpDIB);
	CRect rcDIB;
	rcDIB.top    = rcDIB.left=0;// = 50;
	rcDIB.right  = cxDIB;//+50;
	rcDIB.bottom = cyDIB;//+50;
	//设置目标客户区输出大小尺寸

	//CDC* pDC=GetDC();
    //	ClearAll(pDC);
	//在客户区显示图像

	::PaintDIB(pDC->m_hDC,rcDest,hDIB,rcDIB,NULL);
	::GlobalUnlock((HGLOBAL)hDIB);
}


/*************************************************************************
 *
 * 函数名称：
 *   SaveDIB()
 *
 * 参数:
 *   HDIB hDib          - 要保存的DIB
 *   CFile& file        - 保存文件CFile
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE或者CFileException
 *
 * 说明:
 *   该函数将指定的DIB对象保存到指定的CFile中。该CFile由调用程序打开和关闭。
 *
 *************************************************************************/

BOOL WINAPI SaveDIB(HDIB hDib, CFile& file)
{
	// Bitmap文件头
	BITMAPFILEHEADER bmfHdr;
	
	// 指向BITMAPINFOHEADER的指针
	LPBITMAPINFOHEADER lpBI;
	
	// DIB大小
	DWORD dwDIBSize;

	if (hDib == NULL)
	{
		// 如果DIB为空，返回FALSE
		return FALSE;
	}

	// 读取BITMAPINFO结构，并锁定
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDib);
	
	if (lpBI == NULL)
	{
		// 为空，返回FALSE
		return FALSE;
	}
	
	// 判断是否是WIN3.0 DIB
	if (!IS_WIN30_DIB(lpBI))
	{
		// 不支持其它类型的DIB保存
		
		// 解除锁定
		::GlobalUnlock((HGLOBAL) hDib);
		
		// 返回FALSE
		return FALSE;
	}

	// 填充文件头

	// 文件类型"BM"
	bmfHdr.bfType = DIB_HEADER_MARKER;

	// 计算DIB大小时，最简单的方法是调用GlobalSize()函数。但是全局内存大小并
	// 不是DIB真正的大小，它总是多几个字节。这样就需要计算一下DIB的真实大小。
	
	// 文件头大小＋颜色表大小
	// （BITMAPINFOHEADER和BITMAPCOREHEADER结构的第一个DWORD都是该结构的大小）
	dwDIBSize = *(LPDWORD)lpBI + ::PaletteSize((LPSTR)lpBI);
	
	// 计算图像大小
	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// 对于RLE位图，没法计算大小，只能信任biSizeImage内的值
		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		// 象素的大小
		DWORD dwBmBitsSize;

		// 大小为Width * Height
		dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;
		
		// 计算出DIB真正的大小
		dwDIBSize += dwBmBitsSize;

		// 更新biSizeImage（很多BMP文件头中biSizeImage的值是错误的）
		lpBI->biSizeImage = dwBmBitsSize;
	}


	// 计算文件大小：DIB大小＋BITMAPFILEHEADER结构大小
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	
	// 两个保留字
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	// 计算偏移量bfOffBits，它的大小为Bitmap文件头大小＋DIB头大小＋颜色表大小
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize
											  + PaletteSize((LPSTR)lpBI);
	// 尝试写文件
	TRY
	{
		// 写文件头
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
		
		// 写DIB头和象素
		file.Write(lpBI, dwDIBSize);
	}
	CATCH (CFileException, e)
	{
		// 解除锁定
		::GlobalUnlock((HGLOBAL) hDib);
		
		// 抛出异常
		THROW_LAST();
	}
	END_CATCH
	
	// 解除锁定
	::GlobalUnlock((HGLOBAL) hDib);
	
	// 返回TRUE
	return TRUE;
}

/*************************************************************************
*
* 函数名称：
*   ConvertToGray()
*
* 参数:
*   LPSTR lpDIB			- 指向源DIB图像指针
*   
* 返回值:
*   BOOL				- 成功返回TRUE，否则返回FALSE。
*
* 说明:
*   该函数将24位真彩色图转换成256级灰度图
*
************************************************************************/

BOOL WINAPI ConvertToGray(LPSTR lpDIB)   
{
	
	LPSTR lpDIBBits;                //指向DIB的象素的指针
	LPSTR lpNewDIBBits;             //指向DIB灰度图图像(新图像)开始处象素的指针
    LONG lLineBytes;
	unsigned char * lpSrc;          //指向原图像象素点的指针
	unsigned char * lpdest;         //指向目标图像象素点的指针
	
    unsigned  char *ired,*igreen,*iblue;
	
    long lWidth;                    //图像宽度和高度
	long lHeight;
	long i,j;           //循环变量
	
	lWidth = ::DIBWidth(lpDIB);   //DIB 宽度
	lHeight = ::DIBHeight(lpDIB); //DIB 高度
	RGBQUAD  *lpRGBquad;
	lpRGBquad = (RGBQUAD *)&lpDIB[sizeof(BITMAPINFOHEADER)]; //INFOHEADER后为调色板		
    
	if(::DIBNumColors(lpDIB) == 256)  //256色位图不作任何处理
	{
		return TRUE;
    }
	
	if(::DIBNumColors(lpDIB) != 256)  //非256色位图将它灰度化
	{
		lLineBytes = WIDTHBYTES(lWidth*8*3);
		lpdest = new  BYTE[lHeight*lWidth];
		lpDIBBits = (LPSTR)lpDIB + sizeof(BITMAPINFOHEADER);//指向DIB象素

		for(i=0; i<lHeight; i++)
		{
			for(j=0; j< lWidth*3; j+=3)
			{
				ired   =  (unsigned  char*)lpDIBBits + lLineBytes * i + j + 2;
				igreen =  (unsigned  char*)lpDIBBits + lLineBytes * i + j + 1;
				iblue  =  (unsigned  char*)lpDIBBits + lLineBytes * i +	j ;
				lpdest[i*lWidth + j/3]  = (unsigned  char)((*ired)*0.299 + (*igreen)*0.587 + (*iblue)*0.114);
			}
		}

		//需要做三件事情：1、修改INFOHEADER 2、增加调色板 3、修改原图像灰度值
		LPBITMAPINFOHEADER  lpBI;
		lpBI  =  (LPBITMAPINFOHEADER)lpDIB;
		lpBI->biBitCount = 8;
			
		//设置灰度调色板
		for(i=0; i<256; i++)
		{
			lpRGBquad[i].rgbRed   = (unsigned char)i;
			lpRGBquad[i].rgbGreen = (unsigned char)i;
			lpRGBquad[i].rgbBlue  = (unsigned char)i;
			lpRGBquad[i].rgbReserved = 0;
		}
			
		lpNewDIBBits= ::FindDIBBits(lpDIB);  //找到DIB图像象素起始位置
		lLineBytes=WIDTHBYTES(lWidth  *  8);
		
		//修改灰度值
		for(i=0; i<lHeight; i++)
		{
			for(j=0; j<lWidth; j++)
			{
				lpSrc = (unsigned  char*)lpNewDIBBits + lLineBytes * i+ j ;
				*lpSrc=lpdest[i*lWidth+j];
			}
		}
		delete  lpdest;
	}	
	return true;
}

/*************************************************************************
 *
 * 函数名称：
 *   MedianFilter()
 *
 * 参数:
 *   LPSTR lpDIBBits		- 指向源DIB图像指针
 *   LONG  lWidth			- 源图像宽度（象素数）
 *   LONG  lHeight			- 源图像高度（象素数）
 *   int   iFilterH			- 滤波器的高度
 *   int   iFilterW			- 滤波器的宽度
 *   int   iFilterMX		- 滤波器的中心元素X坐标
 *   int   iFilterMY		- 滤波器的中心元素Y坐标
 *
 * 返回值:
 *   BOOL					- 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数对DIB图像进行中值滤波。
 *
 ************************************************************************/

BOOL WINAPI MedianFilter(LPSTR lpDIBBits,LONG lWidth,LONG lHeight,int iFilterH,int iFilterW,int iFilterMX,int iFilterMY)
{
	// 指向源图像的指针
	unsigned char*	lpSrc;
	
	// 指向要复制区域的指针
	unsigned char*	lpDst;
	
	// 指向复制图像的指针
	LPSTR			lpNewDIBBits;
	HLOCAL			hNewDIBBits;
	
	// 指向滤波器数组的指针
	unsigned char	* aValue;
	HLOCAL			hArray;
	
	// 循环变量
	LONG			i;
	LONG			j;
	LONG			k;
	LONG			l;
	
	// 图像每行的字节数
	LONG			lLineBytes;
	
	// 计算图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// 暂时分配内存，以保存新图像
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	
	// 判断是否内存分配失败
	if (hNewDIBBits == NULL)
	{
		// 分配内存失败
		return FALSE;
	}
	
	// 锁定内存
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// 初始化图像为原始图像
	memcpy(lpNewDIBBits, lpDIBBits, lLineBytes * lHeight);
	
	// 暂时分配内存，以保存滤波器数组
	hArray = LocalAlloc(LHND, iFilterH * iFilterW);
	
	// 判断是否内存分配失败
	if (hArray == NULL)
	{
		// 释放内存
		LocalUnlock(hNewDIBBits);
		LocalFree(hNewDIBBits);
		
		// 分配内存失败
		return FALSE;
	}
	
	// 锁定内存
	aValue = (unsigned char * )LocalLock(hArray);
	
	// 开始中值滤波
	// 行(除去边缘几行)
	for(i = iFilterMY; i < lHeight - iFilterH + iFilterMY + 1; i++)
	{
		// 列(除去边缘几列)
		for(j = iFilterMX; j < lWidth - iFilterW + iFilterMX + 1; j++)
		{
			// 指向新DIB第i行，第j个象素的指针
			lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 读取滤波器数组
			for (k = 0; k < iFilterH; k++)
			{
				for (l = 0; l < iFilterW; l++)
				{
					// 指向DIB第i - iFilterMY + k行，第j - iFilterMX + l个象素的指针
					lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i + iFilterMY - k) + j - iFilterMX + l;
					
					// 保存象素值
					aValue[k * iFilterW + l] = *lpSrc;
				}
			}
			
			// 获取中值
			* lpDst = GetMedianNum(aValue, iFilterH * iFilterW);
		}
	}
	
	// 复制变换后的图像
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	// 释放内存
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	LocalUnlock(hArray);
	LocalFree(hArray);
	
	// 返回
	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   GetMedianNum()
 *
 * 参数:
 *   unsigned char * bpArray	- 指向要获取中值的数组指针
 *   int   iFilterLen			- 数组长度
 *
 * 返回值:
 *   unsigned char      - 返回指定数组的中值。
 *
 * 说明:
 *   该函数用冒泡法对一维数组进行排序，并返回数组元素的中值。
 *
 ************************************************************************/

unsigned char WINAPI GetMedianNum(unsigned char * bArray, int iFilterLen)
{
	// 循环变量
	int		i;
	int		j;
	
	// 中间变量
	unsigned char bTemp;
	
	// 用冒泡法对数组进行排序
	for (j = 0; j < iFilterLen - 1; j ++)
	{
		for (i = 0; i < iFilterLen - j - 1; i ++)
		{
			if (bArray[i] > bArray[i + 1])
			{
				// 互换
				bTemp = bArray[i];
				bArray[i] = bArray[i + 1];
				bArray[i + 1] = bTemp;
			}
		}
	}
	
	// 计算中值
	if ((iFilterLen & 1) > 0)
	{
		// 数组有奇数个元素，返回中间一个元素
		bTemp = bArray[(iFilterLen + 1) / 2];
	}
	else
	{
		// 数组有偶数个元素，返回中间两个元素平均值
		bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
	}
	
	// 返回中值
	return bTemp;
}

/*************************************************************************
 *
 * 函数名称：
 *   ClearBlackBorder()
 *
 * 参数:
 *   LPSTR  lpDIBBits		 - 指向源DIB图像指针
 *   LONG   lWidth			 - 源图像宽度（象素数）
 *   LONG   lHeight			 - 源图像高度（象素数）
 *   double fK               - 去掉黑色区域的灰度调整因子
 *   double fAverGray        - 平均灰度
 *
 * 返回值:
 *   BOOL					 - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数去掉采集图像时可能形成的黑边 
 *
 ************************************************************************/

BOOL WINAPI ClearBlackBorder(	HDIB   hDIB,
								double dBlackWidth,
								double fK,//dGrayThreshold,
								double dAverGray)
{

	if(!hDIB)
		return FALSE;
	
	//指向原图像的指针
	unsigned char*  lpSrc;

	//指向DIB的指针
	LPSTR lpDIB;

	//指向DIB像素指针
	LPSTR lpDIBBits;

	long lWidth;   //源图像宽度（象素数）

	long lHeight;  //源图像高度（象素数）


	//锁定DIB
	lpDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);

	if (::DIBNumColors(lpDIB)!=256) 
	{
		::AfxMessageBox("对不起，现在只能处理灰度图",MB_OK,0);

		::GlobalUnlock((HGLOBAL)hDIB);
		return FALSE;
	}

	lpDIBBits=::FindDIBBits(lpDIB);

	lWidth=::DIBWidth(lpDIB);  
	lHeight=::DIBHeight(lpDIB);

	//图像每行的字节数
	LONG lLineBytes;

	//计算图像每行的字节数
	lLineBytes=WIDTHBYTES(lWidth*8);

	for(int i=0; i<lHeight; i++)     //行
	{
		for(int j=0; j<lWidth; j++)  //列
		{
			lpSrc=(unsigned char*)lpDIBBits+lLineBytes*(lHeight-1-i)+j;  //一般来说，BMP文件得数据是从下到上，
			                                        //从左到右的。即从文件中最先读到的是
			if((i<dBlackWidth)||(i>lHeight-dBlackWidth)||
				(j<dBlackWidth)||(j>lWidth-dBlackWidth))
				*lpSrc=(unsigned char)(fK*dAverGray+0.5); 
		}
	}

	::GlobalUnlock((HGLOBAL)hDIB);	

	//返回
	return TRUE;

}

/*************************************************************************
 *
 * 函数名称：
 *   ClearBackGround()
 *
 * 参数:
 *   HDIB  hDIB		        - 指向源DIB句柄
 *   double  lA			    - 圆心的大致位置
 *   double  lB			    - 圆心的大致位置
 *   double fK               -去掉黑色区域的灰度调整因子
 *   double   lR              -光靶有效区域的最大半径，大于该值为需要灰度重建部分
 *   double fAverGray        -平均灰度
 *
 * 返回值:
 *   BOOL					- 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   图像预处理:去掉黑色背景区域
 *
 ************************************************************************/

BOOL WINAPI ClearBackGround(HDIB   hDIB,
							double dCenterH,
							double dCenterV,
							double dMaxR,
							double fK,
							double dAverGray)
{

	if(!hDIB)
		return FALSE;
	
	//指向原图像的指针
	unsigned char*  lpSrc;

	//指向DIB的指针
	LPSTR lpDIB;

	//指向DIB像素指针
	LPSTR lpDIBBits;

	long lWidth;   //源图像宽度（象素数）

	long lHeight;  //源图像高度（象素数）


	//锁定DIB
	lpDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);

	if (::DIBNumColors(lpDIB)!=256) 
	{
		::AfxMessageBox("对不起，现在只能处理灰度图",MB_OK,0);

		::GlobalUnlock((HGLOBAL)hDIB);
		return FALSE;
	}

	lpDIBBits=::FindDIBBits(lpDIB);

	lWidth=::DIBWidth(lpDIB);  
	lHeight=::DIBHeight(lpDIB);


	//图像每行的字节数
	LONG lLineBytes;

	//中间变量
	float fDistTemp;

	//计算图像每行的字节数
	lLineBytes=WIDTHBYTES(lWidth*8);

	for(int i=0; i<lHeight; i++)//行
	{
		for(int j=0; j<lWidth; j++)//列
		{

		    lpSrc=(unsigned char*)lpDIBBits+lLineBytes*(lHeight-1-i)+j;
			fDistTemp = (float)((i-dCenterV)*(i-dCenterV)+(j-dCenterH)*(j-dCenterH));
			fDistTemp = (float)sqrt(fDistTemp);
			if(fDistTemp>dMaxR)
		        *lpSrc=(unsigned char)(fK*dAverGray+0.5);
		}
	}

	::GlobalUnlock((HGLOBAL)hDIB);

	//返回
	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   CopyHandle()
 *
 * 参数:
 *   HGLOBAL h          - 要复制的内存区域
 *
 * 返回值:
 *   HGLOBAL            - 复制后的新内存区域
 *
 * 说明:
 *   该函数复制指定的内存区域。返回复制后的新内存区域，出错时返回0。
 *
 ************************************************************************/

HGLOBAL WINAPI CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	// 获取指定内存区域大小
	DWORD dwLen = ::GlobalSize((HGLOBAL) h);
	
	// 分配新内存空间
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);
	
	// 判断分配是否成功
	if (hCopy != NULL)
	{
		// 锁定
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		
		// 复制
		memcpy(lpCopy, lp, dwLen);
		
		// 解除锁定
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

