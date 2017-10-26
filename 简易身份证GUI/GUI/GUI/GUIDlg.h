
// GUIDlg.h : 头文件
//

#pragma once

#include <WinSock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

class CCalcTime
{
public:	
	LARGE_INTEGER t1;
	LARGE_INTEGER t2;
	__int64 CountTick ;
	__int64 Count ;
	__int64 PerSec;
	__int64 ThisTick;
	int ThisMSecond;


	int MSecond;
	int ms[10000];
	int Index ;



	CCalcTime()
	{ 
		Clear();
	} 
	~CCalcTime(){};

	void BegginTime(){ QueryPerformanceCounter( &t1 ); };
	void EndTime() { 
		QueryPerformanceCounter( &t2 ) ;
		ThisTick   = (t2.QuadPart-t1.QuadPart);
		CountTick += ThisTick;
		Count++;
		ThisMSecond = (int)( ThisTick*1000 / PerSec);
		MSecond = (int) (((double)CountTick )/ PerSec * 1000 );
		ms[Index++] =    (int)((double)(t2.QuadPart-t1.QuadPart) / PerSec * 1000 );
		if( Index == 10000 ) Index = 0;
	};
	void Clear()
	{
		Count = 0 ;
		CountTick = 0 ; 
		QueryPerformanceFrequency ( &t1 );
		PerSec = t1.QuadPart;
		MSecond =0;
		Index = 0;
		memset( ms , 0 , sizeof(ms) );
		ThisMSecond = 0;
		ThisTick = 0;
	};

};
// CGUIDlg 对话框
class CGUIDlg : public CDialogEx
{
// 构造
public:
	CGUIDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GUI_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_ListMsg;
	int m_iPort;
	unsigned char m_iMaxByte;
	int m_iReadType;
	int m_iReadSum;
	int m_iReadOKSum;
	int m_iReadStart;

	int m_iM1ReadSum;
	int m_iM1ReadOKSum;
	int m_iM1ReadStart;

	int m_iM1WriteSum;
	int m_iM1WriteOKSum;
	int m_iM1WriteStart;

	int m_iLoopSum;
	int m_iTime;
	CFile m_fLog;
	void InsertDebugMes(CString sMsg);
	BOOL OpenLogFile();

	afx_msg void OnClickedButton1();
	afx_msg void OnClickedButton2();
	afx_msg void OnChangeEdit1();
//	afx_msg void OnStatic();
	CListCtrl m_ListView;
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateStatic(CCmdUI *pCmdUI);
	int RawToBmp(unsigned char *ucRaw,CString bmpFile);
		afx_msg void ViewBmp(CString bmpFile);
//		afx_msg void OnStatic();
		afx_msg void OnStatic();
};
