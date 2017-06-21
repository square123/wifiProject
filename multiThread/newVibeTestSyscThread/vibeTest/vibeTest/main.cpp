#include "ViBe.h"
#include "gamma.h"
#include "overlapPro.h"
#include <cstdio>
#include "myKinect.h"
#include "others.h"
#include "WifiProbe.h"

using namespace cv;
using namespace std;

DWORD WINAPI MyThreadProc1(LPVOID lpParameter);
DWORD WINAPI MyThreadProc2(LPVOID lpParameter);

//还需要完成连通区域的连续标记部分，可能高斯模型比较慢，暂时先不考虑这个。合并也再说
//通过彩色模型或许程序会比较慢，看看能不能通过连续的情况实现。这个是整个程序的关键，或许有现成的跟踪函数
//这个是重头，完成了连续标记

int main(int argc, char* argv[])
{
	HANDLE handle1,handle2;
	handle1 = CreateThread(NULL,0,MyThreadProc1,NULL,0,NULL);
	handle2 = CreateThread(NULL,0,MyThreadProc2,NULL,0,NULL);
	WaitForSingleObject(handle1,INFINITE);//加了这句就能同时运行了。
	WaitForSingleObject(handle1,INFINITE);
	CloseHandle(handle1);
	CloseHandle(handle2);
	return 0;
}

DWORD WINAPI MyThreadProc1(LPVOID lpParameter)
{
	//Kinect部分
	Kinect kinect;
	kinect.InitKinect();
	kinect.InitColor();
	kinect.colorProcess();

	//video获取部分
	//VideoCapture capture;
	////capture.open("bodyvideo2.avi");
	//capture.open("4ren.mp4");

	//vibe部分   利用vibe算法，其对颜色敏感程度较高，但在处理进行跟踪时，图像会出现较大的损失。
	//Mat  mask;
	//ViBe_BGS Vibe_Bgs;
	//int count = 0;

	//GMM 部分 为了程序速度暂时先不用
	cv::BackgroundSubtractorMOG mog(5,5,0.7,10);
	Mat foregroud,background;

	//opticalflow 部分（不好用）
	//Mat prvGray, optFlow ,absoluteFlow, img_for_show;

	//图像处理部分
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(3,3));

	//overlap算法部分
	overlapPro olr;
	olr.outfile.open("cameradata.csv",fstream::app);
	//设置掩模图像
	Mat maskMat=Mat::zeros(270,480,CV_8UC1);
	line(maskMat,Point(219,79),Point(257,79),255,2,8);
	line(maskMat,Point(257,79),Point(338,269),255,2,8);
	line(maskMat,Point(338,269),Point(110,269),255,2,8);
	line(maskMat,Point(110,269),Point(219,79),255,2,8);
	floodFill(maskMat,Point(220,150),255);

	while(true)
	{
		Mat frame;
		//新建一个掩模
		/*	imshow("hhh",maskMat);*/
		////kinect部分
		kinect.colorProcess();
		resize(kinect.colorHalfSizeMat,frame,Size(),0.5,0.5);

		////获取已保存的数据部分
		//Mat framed;
		//capture>>framed;
		//resize(framed,frame,Size(),0.5,0.5);
		//imshow("读视频",frame);

		////图像预处理部分
		if (frame.empty())
			continue;
		//gammaCorrection (frame, 0.8);//只是调整了一下整体亮度，并没有很好的效果 而且加入这个速度会变得很慢
		Mat  gray,outed;
		//medianBlur(frame,out,7);
		cvtColor(frame, outed, CV_RGB2GRAY);
		//equalizeHist(outed,out);
		bilateralFilter(outed,gray,7,14,-1);
		imshow("input",gray);	
		//imshow("hhhhh",maskMat & gray);
		////GMM部分maskMat&
		mog(gray,foregroud,0.001);
		////mog.getBackgroundImage(background);//加入这句会出bug

		////vibe部分
		//count++;
		//Mat masked;
		//if (count == 1)
		//{
		//	Vibe_Bgs.init(gray);
		//	Vibe_Bgs.processFirstFrame(gray);
		//	cout<<"Init complete!"<<endl;
		//	
		//}
		//else
		//{
		//	Vibe_Bgs.testAndUpdate(gray);
		//	mask = Vibe_Bgs.getMask();

		//	//vibe 显示部分
		//	fillHole(mask,masked);
		//	erode(masked,masked,element);
		//	dilate(masked,masked,element);
		//	dilate(masked,masked,element);
		//	dilate(masked,masked,element);
		//	imshow("mask", masked);
		//	cout<<count<<endl;
		//}

		////opticalflow 部分(不好用)
		//if (prvGray.data){  
		//	calcOpticalFlowFarneback(prvGray, gray, optFlow, 0.5, 3, 15, 3, 5, 1.2, 0); //使用论文参数      
		//	compute_absolute_mat(optFlow,absoluteFlow);
		//	normalize(absoluteFlow, img_for_show, 0, 255, NORM_MINMAX, CV_8UC1);  
		//	imshow("opticalFlow", img_for_show);  
		//}  
		//cv::swap(prvGray, gray);  

		////GMM显示部分
		Mat maskeded;
		fillHole(foregroud,maskeded);
		erode(maskeded,maskeded,element);
		dilate(maskeded,maskeded,element);
		imshow("GMM foreground",maskeded);

		////vibe和GMM合并部分
		//Mat mergeMat;
		//if (count != 1)//要注意第一位时没有图的
		//{
		//	cv::bitwise_or(masked,maskeded,mergeMat);
		//	imshow("merge",mergeMat);
		//}
		olr.process(maskeded);//进行连通区域的overlap跟踪
		if ( cvWaitKey(3) == 'q' )
		{

			kinect.~Kinect();//这个函数可能写的有一些问题，在进行kinect的释放中，释放的大于未释放的，其增加了复杂度。
			break;
		}
	}
	return 0;
}

DWORD WINAPI MyThreadProc2(LPVOID lpParameter)
{
	//探针部分
	Probe probe;
	probe.InitProbe();
	while(true)
	{
			probe.probeProcess();
			if ( cvWaitKey(3) == 'q' )
			{
				probe.~Probe();
				break;
			}
	}
	return 0;
}