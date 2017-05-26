#include "ViBe.h"
#include "gamma.h"
#include "overlapPro.h"
#include <cstdio>
#include "myKinect.h"
#include "others.h"

using namespace cv;
using namespace std;

//还需要完成连通区域的连续标记部分，可能高斯模型比较慢，暂时先不考虑这个。合并也再说
//通过彩色模型或许程序会比较慢，看看能不能通过连续的情况实现。这个是整个程序的关键，或许有现成的跟踪函数
//这个是重头，完成了连续标记

int main(int argc, char* argv[])
{
	//Kinect部分
	//Kinect kinect;
	//kinect.InitKinect();
	//kinect.InitColor();
	//kinect.colorProcess();

	//video获取部分
	VideoCapture capture;
	//capture.open("bodyvideo2.avi");
	capture.open("4ren.mp4");

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


	while (1)//处理部分
	{	
		Mat frame;

		////kinect部分
		//kinect.colorProcess();
		//resize(kinect.colorHalfSizeMat,frame,Size(),0.3,0.3);

		////获取已保存的数据部分
		Mat framed;
		capture>>framed;
		resize(framed,frame,Size(),0.5,0.5);
		imshow("读视频",frame);

		////图像预处理部分
		if (frame.empty())
			break;
		//gammaCorrection (frame, 0.8);//只是调整了一下整体亮度，并没有很好的效果 而且加入这个速度会变得很慢
        Mat  gray,outed;
		//medianBlur(frame,out,7);
		cvtColor(frame, outed, CV_RGB2GRAY);
		//equalizeHist(outed,out);
		bilateralFilter(outed,gray,7,14,-1);
		imshow("input",gray);	

		////GMM部分
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

		if ( cvWaitKey(50) == 'q' )
		{
			//kinect.~Kinect();//这个函数可能写的有一些问题，在进行kinect的释放中，释放的大于未释放的，其增加了复杂度。
			break;
		}
	}
	//kinect.~Kinect();
	return 0;
}
