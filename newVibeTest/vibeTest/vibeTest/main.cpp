#include "ViBe.h"
#include "gamma.h"
#include "otherfun.h"
#include <cstdio>
#include "myKinect.h"

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
	;
	//opticalflow ���֣������ã�
	//Mat prvGray, optFlow ,absoluteFlow, img_for_show;

	//ͼ������
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	
	//overlap�㷨����

	vector<bwMix> prePicBwMix;
	prePicBwMix.reserve(100);

	while (1)//������
	{	
		Mat frame;
		vector<bwMix> picBwMix;
		picBwMix.reserve(100);
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
        Mat  gray,out,outed;
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
		
		//�Ӽ�⵽120֮֡��ſ�ʼ���ң���Ҫ���ʱ������ʼ��ͼ�񣬲�Ȼ�����ghost����
		if(1)//���Ӧ������Ҫ����ʵ��������޸ĵ�count>=20
		{
			//count--;
			//����ſ�ʼ���к���������Ȼ��ͨ����̫����
			Mat selectedMat;
			selectArea(maskeded,selectedMat,180,4000);//ͨ��������һЩ��ͨ��С�Ľ���ɸѡ  �Ժ���Ը���ʵ���ж�ɸѡ ���ϡ������Ŀ��ĸ���
			imshow("dd",selectedMat);
			//��Ӧ���ٽ��д�С���ж� ��Ŀ�� �������ܴ�����⣬���г�����ж�
	//�㷨�������裺
			//����ͨ�����þ��α�ʾ���������þ������
			Mat hullMat;
			hullArea(selectedMat,hullMat);
			imshow("hh",hullMat);
			//�ٽ���ͨ����ϲ� ��ֹ��������ȱʧ  ����ʱ�����ǣ�Ĭ�����ܱȽϺã�
			//��ȡ�ṹ�壨���庯���ṹ�壩
			getBwMix(hullMat,picBwMix);//�õ��ṹ��

			//�Խṹ����и���
			//�·�ͼ���������Ϸ�ͼ��ı�� ��������ǰһ֡�ͺ�һ֡��
			//���û�б�������¸�ֵһ���µı�� 
			//������Ѿ�������ʱ��Ҫ��취ȥ�أ���ô����ʧ���⣬���Ҳ����Ҫ�������Ȳ����ǣ���5�½�����д�������ٿ��Ǹ����飬�Ӵ��㷨�ĸ��Ӷȣ�
			if (prePicBwMix.size()!=0)
			{
				renewVec(prePicBwMix,picBwMix,0.0);
			}
			prePicBwMix=picBwMix;//�����ݽ��и���

			//��ɫ��ʾ	//��������ɫ��ʾ����ǰ��������ֵ����ɫ��
			Mat showColor=Mat::zeros(hullMat.size(),CV_8UC3);
			bwMixToColorMat(picBwMix,showColor);
			imshow("color",showColor);
			//����켣
			bwMixToOrbit(picBwMix);

			

		}

		if ( cvWaitKey(50) == 'q' )
		{
			//kinect.~Kinect();//�����������д����һЩ���⣬�ڽ���kinect���ͷ��У��ͷŵĴ���δ�ͷŵģ��������˸��Ӷȡ�
			break;
		}
	}
	//kinect.~Kinect();
	return 0;
}
