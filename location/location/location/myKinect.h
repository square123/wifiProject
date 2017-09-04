#ifndef MYKINECT_H
#define MYKINECT_H
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <kinect.h>
#include<fstream>
#include<math.h>
#include <time.h>
#define FilePath "H://testColor//"
#define saveDepth 1
#define saveColor 2
#define PI 3.1415926
#define maxStore 30
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

struct HistBind 
{
	Mat redHist;
	Mat greenHist;
	Mat blueHist;
	HistBind & operator =(HistBind & src1){
		if (this==&src1)
		{
			return *this;
		}
		this->blueHist=src1.blueHist;
		this->greenHist=src1.greenHist;
		this->redHist=src1.redHist;
		return *this;
	}
};

struct HistBindAndIndex
{
	HistBind hist;
	int magicNumber; //��Ϊ-1ʱ��ʾ����Ϊ�գ�Ϊ��ʱ��ʾ��ʹ�ã���һ��û��ʹ��ʱ����0��1���������󣬵�����������һ���̶�ʱ����������Ϊ-1������ֱ��ͼ���㣨��������ֱ��ͼ�����ݣ�
	int strIndex; //�洢������������ֱ��ͼ�Ĺؼ�
	HistBindAndIndex & operator =(HistBindAndIndex & src1){
		if (this==&src1)
		{
			return *this;
		}
		this->hist=src1.hist;
		this->magicNumber=src1.magicNumber;
		return *this;
	}
};

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
	Mat bodyIndexMat;//��ʾΪ��ɫͼ���bodyIndex
	Mat bodyIndexMatInt;//��ʾΪ��ֵ��bodyIndex
	Mat CoordinateMapperMat;//ƥ���ľ���
	Mat CoordinateMapperMat_TM;//��ģ��ĺ�����������ʾ���
	Mat TwoMat;//��ֵͼ�����
	Mat TwoMatTemp;
	Mat depthDeTemp;//��һ��ȫ�����Ⱦ���Ϊcoordinate����
	Mat showOrbit; //�켣��ʾ����
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	HRESULT InitBodyIndex();
	HRESULT Initbody();
	ofstream outfile;//���ڴ洢�켣���ݵ�
	void colorProcess();
	void depthProcess();//�÷����ڶ�ȡ16λʱ���ڴ���������,���õ������帳ֵ
	void depthProcess2();//�÷���û�����⣬���õ��Ƿֱ�ֵ
	void bodyIndexProcess();
	void kincetSave(Mat a,string savePath,int opt);//������ģʽѡ�񣬱����ɫ�����
	void kinectSaveAll(string savePath);//ͬʱ�����ɫ�����
	void bodyLocation();
	void kinectDataRawProecess(char dataTime[14],int BIndex,double r1,double r2,double r3,double r4);
	void kinectDataProProecess();
	int charTimeGetSecond(char ttt[14]);//��õõ����ݵĺ���λ
	void bodyColorHistGet(int index,HistBind &dst);
	void CoordinateMapperProcess();//���CoordinateMapper
	int reNewIndex(int i); //���������µ�����
	double histBindCompare(HistBind &src1,HistBind &src2);

	//��ת����ת������ 
	//���������⺯��
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
	IBodyIndexFrameSource* pBodyIndexSource;
	IBodyIndexFrameReader* pBodyIndexReader;
	IBodyIndexFrame* pBodyIndexFrame;
	time_t processIndex;
	struct KinectDataRaw //��õ�KinectԪ��
	{
		char Timestamp[14];
		double r[4];//��¼������Ϣ
	}kinectTimeRaw[60][BODY_COUNT][maxStore];//�����洢����ʱ�����Ϻ��kinect����
	int kinectTIIndex[60][BODY_COUNT];//�洢������¼������
	UINT8 bodyInt[6];//��������6����
	//ֱ��ͼ������
	HistBindAndIndex storeTemp[6];//�������´洢
	ofstream deee; //Ҫɾ���Ķ�����������������������������������������������������������������������������������������������������������
	Mat histElement;
	int trackStateHold[6];
};
#endif