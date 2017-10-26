
// GUIDlg.cpp : ʵ���ļ�
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




// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CGUIDlg �Ի���



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


// CGUIDlg ��Ϣ�������

BOOL CGUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	sMsg = "û���ҵ�������";
}
else
{
	m_iPort = nRet;
	Beep(4000,200);
	if (nRet >1000)
	{
		sMsg.Format("������������USB�˿� %d,%d ms",nRet,tSpan.MSecond); 
	}
	else
	{
		Sleep(200);
		nRet2 = Syn_GetCOMBaud( nRet ,&uiCurrBaud);
		sMsg2.Format("%u",uiCurrBaud);
		sMsg.Format("�����������ڴ��� %d,��ǰSAM������Ϊ %u ,%d ms",nRet,uiCurrBaud,tSpan.MSecond); 
	}
}
InsertDebugMes(sMsg);
if (m_iPort == 9999)
{
	nRet = Syn_OpenPort(m_iPort);
	nRet = Syn_USBHIDGetMaxByte(m_iPort,&m_iMaxByte,&nARMVol);
	nRet = Syn_ClosePort(m_iPort);
	sMsg.Format("������֧�ֵ����ͨѶ�ֽ���Ϊ%d,�汾%d",m_iMaxByte,nARMVol);
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
			sMsg.Format("�������̶�ȡ���֤��Ϣ(��ָ��)�ɹ�, %d ms",tSpan.MSecond);
			std::string sTemp;
			InsertDebugMes(sMsg);
			sMsg.Format("����:%s",idcardData.Name);
			InsertDebugMes(sMsg);
			sMsg.Format("�Ա�:%s",idcardData.Sex);
			InsertDebugMes(sMsg);
			sMsg.Format("����:%s",idcardData.Nation);
			InsertDebugMes(sMsg);
			sMsg.Format("����:%s",idcardData.Born);
			InsertDebugMes(sMsg);
			sMsg.Format("סַ:%s",idcardData.Address);
			InsertDebugMes(sMsg);
			sMsg.Format("���֤��:%s",idcardData.IDCardNo);
			InsertDebugMes(sMsg);
			sMsg.Format("��֤����:%s",idcardData.GrantDept);
			InsertDebugMes(sMsg);
			sMsg.Format("��Ч�ڿ�ʼ:%s",idcardData.UserLifeBegin);
			InsertDebugMes(sMsg);
			sMsg.Format("��Ч�ڽ���:%s",idcardData.UserLifeEnd);
			InsertDebugMes(sMsg);
			//			sMsg.Format("��Ƭ:%s",idcardData.PhotoFileName);
			//			InsertDebugMes(sMsg);
			HBITMAP hBitmap;
			CStatic *pStatic = (CStatic *)GetDlgItem(IDC_STATIC);
			sMsg.Format("%s",idcardData.PhotoFileName);
			std::string tmpSTR="copy "+std::string(idcardData.PhotoFileName)+" "+ "G:\\�����\\�������\\"+std::string(idcardData.IDCardNo)+".bmp";//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Ҫ�޸� �ĵ���������
			system(tmpSTR.c_str());
			//����socketͨ��
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
				//��������
				recv(sockClient, buff, sizeof(buff), 0);
				//printf("%s\n", buff);
			}
			send(sockClient, (char *)(&idcardData), sizeof(idcardData), 0);//�������ݳ���
			closesocket(sockClient);
			WSACleanup();
			///////////////////////////////////////////////////////////////////////////
			hBitmap = (HBITMAP) ::LoadImage(NULL,sMsg,IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
			pStatic->ModifyStyle(0XF,SS_BITMAP);

			pStatic->SetBitmap(hBitmap);					
			if (nRet == 0 || nRet == 1)
			{
				sMsg.Format("ָ���ļ�:%s",szPath);
				InsertDebugMes(sMsg);
			}
			else
			{
				sMsg = "û��ָ����Ϣ";
				InsertDebugMes(sMsg);
			}
		}
		else
		{
			sMsg.Format("�������̶�ȡ���֤��Ϣ����, nRet = %d, %d ms",nRet ,tSpan.MSecond);
			InsertDebugMes(sMsg);
		}
	}
	else
	{
		sMsg = "�򿪶˿ڴ���";
		InsertDebugMes(sMsg);	
	}
	if (m_iPort>0)
	{
		Syn_ClosePort(m_iPort);
	}
}
		


void CGUIDlg::OnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�



	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


//void CGUIDlg::OnStatic()
//{
//	// TODO: �ڴ���������������
//}


void CGUIDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
		*pResult = 0;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}


void CGUIDlg::OnUpdateStatic(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
}


//void CGUIDlg::OnStatic()
//{
//	// TODO: �ڴ���������������
//}


void CGUIDlg::OnStatic()
{
	// TODO: �ڴ���������������
}
