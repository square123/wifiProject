#include"myKinect.h"
#include "wifi.h"
#include "proMySQL.h"

#include <ctime> //用于时间函数
#include<sstream> //用以类型转换
#include <fstream>
#include <iostream>

//可以调整的

#define screenFilePath "F://处理后//探针数据//" //存文件的路径
#define jihuaFilePath "F://处理后//图像数据//"

DWORD WINAPI MyThreadProc1(LPVOID lpParameter);
DWORD WINAPI MyThreadProc2(LPVOID lpParameter);
DWORD WINAPI MyThreadProc3(LPVOID lpParameter);


//各线程通信的中间变量
time_t globTime;
bool globflag=0;
char recvBuf[1000];


using namespace std;

string timeIntToTimestampString(time_t input)//time_t转换成timestamp   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!临时
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

	WaitForSingleObject(handle1,INFINITE);//加了这句就能同时运行了。
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
		myKinect.depthProcess2();  //用于判断阈值需要用到的函数

		rectangle(myKinect.depthMat8,rec_tl,rec_br,Scalar(255,255,255),2,8);
		imshow("检测范围深度示意图",myKinect.depthMat8);

		myKinect.bodyProcess2();     //前景的提取

		//找出最近的人体索引号及深度值
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

		if ((min!=10)) //当最近的人不是10时，才说明索引到至少有一个人
		{
			cout<<"最近索引号： "<<miner<<endl<<endl;

			for (int i=0;i<6;i++)  //当找到最近的人时立即把数组置为初始值
			{
				myKinect.DepthSort[i]=10;
			}
		}
		else
		{
			miner=6;
		}

		//部位分割的操作
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
					cout<<"开始存图时刻："<<myKinect.T_enter<<endl;
				}

				cout<<"最近人的索引号是： "<<miner<<" 进行分割"<<endl;
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
			cout<<"图像筛选时刻："<<myKinect.T_leave<<endl;
			cout<<"进入第一种"<<endl;
			cout<<"上一次"<<myKinect.Temp<<"  这一次"<<miner<<endl;	
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
				cout<<"图像筛选时刻："<<myKinect.T_leave<<endl;
				cout<<"进入第二种"<<endl;
				cout<<"上一次"<<myKinect.Temp<<"  这一次"<<miner<<endl;	
				myKinect.select2(myKinect.Temp);
			}
		}
		myKinect.Temp=miner;
		rectangle(myKinect.bodyMat,rec_TL,rec_BR,Scalar(0,0,255),2,8);
		imshow("人物分割与跟踪示意图",myKinect.bodyMat);
		waitKey(10);
	}  
	return 0;
}

DWORD WINAPI MyThreadProc2(LPVOID lpParameter)
{

	////////////////////////////////////////要把采集和存储分开，数据库的部分单独放在自己的位置上，而且后续应该还具有查询功能。多点碰撞中应该是有查询功能的。
	proMySQL dataStoreTest;
	dataStoreTest.creBigTable("bigTable");
	time_t t1=0; //记录传入的历史时刻
	Wifi wifi;
	//wifi初始化
	wifi.InitWifi();
	bool waitFlag=true;//true代表是处于静止位的，false 代表已经不再等待在处理数据 //这个是传入的参数
	bool fileFlag=true;  //true代表文件是可以使用的，即还没有处理
	//用于存储
	ofstream outfileDing,outfileDingRaw;   //存储基本的和处理后的


	while(1)
	{
		if (globTime!=t1) //根据时间变化的状态位的更新 //默认globTime 初始也为零，一旦开始变化开始更新
		{
			t1=globTime;
		}
		waitFlag=!globflag;
		if (waitFlag==true&&fileFlag==true)
		{
			//cout<<"啥都不干"<<endl;
			Sleep(100);//等待
		}
		else if(waitFlag==false&&fileFlag==false) //此时文件已经新建好了，正在存储数据
		{
			//wifi记录操作
			//cout<<"在探测数据"<<endl;
			wifi.wifiProcess(); 
		}
		else if (waitFlag==false&&fileFlag==true)
		{

			fileFlag=false;
			cout<<"WIFI开始记录----------------------------------------------------------------"<<endl;
		}
		else if (waitFlag==true&&fileFlag==false)
		{
			//一次结束后要完成的操作
			//文件初始的操作	
			outfileDing.open(string(screenFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+".txt",ios::app); //处理后的数据 如果身份证读到数据就按身份证号打开对应的文件
			outfileDingRaw.open(string(screenFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+"_raw"+".txt",ios::app); //原始数据
			vector<Wifi::mapUsed> outdataDing;
			wifi.wifiProcessed2(wifi.selMapDing,outdataDing,outfileDingRaw,outfileDing,timeIntToTimestampString(t1));//对数据进行处理并排序,同时把原始数据保存下
			wifi.reSelMacRssi();//将数据清零
			outfileDing.close();//关闭文件
			outfileDingRaw.close();//关闭文件
			//输出操作 这里先空上
			string tmpStr1=string(screenFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+".txt";//如果没有身份证就使用时间来存储 时间是主键 raw可有可无，就只处理处理后的数据吧
			string tmpStr2=string(jihuaFilePath)+(string(recvBuf+148).size()!=0?string(recvBuf+148):timeIntToTimestampString(t1))+".jpg";//如果没有身份证就使用时间来存储 时间是主键
			//现在还没有离开时间，先拿进入时间来抵
			dataStoreTest.insertBigTabData("bigTable",timeIntToTimestampString(t1),timeIntToTimestampString(t1),tmpStr1.c_str(),tmpStr2.c_str(),recvBuf);//多点碰撞放在插入数据操作中了，因为多点碰撞需要读取文件，因此需要在之前将文件关闭
			memset(recvBuf, 0, sizeof(recvBuf));//清零身份证数据

			fileFlag=true;
			cout<<"WIFI记录结束-----------------------------------------------------------"<<endl;
		}
	}
	return 0;
}



DWORD WINAPI MyThreadProc3(LPVOID lpParameter)//负责和身份证的通信
{
	//socket通信
	WSADATA wsaData;
	int port = 5099;     //与身份证的端口号保持一致

	char buf[] = "0"; 

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
	}

	//创建用于监听的套接字
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(port); //1024以上的端口号
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
		//socket通信
		//等待客户请求到来	
		SOCKET sockConn = accept(sockSrv, (SOCKADDR *) &addrClient, &len);
		if(sockConn == SOCKET_ERROR){
			printf("Accept failed:%d", WSAGetLastError());
		}

		//发送数据
		int iSend = send(sockConn, buf, sizeof(buf) , 0);
		if(iSend == SOCKET_ERROR){
			printf("send failed");
		}

		// 		//接收数据
		recv(sockConn, recvBuf, sizeof(recvBuf), 0);
		closesocket(sockConn);
	}
	return 0;
}