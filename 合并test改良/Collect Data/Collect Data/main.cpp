#include"myKinect.h"
#include "wifi.h"
#include "proMySQL.h"

#include <ctime> //����ʱ�亯��
#include<sstream> //��������ת��
#include <fstream>
#include <iostream>

//���Ե�����

#define screenFilePath "F://�����//̽������//" //���ļ���·��
#define jihuaFilePath "F://�����//ͼ������//"

DWORD WINAPI MyThreadProc1(LPVOID lpParameter);
DWORD WINAPI MyThreadProc2(LPVOID lpParameter);
DWORD WINAPI MyThreadProc3(LPVOID lpParameter);


//���߳�ͨ�ŵ��м����
time_t globTime;
bool globflag=0;
char recvBuf[1000];


using namespace std;

string timeIntToTimestampString(time_t input)//time_tת����timestamp   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!��ʱ
{
	tm *pTmp=localtime(&input);
	char pStr[15]={};
	sprintf(pStr,"%d%02d%02d%02d%02d%02d",pTmp->tm_year+1900,pTmp->tm_mon+1,pTmp->tm_mday+1,pTmp->tm_hour,pTmp->tm_min,pTmp->tm_sec);
	return string(pStr);
}



int main()
{
	HANDLE handle1,handle2,handle3;
	handle1 = CreateThread(NULL,0,MyThreadProc1,NULL,0,NULL);
	handle2 = CreateThread(NULL,0,MyThreadProc2,NULL,0,NULL);
	handle3 = CreateThread(NULL,0,MyThreadProc3,NULL,0,NULL);

	WaitForSingleObject(handle1,INFINITE);//����������ͬʱ�����ˡ�
	WaitForSingleObject(handle2,INFINITE);
	WaitForSingleObject(handle3,INFINITE);

	CloseHandle(handle1);
	CloseHandle(handle2);
	CloseHandle(handle3);
}  



DWORD WINAPI MyThreadProc1(LPVOID lpParameter)
{
	Kinect myKinect;
	myKinect.InitKinect();
	myKinect.InitColor();
	myKinect.InitDepth();
	myKinect.InitBodyIndex();
	myKinect.Initbody();
	myKinect.InitCoorMap();

	while (1)
	{  
		myKinect.depthProcess2();  //�����ж���ֵ��Ҫ�õ��ĺ���

		rectangle(myKinect.depthMat8,rec_tl,rec_br,Scalar(255,255,255),2,8);
		imshow("��ⷶΧ���ʾ��ͼ",myKinect.depthMat8);

		myKinect.bodyProcess2();     //ǰ������ȡ

		//�ҳ���������������ż����ֵ
		float min=myKinect.DepthSort[0]; 
		int miner=6;  
		for(int i=0;i<6;i++)
		{
			if(myKinect.DepthSort[i]<=min)
			{
				miner=i;
				min=myKinect.DepthSort[i];
			}
		}

		if ((min!=10)) //��������˲���10ʱ����˵��������������һ����
		{
			cout<<"��������ţ� "<<miner<<endl<<endl;

			for (int i=0;i<6;i++)  //���ҵ��������ʱ������������Ϊ��ʼֵ
			{
				myKinect.DepthSort[i]=10;
			}
		}
		else
		{
			miner=6;
		}

		//��λ�ָ�Ĳ���
		if (((myKinect.detPeopleDepth3())<peopleDisThd)&&(myKinect.detPeopleDepth3()>0))
		{
			if (miner!=6)
			{
				if (myKinect.T_enter_flag==0)
				{
					myKinect.T_enter=time(NULL);
					globTime=myKinect.T_enter;
					myKinect.T_enter_flag=1;
					globflag=1;
					cout<<"��ʼ��ͼʱ�̣�"<<myKinect.T_enter<<endl;
				}

				cout<<"����˵��������ǣ� "<<miner<<" ���зָ�"<<endl;
				myKinect.segmentProcess2(miner);
				cout<<"********************************"<<endl;
			}
		}

		if ((myKinect.Temp!=6)&&(miner==6))
		{
			if (myKinect.NumOnce!=0)
			{
			myKinect.T_leave=time(NULL);
			myKinect.T_enter_flag=0;
			globflag=0;
			globTime=myKinect.T_leave;
			cout<<"ͼ��ɸѡʱ�̣�"<<myKinect.T_leave<<endl;
			cout<<"�����һ��"<<endl;
			cout<<"��һ��"<<myKinect.Temp<<"  ��һ��"<<miner<<endl;	
			myKinect.select2(myKinect.Temp);
			}

		}

		if ((myKinect.Temp!=6)&&(miner!=6)&&(myKinect.Temp!=miner))
		{   
			if (myKinect.NumOnce!=0)
			{
				myKinect.T_leave=time(NULL);
				globTime=myKinect.T_leave;
				myKinect.T_enter_flag=0;
				globflag=0;
				cout<<"ͼ��ɸѡʱ�̣�"<<myKinect.T_leave<<endl;
				cout<<"����ڶ���"<<endl;
				cout<<"��һ��"<<myKinect.Temp<<"  ��һ��"<<miner<<endl;	
				myKinect.select2(myKinect.Temp);
			}
		}
		myKinect.Temp=miner;
		rectangle(myKinect.bodyMat,rec_TL,rec_BR,Scalar(0,0,255),2,8);
		imshow("����ָ������ʾ��ͼ",myKinect.bodyMat);
		waitKey(10);
	}  
	return 0;
}

DWORD WINAPI MyThreadProc2(LPVOID lpParameter)
{

	////////////////////////////////////////Ҫ�Ѳɼ��ʹ洢�ֿ������ݿ�Ĳ��ֵ��������Լ���λ���ϣ����Һ���Ӧ�û����в�ѯ���ܡ������ײ��Ӧ�����в�ѯ���ܵġ�
	proMySQL dataStoreTest;
	dataStoreTest.creBigTable("bigTable");
	time_t t1=0; //��¼�������ʷʱ��
	Wifi wifi;
	//wifi��ʼ��
	wifi.InitWifi();
	bool waitFlag=true;//true�����Ǵ��ھ�ֹλ�ģ�false �����Ѿ����ٵȴ��ڴ������� //����Ǵ���Ĳ���
	bool fileFlag=true;  //true�����ļ��ǿ���ʹ�õģ�����û�д���
	//���ڴ洢
	ofstream outfileDing,outfileDingRaw;   //�洢�����ĺʹ�����


	while(1)
	{
		if (globTime!=t1) //����ʱ��仯��״̬λ�ĸ��� //Ĭ��globTime ��ʼҲΪ�㣬һ����ʼ�仯��ʼ����
		{
			t1=globTime;
		}
		waitFlag=!globflag;
		if (waitFlag==true&&fileFlag==true)
		{
			//cout<<"ɶ������"<<endl;
			Sleep(100);//�ȴ�
		}
		else if(waitFlag==false&&fileFlag==false) //��ʱ�ļ��Ѿ��½����ˣ����ڴ洢����
		{
			//wifi��¼����
			//cout<<"��̽������"<<endl;
			wifi.wifiProcess(); 
		}
		else if (waitFlag==false&&fileFlag==true)
		{

			fileFlag=false;
			cout<<"WIFI��ʼ��¼----------------------------------------------------------------"<<endl;
		}
		else if (waitFlag==true&&fileFlag==false)
		{
			//һ�ν�����Ҫ��ɵĲ���
			//�ļ���ʼ�Ĳ���	
			outfileDing.open(string(screenFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+".txt",ios::app); //���������� ������֤�������ݾͰ����֤�Ŵ򿪶�Ӧ���ļ�
			outfileDingRaw.open(string(screenFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+"_raw"+".txt",ios::app); //ԭʼ����
			vector<Wifi::mapUsed> outdataDing;
			wifi.wifiProcessed2(wifi.selMapDing,outdataDing,outfileDingRaw,outfileDing,timeIntToTimestampString(t1));//�����ݽ��д�������,ͬʱ��ԭʼ���ݱ�����
			wifi.reSelMacRssi();//����������
			outfileDing.close();//�ر��ļ�
			outfileDingRaw.close();//�ر��ļ�
			//������� �����ȿ���
			string tmpStr1=string(screenFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+".txt";//���û�����֤��ʹ��ʱ�����洢 ʱ�������� raw���п��ޣ���ֻ�����������ݰ�
			string tmpStr2=string(jihuaFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+".jpg";//���û�����֤��ʹ��ʱ�����洢 ʱ��������
			//���ڻ�û���뿪ʱ�䣬���ý���ʱ������
			dataStoreTest.insertBigTabData("bigTable",timeIntToTimestampString(t1),timeIntToTimestampString(t1),tmpStr1.c_str(),tmpStr2.c_str(),recvBuf);//�����ײ���ڲ������ݲ������ˣ���Ϊ�����ײ��Ҫ��ȡ�ļ��������Ҫ��֮ǰ���ļ��ر�
			memset(recvBuf, 0, sizeof(recvBuf));//�������֤����

			fileFlag=true;
			cout<<"WIFI��¼����-----------------------------------------------------------"<<endl;
		}
	}
	return 0;
}



DWORD WINAPI MyThreadProc3(LPVOID lpParameter)//��������֤��ͨ��
{
	//socketͨ��
	WSADATA wsaData;
	int port = 5099;     //�����֤�Ķ˿ںű���һ��

	char buf[] = "0"; 

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
	}

	//�������ڼ������׽���
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port); //1024���ϵĶ˿ں�
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if(retVal == SOCKET_ERROR){
		printf("Failed bind:%d\n", WSAGetLastError());

	}

	if(listen(sockSrv,10) ==SOCKET_ERROR){
		printf("Listen failed:%d", WSAGetLastError());

	}

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	while (1)
	{
		//socketͨ��
		//�ȴ��ͻ�������	
		SOCKET sockConn = accept(sockSrv, (SOCKADDR *) &addrClient, &len);
		if(sockConn == SOCKET_ERROR){
			printf("Accept failed:%d", WSAGetLastError());
		}

		//��������
		int iSend = send(sockConn, buf, sizeof(buf) , 0);
		if(iSend == SOCKET_ERROR){
			printf("send failed");
		}

		// 		//��������
		recv(sockConn, recvBuf, sizeof(recvBuf), 0);
		closesocket(sockConn);
	}
	return 0;
}