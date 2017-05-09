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
	Mat  mask;
	ViBe_BGS Vibe_Bgs;
	int count = 0;

	//GMM 部分 为了程序速度暂时先不用
	//cv::BackgroundSubtractorMOG mog(5,5,0.7,10);
	//Mat foregroud,background;

	//opticalflow 部分（不好用）
	//Mat prvGray, optFlow ,absoluteFlow, img_for_show;

	//图像处理部分
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	

	while (1)//处理部分
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
        Mat  gray,out,outed;
		//medianBlur(frame,out,7);
		cvtColor(frame, outed, CV_RGB2GRAY);
		//equalizeHist(outed,out);
		bilateralFilter(outed,gray,7,14,-1);
		imshow("input",gray);	

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
			dilate(masked,masked,element);
			dilate(masked,masked,element);
			imshow("mask", masked);
			cout<<count<<endl;
		}

		////opticalflow 部分(不好用)
		//if (prvGray.data){  
		//	calcOpticalFlowFarneback(prvGray, gray, optFlow, 0.5, 3, 15, 3, 5, 1.2, 0); //使用论文参数      
		//	compute_absolute_mat(optFlow,absoluteFlow);
		//	normalize(absoluteFlow, img_for_show, 0, 255, NORM_MINMAX, CV_8UC1);  
		//	imshow("opticalFlow", img_for_show);  
		//}  
		//cv::swap(prvGray, gray);  

		////GMM显示部分
		//Mat maskeded;
		//fillHole(foregroud,maskeded);
		//erode(maskeded,maskeded,element);
		//dilate(maskeded,maskeded,element);
		//imshow("GMM foreground",maskeded);

		////vibe和GMM合并部分
		//Mat mergeMat;
		//if (count != 1)//要注意第一位时没有图的
		//{
		//	cv::bitwise_or(masked,maskeded,mergeMat);
		//	imshow("merge",mergeMat);
		//}
		
		//从检测到120帧之后才开始查找
		if(count>=120)//这个应该是需要根据实际情况能修改的
		{
			count--;
			//这里才开始进行函数处理，不然连通区域太多了
			Mat selectedMat;
			selectArea(masked,selectedMat,80,10000);//通过函数对一些连通域小的进行筛选
			imshow("dd",selectedMat);
	//算法后续步骤：
			//将连通区域用矩形表示，将区域用矩形填充
			//再将连通区域合并
			//再次将区域变换成矩形
			//考虑一种方法，将连通区域标记
			//下幅图像标记引用上幅图像的标记
			//如果没有标记则重新赋值一个新的标记
			//需要有一个比较好的函数
			//当标记已经不存在时，要想办法去重
		}

		if ( cvWaitKey(1) == 'q' )
			break;
	}
	kinect.~Kinect();
	return 0;
}
