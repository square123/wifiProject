#include"myKinect.h"
#include"wifi.h"
#include <ctime> //用于时间函数
#include<sstream> //用以类型转换
#include <fstream>
#define peopleDisThd 2500 //画面检测人的深度阈值 先用ruler做测试
#define peopleTimThd 10  //筛选时间阈值
#define screenFilePath "E://projectTest//"

/*/////////////////////////////////////////////////////////////////////////
程序使用步骤：
	1.先将电脑的IP设置好 sokit调试
	2.利用ruler统计区域
	3.将阈值和矩形区域设置好
//////////////////////////////////////////////////////////////////////////
	需要做的事 统计手机亮暗屏的情况
	统计每个人员所待的时长
/////////////////////////////////////////////////////////////////////////*/
string screeenShot(Mat &src, time_t t)//用时间来命名文件名
{
	string savePathTemp=screenFilePath;
	stringstream sstr2;
	sstr2<<t;
	string str2;
	sstr2>>str2;
	savePathTemp=savePathTemp+"quan//"+str2+ ".png";
	imwrite(savePathTemp,src);
	savePathTemp=string(screenFilePath)+"ding//"+str2+ ".png";
	imwrite(savePathTemp,src);
	return savePathTemp;
}

int main()
{
	time_t t1, t2, tTemp;
	Kinect kinect;
	Wifi wifi;
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
	ofstream outfile,outfileDing,outfileRaw,outfileDingRaw;
	while(1){
		kinect.colorProcess();
		//这个还需要再重新判断下
		if((kinect.detPeopleDepth3())<peopleDisThd)//人物进入检测区的操作 进入检测区域应该是值会突然的减小 存在一个剧烈的变化，此时开始计时，并进行处理
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
			    connectImageStr=screeenShot(kinect.colorHalfSizeMat,t1);//保存图片同时将路径返回	
				outfile.open(string(screenFilePath)+"quan//"+str2+".txt",ios::app); //重新建立一个流
				outfileRaw.open(string(screenFilePath)+"quan//"+str2+"_raw"+".txt",ios::app); //重新建立一个流
				outfileDing.open(string(screenFilePath)+"ding//"+str2+".txt",ios::app); //重新建立一个流
				outfileDingRaw.open(string(screenFilePath)+"ding//"+str2+"_raw"+".txt",ios::app); //重新建立一个流
			}
			//wifi要开始记录操作
			wifi.wifiProcess(); 
		}
		else//人物离开检测区的操作
		{
			screenFlag=true;//重新将标志设为true
		    time(&t2);//获取现在的时间
			if((t1!=tTemp)&&((t2-t1)<peopleTimThd))//处理当人进入区域时间较短的操作
			{
				tTemp=t1;
				remove(connectImageStr.c_str());//c_str char转换成str
				remove(string(string(screenFilePath)+"quan//"+str2+".png").c_str());
				wifi.reSelMacRssi();//将数据清零
				outfile.close();//关闭文件
				outfileRaw.close();//关闭文件
				outfileDing.close();//关闭文件
				outfileDingRaw.close();//关闭文件
				remove(string(string(screenFilePath)+"quan//"+str2+".txt").c_str());//删除文件
				remove(string(string(screenFilePath)+"quan//"+str2+"_raw"+".txt").c_str());//删除文件
				remove(string(string(screenFilePath)+"ding//"+str2+".txt").c_str());//删除文件
				remove(string(string(screenFilePath)+"ding//"+str2+"_raw"+".txt").c_str());//删除文件
			}
			else//当时间足够长时，即成功检测的操作
			{
				if(wifiFlag==true)//只处理一次
				{
					vector<Wifi::mapUsed> outdata,outdataDing;
					wifi.wifiProcessed2(wifi.selMap,outdata,outfileRaw);//对数据进行处理并排序,同时把原始数据保存下
					wifi.wifiProcessed2(wifi.selMapDing,outdataDing,outfileDingRaw);//对数据进行处理并排序,同时把原始数据保存下
					for (int it=0;it<outdata.size();it++) //将排序好的信息输出
					{
						if (outdata[it].avgRssi!=0)//剔除0的情况
						{
							outfile<<outdata[it].macName<<" "<<outdata[it].avgRssi<<" "<<outdata[it].num<<endl;
						}
					}
					for (int it=0;it<outdataDing.size();it++) //将排序好的信息输出
					{
						if (outdataDing[it].avgRssi!=0)//剔除0的情况
						{
							outfileDing<<outdataDing[it].macName<<" "<<outdataDing[it].avgRssi<<" "<<outdataDing[it].num<<endl;
						}
					}
					wifi.reSelMacRssi();//将数据清零
					outfile.close();//关闭文件
					outfileDing.close();
				}
				wifiFlag=false;
			}		
		}
		if(waitKey(3)==27)
		{
			break;
		}
	}
	exit(0);
}
