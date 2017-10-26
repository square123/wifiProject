
// GUIDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GUI.h"
#include "GUIDlg.h"
#include "ID_Fpr_Public.H"
#include "ID_FprCap_Public.H"
#include "SynPublic.h"
#include "DIBAPI.H"
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
unsigned char cTouchar(unsigned char cCard[2],unsigned char mode)
{
	unsigned char utmp[2];
	unsigned char uRet;
	for(int j=0;j<2;j++)
	{
		utmp[j] = 0;
	}
	for(int i = 0;i<2;i++)
	{
		if(cCard[i]<='F' && cCard[i]>='A')
		{
			utmp[i] = cCard[i] - 'A' + 10;
		}
		if(cCard[i]<='f' && cCard[i]>='a')
		{
			utmp[i] = cCard[i] - 'a' + 10;
		}
		if(cCard[i]<='9' && cCard[i]>='0')
		{
			utmp[i] = cCard[i] - '0';
		}
	}
	if (mode ==2) {
		uRet = utmp[0]*0X10 + utmp[1] ;
	}
	else
	{
		uRet = utmp[1]*0X10 + utmp[0] ;
	}
	return uRet;
}




// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGUIDlg 对话框



CGUIDlg::CGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGUIDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1 , m_ListMsg);
}

BEGIN_MESSAGE_MAP(CGUIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CGUIDlg::OnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CGUIDlg::OnClickedButton2)
//	ON_COMMAND(IDC_STATIC, &CGUIDlg::OnStatic)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CGUIDlg::OnLvnItemchangedList1)
	ON_UPDATE_COMMAND_UI(IDC_STATIC, &CGUIDlg::OnUpdateStatic)
//	ON_COMMAND(IDC_STATIC, &CGUIDlg::OnStatic)
ON_COMMAND(IDC_STATIC, &CGUIDlg::OnStatic)
END_MESSAGE_MAP()


// CGUIDlg 消息处理程序

BOOL CGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}CString strTime;


void CGUIDlg::InsertDebugMes(CString sMsg)
{
	/*CString strTime;
	CTime   timTemp;
	CString strMes;
	timTemp = CTime::GetCurrentTime();
	strTime = timTemp.Format("%Y-%m-%d %H:%M:%S");
	if(m_ListView.GetItemCount()>200)
	{
		m_ListView.DeleteAllItems();	
	}
	m_ListView.InsertItem(LVIF_IMAGE| LVIF_TEXT|LVIF_STATE, 0, strTime,0,0,0,0);
	m_ListView.SetItemText(0, 1,sMsg );
	strMes = strTime+"    "+sMsg+"\r\n";
	m_fLog.SeekToEnd();
	m_fLog.Write(strMes,strMes.GetLength());*/
	CString strTime;
	CTime   timTemp;
	timTemp = CTime::GetCurrentTime();
	strTime = timTemp.Format("%Y-%m-%d %H:%M:%S   ");
	sMsg = strTime + sMsg;
	m_ListMsg.InsertString(m_ListMsg.GetCount(),sMsg );



}

void CGUIDlg::OnClickedButton1()
{	CString sMsg,sMsg2;
unsigned int uiCurrBaud; 
int nRet,nRet2;
unsigned char nARMVol;
m_iPort = 0;
CCalcTime tSpan;
tSpan.Clear();
tSpan.BegginTime();
nRet = Syn_FindUSBReader();
tSpan.EndTime();
if ( nRet == 0)
{
	sMsg = "没有找到读卡器";
}
else
{
	m_iPort = nRet;
	Beep(4000,200);
	if (nRet >1000)
	{
		sMsg.Format("读卡器连接在USB端口 %d,%d ms",nRet,tSpan.MSecond); 
	}
	else
	{
		Sleep(200);
		nRet2 = Syn_GetCOMBaud( nRet ,&uiCurrBaud);
		sMsg2.Format("%u",uiCurrBaud);
		sMsg.Format("读卡器连接在串口 %d,当前SAM波特率为 %u ,%d ms",nRet,uiCurrBaud,tSpan.MSecond); 
	}
}
InsertDebugMes(sMsg);
if (m_iPort == 9999)
{
	nRet = Syn_OpenPort(m_iPort);
	nRet = Syn_USBHIDGetMaxByte(m_iPort,&m_iMaxByte,&nARMVol);
	nRet = Syn_ClosePort(m_iPort);
	sMsg.Format("读卡器支持的最大通讯字节数为%d,版本%d",m_iMaxByte,nARMVol);
	sMsg2.Format("%d",m_iMaxByte);

	InsertDebugMes(sMsg);

}
}




void CGUIDlg::OnClickedButton2()
{
	
	int nRet ;
	unsigned char pucIIN[4];
	unsigned char pucSN[8];
	CString sMsg;
	IDCardData idcardData;
	char ctmp[256]={0};
	char szPath[_MAX_PATH]={0};
	Syn_SetPhotoPath(1,ctmp);
	Syn_SetPhotoName(3);
	nRet = Syn_OpenPort(m_iPort);
	if (nRet == 0)
	{
		CCalcTime tSpan;
		tSpan.Clear();
		tSpan.BegginTime();
		nRet = Syn_StartFindIDCard(m_iPort,pucIIN,0);
		nRet = Syn_SelectIDCard(m_iPort,pucSN,0);
		nRet = Syn_ReadFPMsg(m_iPort,0,&idcardData,szPath);
		tSpan.EndTime();
		if (m_iReadStart == 1)
		{
			m_iReadSum++;
		}
		if (nRet == 0)
		{
			if (m_iReadStart==1)
			{
				m_iReadOKSum++;
			}
			sMsg.Format("完整过程读取身份证信息(含指纹)成功, %d ms",tSpan.MSecond);
			std::string sTemp;
			InsertDebugMes(sMsg);
			sMsg.Format("姓名:%s",idcardData.Name);
			InsertDebugMes(sMsg);
			sMsg.Format("性别:%s",idcardData.Sex);
			InsertDebugMes(sMsg);
			sMsg.Format("民族:%s",idcardData.Nation);
			InsertDebugMes(sMsg);
			sMsg.Format("出生:%s",idcardData.Born);
			InsertDebugMes(sMsg);
			sMsg.Format("住址:%s",idcardData.Address);
			InsertDebugMes(sMsg);
			sMsg.Format("身份证号:%s",idcardData.IDCardNo);
			InsertDebugMes(sMsg);
			sMsg.Format("发证机关:%s",idcardData.GrantDept);
			InsertDebugMes(sMsg);
			sMsg.Format("有效期开始:%s",idcardData.UserLifeBegin);
			InsertDebugMes(sMsg);
			sMsg.Format("有效期结束:%s",idcardData.UserLifeEnd);
			InsertDebugMes(sMsg);
			//			sMsg.Format("照片:%s",idcardData.PhotoFileName);
			//			InsertDebugMes(sMsg);
			HBITMAP hBitmap;
			CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC);
			sMsg.Format("%s",idcardData.PhotoFileName);
			std::string tmpSTR="copy "+std::string(idcardData.PhotoFileName)+" "+ "G:\\处理后\\身份数据\\"+std::string(idcardData.IDCardNo)+".bmp";//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!要修改 改到主函数中
			system(tmpSTR.c_str());
			//加入socket通信
			///////////////////////////////////////////////////////////////////////////
			WSADATA wsaData;
			char buff[1024];
			SOCKADDR_IN addrSrv;
			SOCKET sockClient;
			memset(buff, 0, sizeof(buff));

			if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
			{
				printf("Failed to load Winsock");
			}
			addrSrv.sin_family = AF_INET;
			addrSrv.sin_port = htons(5099);
			addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			sockClient = socket(AF_INET, SOCK_STREAM, 0);
			if(SOCKET_ERROR == sockClient){
				printf("Socket() error:%d", WSAGetLastError());
			}
			if(connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET){
				printf("Connect failed:%d", WSAGetLastError());
			}
			else
			{
				//接收数据
				recv(sockClient, buff, sizeof(buff), 0);
				//printf("%s\n", buff);
			}
			send(sockClient, (char *)(&idcardData), sizeof(idcardData), 0);//所有数据出发
			closesocket(sockClient);
			WSACleanup();
			///////////////////////////////////////////////////////////////////////////
			hBitmap = (HBITMAP) ::LoadImage(NULL,sMsg,IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			pStatic->ModifyStyle(0XF,SS_BITMAP);

			pStatic->SetBitmap(hBitmap);					
			if (nRet == 0 || nRet == 1)
			{
				sMsg.Format("指纹文件:%s",szPath);
				InsertDebugMes(sMsg);
			}
			else
			{
				sMsg = "没有指纹信息";
				InsertDebugMes(sMsg);
			}
		}
		else
		{
			sMsg.Format("完整过程读取身份证信息错误, nRet = %d, %d ms",nRet ,tSpan.MSecond);
			InsertDebugMes(sMsg);
		}
	}
	else
	{
		sMsg = "打开端口错误";
		InsertDebugMes(sMsg);	
	}
	if (m_iPort>0)
	{
		Syn_ClosePort(m_iPort);
	}
}
		


void CGUIDlg::OnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。



	// TODO:  在此添加控件通知处理程序代码
}


//void CGUIDlg::OnStatic()
//{
//	// TODO: 在此添加命令处理程序代码
//}


void CGUIDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
		*pResult = 0;
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CGUIDlg::OnUpdateStatic(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}


//void CGUIDlg::OnStatic()
//{
//	// TODO: 在此添加命令处理程序代码
//}


void CGUIDlg::OnStatic()
{
	// TODO: 在此添加命令处理程序代码
}
