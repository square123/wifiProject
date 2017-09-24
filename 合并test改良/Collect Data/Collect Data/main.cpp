#include"Kinect.h"
#include "wifi.h"
#include <ctime> //用于时间函数
#include<sstream> //用以类型转换
#include <fstream>

#define screenFilePath "E://数据输出//projectTest//" //存文件的路径

DWORD WINAPI MyThreadProc1(LPVOID lpParameter);
DWORD WINAPI MyThreadProc2(LPVOID lpParameter);

time_t globTime;
bool globflag=0;

int main()
{
	HANDLE handle1,handle2;
	handle1 = CreateThread(NULL,0,MyThreadProc1,NULL,0,NULL);
	handle2 = CreateThread(NULL,0,MyThreadProc2,NULL,0,NULL);

	WaitForSingleObject(handle1,INFINITE);//加了这句就能同时运行了。
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
		myKinect.depthProcess2();  //用于判断阈值需要用到的函数

		rectangle(myKinect.depthMat8,rec_tl,rec_br,Scalar(255,255,255),3,8);
		imshow("depth",myKinect.depthMat8);

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
		rectangle(myKinect.bodyMat,rec_TL,rec_BR,Scalar(255,255,0),1,8);
		imshow("body",myKinect.bodyMat);
		waitKey(10);
	}  
	return 0;
}

DWORD WINAPI MyThreadProc2(LPVOID lpParameter)
{
	time_t t1=0; //记录传入的历史时刻
	Wifi wifi;
	//wifi初始化
	wifi.InitWifi();
	bool waitFlag=true;//true代表是处于静止位的，false 代表已经不再等待在处理数据
	bool fileFlag=true;  //true代表文件是可以使用的，即还没有处理
	//用于存储
	string str2;  //转换后的路径
	stringstream sstr2; //转换流
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
			//文件初始的操作	
			sstr2<<t1;
			sstr2>>str2;
			outfileDing.open(string(screenFilePath)+"ding//"+str2+".txt",ios::app); //处理后的数据
			outfileDingRaw.open(string(screenFilePath)+"ding//"+str2+"_raw"+".txt",ios::app); //原始数据
			fileFlag=false;
			cout<<"文件初始----------------------------------------------------------------"<<endl;
		}
		else if (waitFlag==true&&fileFlag==false)
		{
			//一次结束后要完成的操作
			vector<Wifi::mapUsed> outdataDing;
			wifi.wifiProcessed2(wifi.selMapDing,outdataDing,outfileDingRaw,outfileDing);//对数据进行处理并排序,同时把原始数据保存下
			//输出操作 这里先空上

			wifi.reSelMacRssi();//将数据清零
			outfileDing.close();//关闭文件
			outfileDingRaw.close();//关闭文件
			fileFlag=true;
			cout<<"文件结束-----------------------------------------------------------"<<endl;
		}
	}
}
