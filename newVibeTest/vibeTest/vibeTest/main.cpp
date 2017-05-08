#include "ViBe.h"
#include "gamma.h"
#include "otherfun.h"
#include <cstdio>
#include "myKinect.h"

using namespace cv;
using namespace std;

//还需要完成连通区域的连续标记部分，可能高斯模型比较慢，暂时先不考虑这个。合并也再说
//先通过判断连通区域标记的个数来进行划分，同时要想办法提出一个阈值来剔除不合理的元素，比如较大和较小。
//通过彩色模型或许程序会比较慢，看看能不能通过连续的情况实现。这个是整个程序的关键，或许有现成的跟踪函数

//这个是重头，完成了连续标记，

int main(int argc, char* argv[])
{
	//Kinect部分
	Kinect kinect;
	kinect.InitKinect();
	kinect.InitColor();
	kinect.colorProcess();

	//video获取部分
	//VideoCapture capture;
	//capture.open("4.mp4");

	//vibe部分
	Mat  gray, mask;
	ViBe_BGS Vibe_Bgs;
	int count = 0;

	//GMM 部分 为了程序速度暂时先不用
	//cv::BackgroundSubtractorMOG mog(5,5,0.7,10);
	//Mat foregroud,background;

	//图像处理部分
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	
	while (1)
	{	
		Mat frame;
		////kinect部分
		kinect.colorProcess();
		resize(kinect.colorHalfSizeMat,frame,Size(),0.5,0.5);

		////摄像头获取数据部分
		//Mat framed;
		//capture >> framed;
		//resize(framed,frame,Size(),0.25,0.25);

		////图像预处理部分
		if (frame.empty())
			break;
		gammaCorrection (frame, 0.8);//只是调整了一下整体亮度，并没有很好的效果
        Mat out,outed;
		//medianBlur(frame,out,7);
		cvtColor(frame, outed, CV_RGB2GRAY);
		//equalizeHist(outed,out);
		bilateralFilter(outed,gray,7,14,-1);

		////GMM部分
		//mog(gray,foregroud,0.001);
		////mog.getBackgroundImage(background);//加入这句会出bug

		////vibe部分
		count++;
		Mat masked;
		if (count == 1)
		{
			Vibe_Bgs.init(gray);
			Vibe_Bgs.processFirstFrame(gray);
			cout<<"Init complete!"<<endl;
		}
		else
		{
			Vibe_Bgs.testAndUpdate(gray);
			mask = Vibe_Bgs.getMask();

			//vibe 显示部分
			fillHole(mask,masked);
			erode(masked,masked,element);
			dilate(masked,masked,element);
			imshow("mask", masked);
		}

		imshow("input",gray);	

		////GMM显示部分
		Mat maskeded;
		//fillHole(foregroud,maskeded);
		//erode(maskeded,maskeded,element);
		//dilate(maskeded,maskeded,element);
		//imshow("GMM foreground",maskeded);

		////合并部分
		//Mat mergeMat;
		//if (count != 1)//要注意第一位时没有图的
		//{
		//	cv::bitwise_or(masked,maskeded,mergeMat);
		//	imshow("merge",mergeMat);
		//}
		
		if ( cvWaitKey(1) == 'q' )
			break;
	}
	kinect.~Kinect();
	return 0;
}
