#include "ViBe.h"
#include "gamma.h"
#include "otherfun.h"
#include <cstdio>
#include "myKinect.h"

using namespace cv;
using namespace std;

//����Ҫ�����ͨ�����������ǲ��֣����ܸ�˹ģ�ͱȽ�������ʱ�Ȳ�����������ϲ�Ҳ��˵
//��ͨ���ж���ͨ�����ǵĸ��������л��֣�ͬʱҪ��취���һ����ֵ���޳��������Ԫ�أ�����ϴ�ͽ�С��
//ͨ����ɫģ�ͻ�������Ƚ����������ܲ���ͨ�����������ʵ�֡��������������Ĺؼ����������ֳɵĸ��ٺ���

//�������ͷ�������������ǣ�

int main(int argc, char* argv[])
{
	//Kinect����
	Kinect kinect;
	kinect.InitKinect();
	kinect.InitColor();
	kinect.colorProcess();

	//video��ȡ����
	//VideoCapture capture;
	//capture.open("4.mp4");

	//vibe����
	Mat  mask;
	ViBe_BGS Vibe_Bgs;
	int count = 0;

	//GMM ���� Ϊ�˳����ٶ���ʱ�Ȳ���
	//cv::BackgroundSubtractorMOG mog(5,5,0.7,10);
	//Mat foregroud,background;

	//opticalflow ���֣������ã�
	//Mat prvGray, optFlow ,absoluteFlow, img_for_show;

	//ͼ������
	Mat element=getStructuringElement(MORPH_ELLIPSE,Size(3,3));
	

	while (1)//������
	{	
		Mat frame;
		////kinect����
		kinect.colorProcess();
		resize(kinect.colorHalfSizeMat,frame,Size(),0.5,0.5);

		////����ͷ��ȡ���ݲ���
		//Mat framed;
		//capture >> framed;
		//resize(framed,frame,Size(),0.25,0.25);

		////ͼ��Ԥ������
		if (frame.empty())
			break;
		gammaCorrection (frame, 0.8);//ֻ�ǵ�����һ���������ȣ���û�кܺõ�Ч��
        Mat  gray,out,outed;
		//medianBlur(frame,out,7);
		cvtColor(frame, outed, CV_RGB2GRAY);
		//equalizeHist(outed,out);
		bilateralFilter(outed,gray,7,14,-1);
		imshow("input",gray);	

		////GMM����
		//mog(gray,foregroud,0.001);
		////mog.getBackgroundImage(background);//���������bug

		////vibe����
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

			//vibe ��ʾ����
			fillHole(mask,masked);
			erode(masked,masked,element);
			dilate(masked,masked,element);
			dilate(masked,masked,element);
			dilate(masked,masked,element);
			imshow("mask", masked);
			cout<<count<<endl;
		}

		////opticalflow ����(������)
		//if (prvGray.data){  
		//	calcOpticalFlowFarneback(prvGray, gray, optFlow, 0.5, 3, 15, 3, 5, 1.2, 0); //ʹ�����Ĳ���      
		//	compute_absolute_mat(optFlow,absoluteFlow);
		//	normalize(absoluteFlow, img_for_show, 0, 255, NORM_MINMAX, CV_8UC1);  
		//	imshow("opticalFlow", img_for_show);  
		//}  
		//cv::swap(prvGray, gray);  

		////GMM��ʾ����
		//Mat maskeded;
		//fillHole(foregroud,maskeded);
		//erode(maskeded,maskeded,element);
		//dilate(maskeded,maskeded,element);
		//imshow("GMM foreground",maskeded);

		////vibe��GMM�ϲ�����
		//Mat mergeMat;
		//if (count != 1)//Ҫע���һλʱû��ͼ��
		//{
		//	cv::bitwise_or(masked,maskeded,mergeMat);
		//	imshow("merge",mergeMat);
		//}
		
		//�Ӽ�⵽120֮֡��ſ�ʼ����
		if(count>=120)//���Ӧ������Ҫ����ʵ��������޸ĵ�
		{
			count--;
			//����ſ�ʼ���к���������Ȼ��ͨ����̫����
			Mat selectedMat;
			selectArea(masked,selectedMat,80,10000);//ͨ��������һЩ��ͨ��С�Ľ���ɸѡ
			imshow("dd",selectedMat);
	//�㷨�������裺
			//����ͨ�����þ��α�ʾ���������þ������
			//�ٽ���ͨ����ϲ�
			//�ٴν�����任�ɾ���
			//����һ�ַ���������ͨ������
			//�·�ͼ���������Ϸ�ͼ��ı��
			//���û�б�������¸�ֵһ���µı��
			//��Ҫ��һ���ȽϺõĺ���
			//������Ѿ�������ʱ��Ҫ��취ȥ��
		}

		if ( cvWaitKey(1) == 'q' )
			break;
	}
	kinect.~Kinect();
	return 0;
}
