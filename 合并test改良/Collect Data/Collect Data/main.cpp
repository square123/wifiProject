#include"Kinect.h"
#include "wifi.h"
#include <ctime> //����ʱ�亯��
#include<sstream> //��������ת��
#include <fstream>

#define screenFilePath "E://�������//projectTest//" //���ļ���·��

DWORD WINAPI MyThreadProc1(LPVOID lpParameter);
DWORD WINAPI MyThreadProc2(LPVOID lpParameter);

time_t globTime;
bool globflag=0;

int main()
{
	HANDLE handle1,handle2;
	handle1 = CreateThread(NULL,0,MyThreadProc1,NULL,0,NULL);
	handle2 = CreateThread(NULL,0,MyThreadProc2,NULL,0,NULL);

	WaitForSingleObject(handle1,INFINITE);//����������ͬʱ�����ˡ�
	WaitForSingleObject(handle2,INFINITE);

	CloseHandle(handle1);
	CloseHandle(handle2);
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

		rectangle(myKinect.depthMat8,rec_tl,rec_br,Scalar(255,255,255),3,8);
		imshow("depth",myKinect.depthMat8);

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
		rectangle(myKinect.bodyMat,rec_TL,rec_BR,Scalar(255,255,0),1,8);
		imshow("body",myKinect.bodyMat);
		waitKey(10);
	}  
	return 0;
}

DWORD WINAPI MyThreadProc2(LPVOID lpParameter)
{
	time_t t1=0; //��¼�������ʷʱ��
	Wifi wifi;
	//wifi��ʼ��
	wifi.InitWifi();
	bool waitFlag=true;//true�����Ǵ��ھ�ֹλ�ģ�false �����Ѿ����ٵȴ��ڴ�������
	bool fileFlag=true;  //true�����ļ��ǿ���ʹ�õģ�����û�д���
	//���ڴ洢
	string str2;  //ת�����·��
	stringstream sstr2; //ת����
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
			//�ļ���ʼ�Ĳ���	
			sstr2<<t1;
			sstr2>>str2;
			outfileDing.open(string(screenFilePath)+"ding//"+str2+".txt",ios::app); //����������
			outfileDingRaw.open(string(screenFilePath)+"ding//"+str2+"_raw"+".txt",ios::app); //ԭʼ����
			fileFlag=false;
			cout<<"�ļ���ʼ----------------------------------------------------------------"<<endl;
		}
		else if (waitFlag==true&&fileFlag==false)
		{
			//һ�ν�����Ҫ��ɵĲ���
			vector<Wifi::mapUsed> outdataDing;
			wifi.wifiProcessed2(wifi.selMapDing,outdataDing,outfileDingRaw,outfileDing);//�����ݽ��д�������,ͬʱ��ԭʼ���ݱ�����
			//������� �����ȿ���

			wifi.reSelMacRssi();//����������
			outfileDing.close();//�ر��ļ�
			outfileDingRaw.close();//�ر��ļ�
			fileFlag=true;
			cout<<"�ļ�����-----------------------------------------------------------"<<endl;
		}
	}
}
