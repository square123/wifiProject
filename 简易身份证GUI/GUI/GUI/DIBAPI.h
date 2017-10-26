//dibapi.h

#include <afxwin.h>
#ifndef _INC_DIBAPI
#define _INC_DIBAPI

//DIB���
DECLARE_HANDLE(HDIB);

//DIB����
#define PALVERSION 0x300

//�ж��Ƿ���Win3.0��DIB
#define IS_WIN30_DIB(lpbi)   ((*(LPDWORD)(lpbi))==sizeof(BITMAPINFOHEADER))

//�����������Ŀ��
#define RECTWIDTH(lpRect)    ((lpRect)->right-(lpRect)->left)

//�����������ĸ߶�
#define RECTHEIGHT(lpRect)   ((lpRect)->bottom-(lpRect)->top)

//�ڼ���ͼ���Сʱ�����ù�ʽ��biSizeImage=biWidth'��biHeight.
//��biWidth',������biWidth,�����biWidth'������4������������ʾ���ڻ����
//biWidth�ģ���4�������������WIDTHBYTES������������biWidth'
#define WIDTHBYTES(bits)   (((bits)+31)/32*4)
//#define WIDTHBYTES(bits)   ((bits)/32*4)

//����ԭ��

//��ȡbmp�ļ�
HDIB WINAPI ReadDIBFile(CFile& file);

//����DIB���
DWORD WINAPI DIBWidth(LPSTR lpDIB);

//����DIB�߶�
DWORD WINAPI DIBHeight(LPSTR lpDIB);

//����DIB��ɫ����ɫ��Ŀ
WORD WINAPI DIBNumColors(LPSTR lpbi);

//����DIB��ɫ���С
WORD WINAPI PaletteSize(LPSTR lpbi);

//����DIBͼ��������ʼλ��
LPSTR WINAPI FindDIBBits(LPSTR lpbi);

//����DIB����
BOOL WINAPI PaintDIB(	HDC      hDC,
					 LPRECT   lpDCRect,
					 HDIB     hDIB,
					 LPRECT   lpDIBRect,
					 CPalette *pPal);

//�����Ļ
void WINAPI ClearAll(CDC* pDC,CRect dcRect);

//����Ļ����ʾλͼ
void WINAPI DisplayDIB(CDC* pDC, HDIB hDIB,CRect rcDest);

//��DIB���浽ָ���ļ���
BOOL WINAPI SaveDIB (HDIB hDib, CFile& file);

//��24λ��ɫͼ��ת��Ϊ�Ҷ�ͼ��
BOOL WINAPI ConvertToGray(LPSTR lpDIB);

//��ֵ�˲�
BOOL WINAPI MedianFilter(	LPSTR lpDIBBits,
						 LONG lWidth,
						 LONG lHeight,
						 int iFilterH,
						 int iFilterW,
						 int iFilterMX,
						 int iFilterMY);

//��ȡ��ֵ�˲�����ֵ
unsigned char WINAPI GetMedianNum(unsigned char * bArray,
								  int iFilterLen);

//�����ڱ�
BOOL WINAPI ClearBlackBorder(	HDIB   hDIB,
							 double dBlackWidth,
							 double dGrayThreshold,
							 double dAverGray);

//������ɫ��������ͼ��
BOOL WINAPI ClearBackGround(HDIB   hDIB,
							double dCenterH,
							double dCenterV,
							double dMaxR,
							double fK,
							double dAverGray);



//����DIB���
HGLOBAL WINAPI CopyHandle(HGLOBAL h);

#endif//!_INC_DIBAPI