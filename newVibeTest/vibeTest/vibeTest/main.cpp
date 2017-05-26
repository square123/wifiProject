#include "ViBe.h"
#include "gamma.h"
#include "overlapPro.h"
#include <cstdio>
#include "myKinect.h"
#include "others.h"

using namespace cv;
using namespace std;

//����Ҫ�����ͨ�����������ǲ��֣����ܸ�˹ģ�ͱȽ�������ʱ�Ȳ�����������ϲ�Ҳ��˵
//ͨ����ɫģ�ͻ�������Ƚ����������ܲ���ͨ�����������ʵ�֡��������������Ĺؼ����������ֳɵĸ��ٺ���
//�������ͷ��������������

int main(int argc, char* argv[])
{
	//Kinect����
	//Kinect kinect;
	//kinect.InitKinect();
	//kinect.InitColor();
	//kinect.colorProcess();

	//video��ȡ����
	VideoCapture capture;
	//capture.open("bodyvideo2.avi");
	capture.open("4ren.mp4");

	//vibe����   ����vibe�㷨�������ɫ���г̶Ƚϸߣ����ڴ�����и���ʱ��ͼ�����ֽϴ����ʧ��
	//Mat  mask;
	//ViBe_BGS Vibe_Bgs;
	//int count = 0;

	//GMM ���� Ϊ�˳����ٶ���ʱ�Ȳ���
	cv::BackgroundSubtractorMOG mog(5,5,0.7,10);
	Mat foregroud,background;

	//opticalflow ���֣������ã�
	//Mat prvGray, optFlow ,absoluteFlow, img_for_show;

	//ͼ������
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	
	//overlap�㷨����
	overlapPro olr;
	olr.outfile.open("cameradata.csv",fstream::app);


	while (1)//������
	{	
		Mat frame;

		////kinect����
		//kinect.colorProcess();
		//resize(kinect.colorHalfSizeMat,frame,Size(),0.3,0.3);

		////��ȡ�ѱ�������ݲ���
		Mat framed;
		capture>>framed;
		resize(framed,frame,Size(),0.5,0.5);
		imshow("����Ƶ",frame);

		////ͼ��Ԥ������
		if (frame.empty())
			break;
		//gammaCorrection (frame, 0.8);//ֻ�ǵ�����һ���������ȣ���û�кܺõ�Ч�� ���Ҽ�������ٶȻ��ú���
        Mat  gray,outed;
		//medianBlur(frame,out,7);
		cvtColor(frame, outed, CV_RGB2GRAY);
		//equalizeHist(outed,out);
		bilateralFilter(outed,gray,7,14,-1);
		imshow("input",gray);	

		////GMM����
		mog(gray,foregroud,0.001);
		////mog.getBackgroundImage(background);//���������bug

		////vibe����
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

		//	//vibe ��ʾ����
		//	fillHole(mask,masked);
		//	erode(masked,masked,element);
		//	dilate(masked,masked,element);
		//	dilate(masked,masked,element);
		//	dilate(masked,masked,element);
		//	imshow("mask", masked);
		//	cout<<count<<endl;
		//}

		////opticalflow ����(������)
		//if (prvGray.data){  
		//	calcOpticalFlowFarneback(prvGray, gray, optFlow, 0.5, 3, 15, 3, 5, 1.2, 0); //ʹ�����Ĳ���      
		//	compute_absolute_mat(optFlow,absoluteFlow);
		//	normalize(absoluteFlow, img_for_show, 0, 255, NORM_MINMAX, CV_8UC1);  
		//	imshow("opticalFlow", img_for_show);  
		//}  
		//cv::swap(prvGray, gray);  

		////GMM��ʾ����
		Mat maskeded;
		fillHole(foregroud,maskeded);
		erode(maskeded,maskeded,element);
		dilate(maskeded,maskeded,element);
		imshow("GMM foreground",maskeded);

		////vibe��GMM�ϲ�����
		//Mat mergeMat;
		//if (count != 1)//Ҫע���һλʱû��ͼ��
		//{
		//	cv::bitwise_or(masked,maskeded,mergeMat);
		//	imshow("merge",mergeMat);
		//}
		
		olr.process(maskeded);//������ͨ�����overlap����

		if ( cvWaitKey(50) == 'q' )
		{
			//kinect.~Kinect();//�����������д����һЩ���⣬�ڽ���kinect���ͷ��У��ͷŵĴ���δ�ͷŵģ��������˸��Ӷȡ�
			break;
		}
	}
	//kinect.~Kinect();
	return 0;
}
