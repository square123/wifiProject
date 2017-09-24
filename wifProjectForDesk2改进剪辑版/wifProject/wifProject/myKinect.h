/**************************************************
 * \file myKinect.h
 * \date 2017/06/27 13:15
 * \author ���Ǹ���
 * Contact: kfhao123@163.com
**************************************************/
#ifndef MYKINECT_H
#define MYKINECT_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <kinect.h>
#include<fstream>
#include<math.h>
#include <time.h>

#define FilePath "H://testbody//"

#define saveDepth 1
#define saveColor 2

#define PI 3.1415926

//��̨�������� ���� ��������
#define rec_tl Point(157,328)
#define rec_br Point(268,349)

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
public://�洢һЩ�������ʹ�õ���

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
	Mat bodyIndexMat;//��ʾΪ��ɫͼ���bodyIndex
	Mat bodyIndexMatInt;//��ʾΪ��ֵ��bodyIndex
	Mat CoordinateMapperMat;//ƥ���ľ���
	Mat CoordinateMapperMat_TM;//��ģ��ĺ�����������ʾ���
	Mat TwoMat;//��ֵͼ�����
	Mat TwoMatTemp;
	Mat depthDeTemp;//���ͼ��ƽ��ȥ��ľ���

	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	HRESULT Initbody();
	HRESULT InitBodyIndex();

	void colorProcess();
	void depthProcess();//�÷����ڶ�ȡ16λʱ���ڴ���������,���õ������帳ֵ
	void depthProcess2();//�÷���û�����⣬���õ��Ƿֱ�ֵ
	void bodyIndexProcess();

	void kincetSave(Mat a,string savePath,int opt);//������ģʽѡ�񣬱����ɫ�����
	void kinectSaveAll(string savePath);//ͬʱ�����ɫ�����

	float detPeopleDepth();//ͨ���ؽڵ����ж������Զ��������Ч�����ȶ������ǹؽڵ�̫�٣����ɺ����ж�
	short detPeopleDepth3();//ͨ���򵥵���ֵ�˲����ж����

	int areaMatForSpecNum(Mat &src,int num);//���ؾ������ض�ֵ�����
	int areaMatOri(Mat &src);//���ؾ�������,Ҫ���������ݸ�ʽΪUC1
	int avgMat(Mat &src,int num);//��ƽ���������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�,��ʵ�õĸ����ԭ���Ǹ����������
	int sumMat( Mat &src);//��ͣ������ǵ�ͨ���� ע���������ݸ�ʽ��int�͵�
	short medianOut(Mat&src,Point &tl,Point &br);//��16λ���ͼ������˲�

	Kinect();
	~Kinect();

private://�洢һЩʵ�ֵĹ�����
	IKinectSensor* pSensor ;

	IColorFrameSource* pColorSource;
	IColorFrameReader* pColorReader;
	IFrameDescription* pColorDescription;
	IColorFrame* pColorFrame;

	IDepthFrameSource* pDepthSource;
	IDepthFrameReader* pDepthReader;
	IFrameDescription* pDepthDescription;
	IDepthFrame* pDepthFrame;

	IBodyIndexFrameSource* pBodyIndexSource;
	IBodyIndexFrameReader* pBodyIndexReader;
	IBodyIndexFrame* pBodyIndexFrame;

	IBodyFrameSource* pBodySource;
	IBodyFrameReader* pBodyReader;
	IBodyFrame* pBodyFrame;
	ICoordinateMapper* pCoordinateMapper;
};
#endif
