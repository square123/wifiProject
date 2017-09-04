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
#define rotateAngle 0.0 //旋转角度
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
	int magicNumber; //数为-1时表示数据为空，为零时表示在使用，当一轮没有使用时将非0和1的数据增大，当数据增到的一定程度时，将数据置为-1，并将直方图清零（辅助更新直方图的数据）
	int strIndex; //存储的索引，更新直方图的关键
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
	Mat bodyIndexMat;//显示为彩色图像的bodyIndex
	Mat bodyIndexMatInt;//显示为数值的bodyIndex
	Mat CoordinateMapperMat;//匹配后的矩阵
	Mat CoordinateMapperMat_TM;//掩模后的函数，用于显示结果
	Mat TwoMat;//二值图像矩阵
	Mat TwoMatTemp;
	Mat depthDeTemp;//做一个全零的深度矩阵，为coordinate服务
	Mat showOrbit; //轨迹显示矩阵
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	HRESULT InitBodyIndex();
	HRESULT Initbody();
	ofstream outfile;//用于存储轨迹数据的
	void colorProcess();
	void depthProcess();//该方法在读取16位时，内存会出现问题,采用的是整体赋值
	void depthProcess2();//该方法没有问题，采用的是分别赋值
	void bodyIndexProcess();
	void kincetSave(Mat a,string savePath,int opt);//有两种模式选择，保存彩色或深度
	void kinectSaveAll(string savePath);//同时保存彩色和深度
	void bodyLocation();
	void kinectDataRawProecess(char dataTime[14],int BIndex,double r1,double r2,double r3,double r4);
	void kinectDataProProecess();
	int charTimeGetSecond(char ttt[14]);//获得得到数据的后两位
	void bodyColorHistGet(int index,HistBind &dst);
	void CoordinateMapperProcess();//获得CoordinateMapper
	int reNewIndex(int i); //用来更新新的索引
	double histBindCompare(HistBind &src1,HistBind &src2);

	//旋转因子转换函数 
	//画出区域检测函数
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
	struct KinectDataRaw //获得的Kinect元素
	{
		char Timestamp[14];
		double r[4];//记录距离信息
	}kinectTimeRaw[60][BODY_COUNT][maxStore];//用来存储按照时间整合后的kinect数据
	int kinectTIIndex[60][BODY_COUNT];//存储上述记录的索引
	UINT8 bodyInt[6];//用于区别6个人
	//直方图更新用
	HistBindAndIndex storeTemp[6];//用来更新存储
	ofstream deee; //要删除的东西！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
	Mat histElement;
	int trackStateHold[6];
};
#endif