// ************************************************************************
//  �ļ�����dibapi.cpp
//
//  ���� DIB(Independent Bitmap) API�����⣺
//
//  PaintDIB()          - ����DIB����
//  CreateDIBPalette()  - ����DIB�����ɫ��
//  FindDIBBits()       - ����DIBͼ��������ʼλ��
//  DIBWidth()          - ����DIB���
//  DIBHeight()         - ����DIB�߶�
//  DIBNumColors()      - ����DIB��ɫ����ɫ��Ŀ
//  CopyHandle()        - �����ڴ��
//
//  SaveDIB()           - ��DIB���浽ָ���ļ���
//  ReadDIBFile()       - ��ָ���ļ��ж�ȡDIB����
//  NewDIB()            - �����ṩ�Ŀ��ߡ���ɫλ��������һ���µ�DIB
//
//  PaletteSize()       - ����DIB��ɫ���С
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
 * Dib�ļ�ͷ��־���ַ���"BM"��дDIBʱ�õ��ó�����
 */
#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

/*************************************************************************
 *
 * �������ƣ�
 *   ReadDIBFile()
 *
 * ����:
 *   CFile& file        - Ҫ��ȡ���ļ��ļ�CFile
 *
 * ����ֵ:
 *   HDIB               - �ɹ�����DIB�ľ�������򷵻�NULL��
 *
 * ˵��:
 *   �ú�����ָ�����ļ��е�DIB�������ָ�����ڴ������С���BITMAPFILEHEADER
 * ������ݶ����������ڴ档
 *
 *************************************************************************/

HDIB WINAPI ReadDIBFile(CFile& file)
{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	HDIB  hDIB;
	LPSTR pDIB;

	// ��ȡDIB���ļ������ȣ��ֽڣ�
	dwBitsSize = file.GetLength();

	// ���Զ�ȡDIB�ļ�ͷ
	if (file.Read((LPSTR)&bmfHeader, sizeof(bmfHeader)) != sizeof(bmfHeader))
	{
		// ��С���ԣ�����NULL��
		return NULL;
	}

	// �ж��Ƿ���DIB���󣬼��ͷ�����ֽ��Ƿ���"BM"
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
	{
		// ��DIB���󣬷���NULL��
		return NULL;
	}

	// ΪDIB�����ڴ�
	hDIB = (HDIB) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (hDIB == 0)
	{
		// �ڴ����ʧ�ܣ�����NULL��
		return NULL;
	}
	
	// ����
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	// ������
	if (file.Read(pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER)) !=
		dwBitsSize - sizeof(BITMAPFILEHEADER) )
	{
		// ��С���ԡ�
		
		// �������
		::GlobalUnlock((HGLOBAL) hDIB);
		
		// �ͷ��ڴ�
		::GlobalFree((HGLOBAL) hDIB);
		
		// ����NULL��
		return NULL;
	}
	
	// �������
	::GlobalUnlock((HGLOBAL) hDIB);
	
	// ����DIB���
	return hDIB;
}

/*************************************************************************
 *
 * �������ƣ�
 *   DIBWidth()
 *
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 *
 * ����ֵ:
 *   DWORD              - DIB��ͼ��Ŀ��
 *
 * ˵��:
 *   �ú�������DIB��ͼ��Ŀ�ȡ�����Windows 3.0 DIB������BITMAPINFOHEADER
 * �е�biWidthֵ��������������BITMAPCOREHEADER�е�bcWidthֵ��
 *
 ************************************************************************/
DWORD WINAPI DIBWidth(LPSTR lpDIB)
{
	// ָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	LPBITMAPINFOHEADER lpbmi;
	
	// ָ��BITMAPCOREINFO�ṹ��ָ��
	LPBITMAPCOREHEADER lpbmc;

	// ��ȡָ��
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	// ����DIB��ͼ��Ŀ��
	if (IS_WIN30_DIB(lpDIB))
	{
		// ����Windows 3.0 DIB������lpbmi->biWidth
		return lpbmi->biWidth;
	}
	else
	{
		// ����������ʽ��DIB������lpbmc->bcWidth
		return (DWORD)lpbmc->bcWidth;
	}
}

/*************************************************************************
 *
 * �������ƣ�
 *   DIBHeight()
 *
 * ����:
 *   LPSTR lpDIB        - ָ��DIB�����ָ��
 *
 * ����ֵ:
 *   DWORD              - DIB��ͼ��ĸ߶�
 *
 * ˵��:
 *   �ú�������DIB��ͼ��ĸ߶ȡ�����Windows 3.0 DIB������BITMAPINFOHEADER
 * �е�biHeightֵ��������������BITMAPCOREHEADER�е�bcHeightֵ��
 *
 ************************************************************************/

DWORD WINAPI DIBHeight(LPSTR lpDIB)
{
	 // ָ��BITMAPINFO�ṹ��ָ�루Win3.0��
	 LPBITMAPINFOHEADER lpbmi;
	
	 // ָ��BITMAPCOREINFO�ṹ��ָ��
	 LPBITMAPCOREHEADER lpbmc;

	 // ��ȡָ��
	 lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	 lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	 // ����DIB��ͼ��Ŀ��
	 if (IS_WIN30_DIB(lpDIB))
	 {
		 // ����Windows 3.0 DIB������lpbmi->biHeight
		 return lpbmi->biHeight;
	 }
	 else
	 {
		 // ����������ʽ��DIB������lpbmc->bcHeight
		 return (DWORD)lpbmc->bcHeight;
	 }
}

/*************************************************************************
 *
 * �������ƣ�
 *   DIBNumColors()
 *
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 *
 * ����ֵ:
 *   WORD               - ���ص�ɫ������ɫ������
 *
 * ˵��:
 *   �ú�������DIB�е�ɫ�����ɫ�����������ڵ�ɫλͼ������2��
 * ����16ɫλͼ������16������256ɫλͼ������256���������ɫ
 * λͼ��24λ����û�е�ɫ�壬����0��
 *
 ************************************************************************/

WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;

	// ����Windows��DIB, ʵ����ɫ����Ŀ���Ա����ص�λ��Ҫ�١�
	// ��������������򷵻�һ�����Ƶ���ֵ��
	
	// �ж��Ƿ���WIN3.0 DIB
	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;
		
		// ��ȡdwClrUsedֵ
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		
		if (dwClrUsed != 0)
		{
			// ���dwClrUsed��ʵ���õ�����ɫ������Ϊ0��ֱ�ӷ��ظ�ֵ
			return (WORD)dwClrUsed;
		}
	}

	// ��ȡ���ص�λ��
	if (IS_WIN30_DIB(lpbi))
	{
		// ��ȡbiBitCountֵ
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	}
	else
	{
		// ��ȡbiBitCountֵ
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
	}
	
	// �������ص�λ��������ɫ��Ŀ
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
 * �������ƣ�
 *   PaletteSize()
 *
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 *
 * ����ֵ:
 *   WORD               - DIB�е�ɫ��Ĵ�С
 *
 * ˵��:
 *   �ú�������DIB�е�ɫ��Ĵ�С������Windows 3.0 DIB��������ɫ��Ŀ��
 * RGBQUAD�Ĵ�С����������������ɫ��Ŀ��RGBTRIPLE�Ĵ�С��
 *
 ************************************************************************/

WORD WINAPI PaletteSize(LPSTR lpbi)
{
	 // ����DIB�е�ɫ��Ĵ�С
	 if (IS_WIN30_DIB (lpbi))
	 {
		 //������ɫ��Ŀ��RGBQUAD�Ĵ�С
		 return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
	 }
	 else
	 {
		 //������ɫ��Ŀ��RGBTRIPLE�Ĵ�С
		 return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
	 }
}

/*************************************************************************
 *
 * �������ƣ�
 *   FindDIBBits()
 *
 * ����:
 *   LPSTR lpbi         - ָ��DIB�����ָ��
 *
 * ����ֵ:
 *   LPSTR              - ָ��DIBͼ��������ʼλ��
 *
 * ˵��:
 *   �ú�������DIB��ͼ�����ص���ʼλ�ã�������ָ������ָ�롣
 *
 ************************************************************************/

LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}

/*************************************************************************
 *
 * �������ƣ�
 *   PaintDIB()
 *
 * ����:
 *   HDC hDC            - ����豸DC
 *   LPRECT lpDCRect    - ���ƾ�������
 *   HDIB hDIB          - ָ��DIB�����ָ��
 *   LPRECT lpDIBRect   - Ҫ�����DIB����
 *   CPalette* pPal     - ָ��DIB�����ɫ���ָ��
 *
 * ����ֵ:
 *   BOOL               - ���Ƴɹ�����TRUE�����򷵻�FALSE��
 *
 * ˵��:
 *   �ú�����Ҫ��������DIB�������е�����StretchDIBits()����
 * SetDIBitsToDevice()������DIB����������豸���ɲ���hDCָ
 * �������Ƶľ��������ɲ���lpDCRectָ�������DIB�������ɲ���
 * lpDIBRectָ����
 *
 ************************************************************************/

BOOL WINAPI PaintDIB(	HDC     hDC,
						LPRECT  lpDCRect,
						HDIB    hDIB,
						LPRECT  lpDIBRect,
						CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // BITMAPINFOHEADERָ��
	LPSTR    lpDIBBits;           // DIB����ָ��
	BOOL     bSuccess=FALSE;      // �ɹ���־
	HPALETTE hPal=NULL;           // DIB��ɫ��
	HPALETTE hOldPal=NULL;        // ��ǰ�ĵ�ɫ��

	// �ж�DIB�����Ƿ�Ϊ��
	if (hDIB == NULL)
	{
		// ����
		return FALSE;
	}

	// ����DIB
	lpDIBHdr  = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

	// �ҵ�DIBͼ��������ʼλ��
	lpDIBBits = ::FindDIBBits(lpDIBHdr);

	// ��ȡDIB��ɫ�壬��ѡ����
	if (pPal != NULL)
	{
		hPal = (HPALETTE) pPal->m_hObject;

		// ѡ�е�ɫ��
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	// ������ʾģʽ
	::SetStretchBltMode(hDC, COLORONCOLOR);

	// �ж��ǵ���StretchDIBits()����SetDIBitsToDevice()������DIB����
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
	{
		// ԭʼ��С���������졣
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
		// ��ԭʼ��С�����졣
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


	// �������
	::GlobalUnlock((HGLOBAL) hDIB);

	// �ָ���ǰ�ĵ�ɫ��
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}
	
	// ����
	return bSuccess;
	
}

//�����Ļ
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
 * �������ƣ�
 *   DisplayDIB()
 *
 * ����:
 *   CDC* pDC        - Ҫ��ʾ��λͼ���豸
 *   HDIB hDIB       - Ҫ��ʾ��DIB���
 *
 * ˵��:
 *   �ú�����ָ��DIB��ʾ��ָ�����豸�С�
 *
 *************************************************************************/

void WINAPI DisplayDIB(CDC* pDC, HDIB hDIB,CRect rcDest)
{
	BYTE* lpDIB=(BYTE*)::GlobalLock((HGLOBAL)hDIB);
	// ��ȡDIB��Ⱥ͸߶�
	int cxDIB =  ::DIBWidth((char*) lpDIB);
	int cyDIB =  ::DIBHeight((char*)lpDIB);
	CRect rcDIB;
	rcDIB.top    = rcDIB.left=0;// = 50;
	rcDIB.right  = cxDIB;//+50;
	rcDIB.bottom = cyDIB;//+50;
	//����Ŀ��ͻ��������С�ߴ�

	//CDC* pDC=GetDC();
    //	ClearAll(pDC);
	//�ڿͻ�����ʾͼ��

	::PaintDIB(pDC->m_hDC,rcDest,hDIB,rcDIB,NULL);
	::GlobalUnlock((HGLOBAL)hDIB);
}


/*************************************************************************
 *
 * �������ƣ�
 *   SaveDIB()
 *
 * ����:
 *   HDIB hDib          - Ҫ�����DIB
 *   CFile& file        - �����ļ�CFile
 *
 * ����ֵ:
 *   BOOL               - �ɹ�����TRUE�����򷵻�FALSE����CFileException
 *
 * ˵��:
 *   �ú�����ָ����DIB���󱣴浽ָ����CFile�С���CFile�ɵ��ó���򿪺͹رա�
 *
 *************************************************************************/

BOOL WINAPI SaveDIB(HDIB hDib, CFile& file)
{
	// Bitmap�ļ�ͷ
	BITMAPFILEHEADER bmfHdr;
	
	// ָ��BITMAPINFOHEADER��ָ��
	LPBITMAPINFOHEADER lpBI;
	
	// DIB��С
	DWORD dwDIBSize;

	if (hDib == NULL)
	{
		// ���DIBΪ�գ�����FALSE
		return FALSE;
	}

	// ��ȡBITMAPINFO�ṹ��������
	lpBI = (LPBITMAPINFOHEADER) ::GlobalLock((HGLOBAL) hDib);
	
	if (lpBI == NULL)
	{
		// Ϊ�գ�����FALSE
		return FALSE;
	}
	
	// �ж��Ƿ���WIN3.0 DIB
	if (!IS_WIN30_DIB(lpBI))
	{
		// ��֧���������͵�DIB����
		
		// �������
		::GlobalUnlock((HGLOBAL) hDib);
		
		// ����FALSE
		return FALSE;
	}

	// ����ļ�ͷ

	// �ļ�����"BM"
	bmfHdr.bfType = DIB_HEADER_MARKER;

	// ����DIB��Сʱ����򵥵ķ����ǵ���GlobalSize()����������ȫ���ڴ��С��
	// ����DIB�����Ĵ�С�������Ƕ༸���ֽڡ���������Ҫ����һ��DIB����ʵ��С��
	
	// �ļ�ͷ��С����ɫ���С
	// ��BITMAPINFOHEADER��BITMAPCOREHEADER�ṹ�ĵ�һ��DWORD���Ǹýṹ�Ĵ�С��
	dwDIBSize = *(LPDWORD)lpBI + ::PaletteSize((LPSTR)lpBI);
	
	// ����ͼ���С
	if ((lpBI->biCompression == BI_RLE8) || (lpBI->biCompression == BI_RLE4))
	{
		// ����RLEλͼ��û�������С��ֻ������biSizeImage�ڵ�ֵ
		dwDIBSize += lpBI->biSizeImage;
	}
	else
	{
		// ���صĴ�С
		DWORD dwBmBitsSize;

		// ��СΪWidth * Height
		dwBmBitsSize = WIDTHBYTES((lpBI->biWidth)*((DWORD)lpBI->biBitCount)) * lpBI->biHeight;
		
		// �����DIB�����Ĵ�С
		dwDIBSize += dwBmBitsSize;

		// ����biSizeImage���ܶ�BMP�ļ�ͷ��biSizeImage��ֵ�Ǵ���ģ�
		lpBI->biSizeImage = dwBmBitsSize;
	}


	// �����ļ���С��DIB��С��BITMAPFILEHEADER�ṹ��С
	bmfHdr.bfSize = dwDIBSize + sizeof(BITMAPFILEHEADER);
	
	// ����������
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;

	// ����ƫ����bfOffBits�����Ĵ�СΪBitmap�ļ�ͷ��С��DIBͷ��С����ɫ���С
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + lpBI->biSize
											  + PaletteSize((LPSTR)lpBI);
	// ����д�ļ�
	TRY
	{
		// д�ļ�ͷ
		file.Write((LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER));
		
		// дDIBͷ������
		file.Write(lpBI, dwDIBSize);
	}
	CATCH (CFileException, e)
	{
		// �������
		::GlobalUnlock((HGLOBAL) hDib);
		
		// �׳��쳣
		THROW_LAST();
	}
	END_CATCH
	
	// �������
	::GlobalUnlock((HGLOBAL) hDib);
	
	// ����TRUE
	return TRUE;
}

/*************************************************************************
*
* �������ƣ�
*   ConvertToGray()
*
* ����:
*   LPSTR lpDIB			- ָ��ԴDIBͼ��ָ��
*   
* ����ֵ:
*   BOOL				- �ɹ�����TRUE�����򷵻�FALSE��
*
* ˵��:
*   �ú�����24λ���ɫͼת����256���Ҷ�ͼ
*
************************************************************************/

BOOL WINAPI ConvertToGray(LPSTR lpDIB)   
{
	
	LPSTR lpDIBBits;                //ָ��DIB�����ص�ָ��
	LPSTR lpNewDIBBits;             //ָ��DIB�Ҷ�ͼͼ��(��ͼ��)��ʼ�����ص�ָ��
    LONG lLineBytes;
	unsigned char * lpSrc;          //ָ��ԭͼ�����ص��ָ��
	unsigned char * lpdest;         //ָ��Ŀ��ͼ�����ص��ָ��
	
    unsigned  char *ired,*igreen,*iblue;
	
    long lWidth;                    //ͼ���Ⱥ͸߶�
	long lHeight;
	long i,j;           //ѭ������
	
	lWidth = ::DIBWidth(lpDIB);   //DIB ���
	lHeight = ::DIBHeight(lpDIB); //DIB �߶�
	RGBQUAD  *lpRGBquad;
	lpRGBquad = (RGBQUAD *)&lpDIB[sizeof(BITMAPINFOHEADER)]; //INFOHEADER��Ϊ��ɫ��		
    
	if(::DIBNumColors(lpDIB) == 256)  //256ɫλͼ�����κδ���
	{
		return TRUE;
    }
	
	if(::DIBNumColors(lpDIB) != 256)  //��256ɫλͼ�����ҶȻ�
	{
		lLineBytes = WIDTHBYTES(lWidth*8*3);
		lpdest = new  BYTE[lHeight*lWidth];
		lpDIBBits = (LPSTR)lpDIB + sizeof(BITMAPINFOHEADER);//ָ��DIB����

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

		//��Ҫ���������飺1���޸�INFOHEADER 2�����ӵ�ɫ�� 3���޸�ԭͼ��Ҷ�ֵ
		LPBITMAPINFOHEADER  lpBI;
		lpBI  =  (LPBITMAPINFOHEADER)lpDIB;
		lpBI->biBitCount = 8;
			
		//���ûҶȵ�ɫ��
		for(i=0; i<256; i++)
		{
			lpRGBquad[i].rgbRed   = (unsigned char)i;
			lpRGBquad[i].rgbGreen = (unsigned char)i;
			lpRGBquad[i].rgbBlue  = (unsigned char)i;
			lpRGBquad[i].rgbReserved = 0;
		}
			
		lpNewDIBBits= ::FindDIBBits(lpDIB);  //�ҵ�DIBͼ��������ʼλ��
		lLineBytes=WIDTHBYTES(lWidth  *  8);
		
		//�޸ĻҶ�ֵ
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
 * �������ƣ�
 *   MedianFilter()
 *
 * ����:
 *   LPSTR lpDIBBits		- ָ��ԴDIBͼ��ָ��
 *   LONG  lWidth			- Դͼ���ȣ���������
 *   LONG  lHeight			- Դͼ��߶ȣ���������
 *   int   iFilterH			- �˲����ĸ߶�
 *   int   iFilterW			- �˲����Ŀ��
 *   int   iFilterMX		- �˲���������Ԫ��X����
 *   int   iFilterMY		- �˲���������Ԫ��Y����
 *
 * ����ֵ:
 *   BOOL					- �ɹ�����TRUE�����򷵻�FALSE��
 *
 * ˵��:
 *   �ú�����DIBͼ�������ֵ�˲���
 *
 ************************************************************************/

BOOL WINAPI MedianFilter(LPSTR lpDIBBits,LONG lWidth,LONG lHeight,int iFilterH,int iFilterW,int iFilterMX,int iFilterMY)
{
	// ָ��Դͼ���ָ��
	unsigned char*	lpSrc;
	
	// ָ��Ҫ���������ָ��
	unsigned char*	lpDst;
	
	// ָ����ͼ���ָ��
	LPSTR			lpNewDIBBits;
	HLOCAL			hNewDIBBits;
	
	// ָ���˲��������ָ��
	unsigned char	* aValue;
	HLOCAL			hArray;
	
	// ѭ������
	LONG			i;
	LONG			j;
	LONG			k;
	LONG			l;
	
	// ͼ��ÿ�е��ֽ���
	LONG			lLineBytes;
	
	// ����ͼ��ÿ�е��ֽ���
	lLineBytes = WIDTHBYTES(lWidth * 8);
	
	// ��ʱ�����ڴ棬�Ա�����ͼ��
	hNewDIBBits = LocalAlloc(LHND, lLineBytes * lHeight);
	
	// �ж��Ƿ��ڴ����ʧ��
	if (hNewDIBBits == NULL)
	{
		// �����ڴ�ʧ��
		return FALSE;
	}
	
	// �����ڴ�
	lpNewDIBBits = (char * )LocalLock(hNewDIBBits);
	
	// ��ʼ��ͼ��Ϊԭʼͼ��
	memcpy(lpNewDIBBits, lpDIBBits, lLineBytes * lHeight);
	
	// ��ʱ�����ڴ棬�Ա����˲�������
	hArray = LocalAlloc(LHND, iFilterH * iFilterW);
	
	// �ж��Ƿ��ڴ����ʧ��
	if (hArray == NULL)
	{
		// �ͷ��ڴ�
		LocalUnlock(hNewDIBBits);
		LocalFree(hNewDIBBits);
		
		// �����ڴ�ʧ��
		return FALSE;
	}
	
	// �����ڴ�
	aValue = (unsigned char * )LocalLock(hArray);
	
	// ��ʼ��ֵ�˲�
	// ��(��ȥ��Ե����)
	for(i = iFilterMY; i < lHeight - iFilterH + iFilterMY + 1; i++)
	{
		// ��(��ȥ��Ե����)
		for(j = iFilterMX; j < lWidth - iFilterW + iFilterMX + 1; j++)
		{
			// ָ����DIB��i�У���j�����ص�ָ��
			lpDst = (unsigned char*)lpNewDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// ��ȡ�˲�������
			for (k = 0; k < iFilterH; k++)
			{
				for (l = 0; l < iFilterW; l++)
				{
					// ָ��DIB��i - iFilterMY + k�У���j - iFilterMX + l�����ص�ָ��
					lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i + iFilterMY - k) + j - iFilterMX + l;
					
					// ��������ֵ
					aValue[k * iFilterW + l] = *lpSrc;
				}
			}
			
			// ��ȡ��ֵ
			* lpDst = GetMedianNum(aValue, iFilterH * iFilterW);
		}
	}
	
	// ���Ʊ任���ͼ��
	memcpy(lpDIBBits, lpNewDIBBits, lLineBytes * lHeight);
	
	// �ͷ��ڴ�
	LocalUnlock(hNewDIBBits);
	LocalFree(hNewDIBBits);
	LocalUnlock(hArray);
	LocalFree(hArray);
	
	// ����
	return TRUE;
}

/*************************************************************************
 *
 * �������ƣ�
 *   GetMedianNum()
 *
 * ����:
 *   unsigned char * bpArray	- ָ��Ҫ��ȡ��ֵ������ָ��
 *   int   iFilterLen			- ���鳤��
 *
 * ����ֵ:
 *   unsigned char      - ����ָ���������ֵ��
 *
 * ˵��:
 *   �ú�����ð�ݷ���һά����������򣬲���������Ԫ�ص���ֵ��
 *
 ************************************************************************/

unsigned char WINAPI GetMedianNum(unsigned char * bArray, int iFilterLen)
{
	// ѭ������
	int		i;
	int		j;
	
	// �м����
	unsigned char bTemp;
	
	// ��ð�ݷ��������������
	for (j = 0; j < iFilterLen - 1; j ++)
	{
		for (i = 0; i < iFilterLen - j - 1; i ++)
		{
			if (bArray[i] > bArray[i + 1])
			{
				// ����
				bTemp = bArray[i];
				bArray[i] = bArray[i + 1];
				bArray[i + 1] = bTemp;
			}
		}
	}
	
	// ������ֵ
	if ((iFilterLen & 1) > 0)
	{
		// ������������Ԫ�أ������м�һ��Ԫ��
		bTemp = bArray[(iFilterLen + 1) / 2];
	}
	else
	{
		// ������ż����Ԫ�أ������м�����Ԫ��ƽ��ֵ
		bTemp = (bArray[iFilterLen / 2] + bArray[iFilterLen / 2 + 1]) / 2;
	}
	
	// ������ֵ
	return bTemp;
}

/*************************************************************************
 *
 * �������ƣ�
 *   ClearBlackBorder()
 *
 * ����:
 *   LPSTR  lpDIBBits		 - ָ��ԴDIBͼ��ָ��
 *   LONG   lWidth			 - Դͼ���ȣ���������
 *   LONG   lHeight			 - Դͼ��߶ȣ���������
 *   double fK               - ȥ����ɫ����ĻҶȵ�������
 *   double fAverGray        - ƽ���Ҷ�
 *
 * ����ֵ:
 *   BOOL					 - �ɹ�����TRUE�����򷵻�FALSE��
 *
 * ˵��:
 *   �ú���ȥ���ɼ�ͼ��ʱ�����γɵĺڱ� 
 *
 ************************************************************************/

BOOL WINAPI ClearBlackBorder(	HDIB   hDIB,
								double dBlackWidth,
								double fK,//dGrayThreshold,
								double dAverGray)
{

	if(!hDIB)
		return FALSE;
	
	//ָ��ԭͼ���ָ��
	unsigned char*  lpSrc;

	//ָ��DIB��ָ��
	LPSTR lpDIB;

	//ָ��DIB����ָ��
	LPSTR lpDIBBits;

	long lWidth;   //Դͼ���ȣ���������

	long lHeight;  //Դͼ��߶ȣ���������


	//����DIB
	lpDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);

	if (::DIBNumColors(lpDIB)!=256) 
	{
		::AfxMessageBox("�Բ�������ֻ�ܴ���Ҷ�ͼ",MB_OK,0);

		::GlobalUnlock((HGLOBAL)hDIB);
		return FALSE;
	}

	lpDIBBits=::FindDIBBits(lpDIB);

	lWidth=::DIBWidth(lpDIB);  
	lHeight=::DIBHeight(lpDIB);

	//ͼ��ÿ�е��ֽ���
	LONG lLineBytes;

	//����ͼ��ÿ�е��ֽ���
	lLineBytes=WIDTHBYTES(lWidth*8);

	for(int i=0; i<lHeight; i++)     //��
	{
		for(int j=0; j<lWidth; j++)  //��
		{
			lpSrc=(unsigned char*)lpDIBBits+lLineBytes*(lHeight-1-i)+j;  //һ����˵��BMP�ļ��������Ǵ��µ��ϣ�
			                                        //�����ҵġ������ļ������ȶ�������
			if((i<dBlackWidth)||(i>lHeight-dBlackWidth)||
				(j<dBlackWidth)||(j>lWidth-dBlackWidth))
				*lpSrc=(unsigned char)(fK*dAverGray+0.5); 
		}
	}

	::GlobalUnlock((HGLOBAL)hDIB);	

	//����
	return TRUE;

}

/*************************************************************************
 *
 * �������ƣ�
 *   ClearBackGround()
 *
 * ����:
 *   HDIB  hDIB		        - ָ��ԴDIB���
 *   double  lA			    - Բ�ĵĴ���λ��
 *   double  lB			    - Բ�ĵĴ���λ��
 *   double fK               -ȥ����ɫ����ĻҶȵ�������
 *   double   lR              -�����Ч��������뾶�����ڸ�ֵΪ��Ҫ�Ҷ��ؽ�����
 *   double fAverGray        -ƽ���Ҷ�
 *
 * ����ֵ:
 *   BOOL					- �ɹ�����TRUE�����򷵻�FALSE��
 *
 * ˵��:
 *   ͼ��Ԥ����:ȥ����ɫ��������
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
	
	//ָ��ԭͼ���ָ��
	unsigned char*  lpSrc;

	//ָ��DIB��ָ��
	LPSTR lpDIB;

	//ָ��DIB����ָ��
	LPSTR lpDIBBits;

	long lWidth;   //Դͼ���ȣ���������

	long lHeight;  //Դͼ��߶ȣ���������


	//����DIB
	lpDIB=(LPSTR)::GlobalLock((HGLOBAL)hDIB);

	if (::DIBNumColors(lpDIB)!=256) 
	{
		::AfxMessageBox("�Բ�������ֻ�ܴ���Ҷ�ͼ",MB_OK,0);

		::GlobalUnlock((HGLOBAL)hDIB);
		return FALSE;
	}

	lpDIBBits=::FindDIBBits(lpDIB);

	lWidth=::DIBWidth(lpDIB);  
	lHeight=::DIBHeight(lpDIB);


	//ͼ��ÿ�е��ֽ���
	LONG lLineBytes;

	//�м����
	float fDistTemp;

	//����ͼ��ÿ�е��ֽ���
	lLineBytes=WIDTHBYTES(lWidth*8);

	for(int i=0; i<lHeight; i++)//��
	{
		for(int j=0; j<lWidth; j++)//��
		{

		    lpSrc=(unsigned char*)lpDIBBits+lLineBytes*(lHeight-1-i)+j;
			fDistTemp = (float)((i-dCenterV)*(i-dCenterV)+(j-dCenterH)*(j-dCenterH));
			fDistTemp = (float)sqrt(fDistTemp);
			if(fDistTemp>dMaxR)
		        *lpSrc=(unsigned char)(fK*dAverGray+0.5);
		}
	}

	::GlobalUnlock((HGLOBAL)hDIB);

	//����
	return TRUE;
}

/*************************************************************************
 *
 * �������ƣ�
 *   CopyHandle()
 *
 * ����:
 *   HGLOBAL h          - Ҫ���Ƶ��ڴ�����
 *
 * ����ֵ:
 *   HGLOBAL            - ���ƺ�����ڴ�����
 *
 * ˵��:
 *   �ú�������ָ�����ڴ����򡣷��ظ��ƺ�����ڴ����򣬳���ʱ����0��
 *
 ************************************************************************/

HGLOBAL WINAPI CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	// ��ȡָ���ڴ������С
	DWORD dwLen = ::GlobalSize((HGLOBAL) h);
	
	// �������ڴ�ռ�
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);
	
	// �жϷ����Ƿ�ɹ�
	if (hCopy != NULL)
	{
		// ����
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		
		// ����
		memcpy(lpCopy, lp, dwLen);
		
		// �������
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

