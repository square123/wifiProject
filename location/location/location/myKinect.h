#ifndef MYKINECT_H
#define MYKINECT_H
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <strsafe.h>
#include <string>
#include <kinect.h>
#include<fstream>
#include<math.h>
#include <time.h>

#define FilePath "H://testColor//"
#define saveDepth 1
#define saveColor 2
#define PI 3.1415926
#define rotateAngle 0.0 //��ת�Ƕ�
using namespace cv;
using namespace std;

template<class Interface> 
inline void SafeRelease( Interface *& pInterfaceToRelease )
{
	if ( pInterfaceToRelease != NULL )
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}


class Kinect
{
public:
	static const int DepthWidth = 512;
	static const int DepthHeight = 424;
	static const int ColorWidth = 1920;
	static const int ColorHeight = 1080;

	char saveTmpStrDep[1000];
	int saveTmpIntDep;
	char saveTmpStrClr[1000];
	int saveTmpIntClr;

	Mat depthMat8;
	Mat depthMat16;
	Mat colorMat;
	Mat colorHalfSizeMat;
	Mat CoordinateMapperMat;//ƥ���ľ���
	Mat CoordinateMapperMat_TM;//��ģ��ĺ�����������ʾ���
	Mat TwoMat;//��ֵͼ�����
	Mat TwoMatTemp;
	Mat depthDeTemp;//���ͼ��ƽ��ȥ��ľ���
	Mat showOrbit;
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	HRESULT Initbody();
	ofstream outfile;//���ڴ洢�켣���ݵ�
	void colorProcess();
	void depthProcess();//�÷����ڶ�ȡ16λʱ���ڴ���������,���õ������帳ֵ
	void depthProcess2();//�÷���û�����⣬���õ��Ƿֱ�ֵ
	void kincetSave(Mat a,string savePath,int opt);//������ģʽѡ�񣬱����ɫ�����
	void kinectSaveAll(string savePath);//ͬʱ�����ɫ�����
	void bodyLocation();
	//��ת����ת������ 
	//���������⺯��
	//ʱ���������
	//��Ҫ����bodyIndex���洢�ض��Ĺ켣,������Ҫ�Ѻ�����һ��
	Kinect();
	~Kinect();

private:
	int sOX,sOY;
	IKinectSensor* pSensor ;
	IColorFrameSource* pColorSource;
	IColorFrameReader* pColorReader;
	IFrameDescription* pColorDescription;
	IColorFrame* pColorFrame;
	IDepthFrameSource* pDepthSource;
	IDepthFrameReader* pDepthReader;
	IFrameDescription* pDepthDescription;
	IDepthFrame* pDepthFrame;
	IBodyFrameSource* pBodySource;
	IBodyFrameReader* pBodyReader;
	IBodyFrame* pBodyFrame;
	ICoordinateMapper* pCoordinateMapper;
};
#endif