#include"myKinect.h"
#include"wifi.h"
#include <ctime> //用于时间函数
#include<sstream> //用以类型转换
#define peopleDisThd 1400 //画面检测人的深度阈值，需根据实验调试
#define peopleTimThd 15  //筛选时间阈值
#define screenFilePath "H://testbody//"
//程序计划：
//1.先完成通过深度来判定开始的条件									完成！
//2.完成截图的功能，且只拍一次，还要完成提出时间较短的数据记录      完成！
//3.将采集mac的程序写成函数											完成！
//4.将Mac的采集做一个优化，可以使用结构体，完成匹配					完成！
//5.将程序深度检测的宽度范围缩小，只检测小区域的
//6.提高程序的速度
//7.考虑直接用kinect检测人脸
//8*.最终根据程序的效果，将程序封装成界面

//使用关节点采用的方法记录，毕竟也是智慧结晶，留作记录
//float avgDet(float a[])//取均值函数
//{
//	//int n=sizeof(a)/sizeof(a[0]);//正常情况下应该这么用，但为了简便直接去除以已知的数组长度6
//	float sum=0;
//	for(int i=0;i<10;i++)
//	{
//		sum=sum+a[i];
//	}
//	return(sum/10.0);
//}
//float medDet(float a[])//取中值函数
//{
//	int n=10;
//	for (int i=1; i<n; i++)
//    {
//        for (int j=0; j<n-1; j++)
//        {
//            if (a[j]>a[j+1])
//            {
//                swap(a[j],a[j+1]); 
//            }
//        }
//    }
//	return a[(n+1)/2];
//}

string screeenShot(Mat &src, time_t t)//用时间来命名文件名
{
	string savePathTemp=screenFilePath;
	stringstream sstr2;
	sstr2<<t;
	string str2;
	sstr2>>str2;
	savePathTemp=savePathTemp+ "scene_" +str2+ ".png";
	imwrite(savePathTemp,src);
	return savePathTemp;
}

//产生长度为length的随机字符串  

//主程序
int main(int argc, char** argv )
{
	time_t t1, t2, tTemp;
	Kinect kinect;
	Wifi wifi;
	//采用关节点的方法，因不稳定所以不用
	//float peopleDet;
	//float smoothDet[10]={8.0,8.0,8.0,8.0,8.0,8.0,8.0,8.0,8.0,8.0};//需要将数完整的初始化

	//kinect 初始化
	kinect.InitKinect();
	kinect.InitColor();
	kinect.InitBodyIndex();
	kinect.InitDepth();
	//wifi初始化
	wifi.InitWifi();
 	bool screenFlag=true;//拍照标志位
	bool wifiFlag=false;//wifi处理标志位

    //时间初始化
	time(&t1);
	tTemp=t1;

	//照相参数设置
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	string connectImageStr;
	string str2;
	while(1){
		kinect.colorProcess();
		//采用关节点的方法，因不稳定所以不用
		//smoothDet[i%10]=kinect.detPeopleDepth();//平滑可能出现的噪声点,这样能让数组迭代使用,然而发现并不是很好用
		//imshow("aa",kinect.colorHalfSizeMat);
		//cout<<"实时检测深度："<<smoothDet[i%10]<<endl;
		//cout<<"滤波后结果："<<medDet(smoothDet)<<endl;//中值滤波比平均滤波效果好
		//_sleep(100);
		//if(medDet(smoothDet)< peopleDetThd){
	
		if(kinect.detPeopleDepth2()< peopleDisThd)//人物进入检测区的操作
		{
			wifiFlag=true;
			if (screenFlag==true)
			{
				screenFlag=false;
				time(&t1);
				stringstream sstr2;
				sstr2<<t1;
				sstr2>>str2;
				//截图操作 只进行一次
				//cout<<"拍照啦"<<endl;
				//cout<<t1<<endl;
			    connectImageStr=screeenShot(kinect.colorHalfSizeMat,t1);//保存图片同时将路径返回				
			}
			//wifi要开始记录操作
			wifi.wifiProcess();
		}
		else//人物离开检测区的操作
		{
			screenFlag=true;//重新将标志设为true
		    time(&t2);
			if((t1!=tTemp)&&((t2-t1)<peopleTimThd))//处理当人进入区域时间较短的操作
			{
				tTemp=t1;
				remove(connectImageStr.c_str());//c_str char转换成str
				wifi.reSelMacRssi();//将数据清零
			}
			else//当时间足够长时，即成功检测的操作
			{
				if(wifiFlag==true)//只处理一次
				{
					string macImageStr;
					//对wifi数据进行操作
					macImageStr=wifi.wifiProcessed();
					//根据上述操作返回的指令来决定是什么都不做，还是将照片重命名
					if(macImageStr!="0")
					{
						macImageStr=screenFilePath+str2+"_"+macImageStr+ ".png";
						rename(connectImageStr.c_str(),macImageStr.c_str());
					}	
					wifi.reSelMacRssi();//将数据清零
				}
				wifiFlag=false;
			}		
		}

		//采用关节点的方法，因不稳定所以不用
		//i++;//递增
		//if(i==60000) i=0;//避免溢出

		if(waitKey(3)==27)
		{
			break;
		}
	}
	wifi.~Wifi();
	kinect.~Kinect();
	exit(0);
}
