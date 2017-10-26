#ifndef _KINECT_H_   //条件编译语句（防止类名重复声明）
#define _KINECT_H_

//用到的头文件
#include<stdio.h>
#include<conio.h>
#include<opencv2/opencv.hpp>
#include <iostream> 
#include <Kinect.h>
#include<cmath>
#include<ctime>

using namespace cv;   //命名空间
using namespace std;

#define peopleDisThd 1500
//柜台区域设置 左上 右下坐标
#define rec_tl Point(202,150)
#define rec_br Point(301,202)

//采集区域
#define rec_TL Point(240,0)
#define rec_BR Point(720,539)

//安全释放指针的类模板
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)   //内联函数
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}


class Kinect
{
public:   
	int Num;         //全局已拍人体数目
	int NumOnce;     //统计某一个索引号人体拍摄的多少帧
	int Temp;        //记录上次和下次的索引号的变化

	static const int DepthWidth = 512;    //深度图的长度
	static const int DepthHeight = 424;   //深度图的宽度
	static const int ColorWidth = 1920;   //彩色图的长度
	static const int ColorHeight = 1080;  //彩色图的宽度
   
	vector<int> compression_params;
	char chari[1000]; 
	string dst;
	string dst_temp;


	float DepthShow[6];   //用于实时显示各索引号人的深度值
	float DepthSort[6];   //用于各索引号人的深度值排序而先存下来的一个数组

	//24个骨骼点状态标志位
	int state[6][25];

	//24个骨骼节点坐标编号
	float coord_x[6][25],coord_y[6][25];

	time_t T_enter;  //设置区域的深度值变化到了阈值子内记录的时刻
	time_t T_leave;  //当对某个索引号人体进行筛选工作时记录的时刻
	

	bool T_enter_flag;   //设置用于判断进入离开时刻的2个标志位   
	bool T_leave_flag;

	Mat img;    //1920x1080的彩图
	Mat color;  //减小一半的彩图
	Mat bodyMat;     //集合各种信息的彩色前景图
	Mat depthMat8;  //8位显示的深度图
	Mat depthMat16;  //16位显示的深度图
	Mat bodyIndexMat;   //bodyindex的前景轮廓图
	Mat bodyIndexMatInt;

	unsigned char* pBodyIndexBuffer;  //这2个语句必须写在这里，因为多个子函数中需要用到
	UINT16 *pDepthBuffer;
	
	//Kienct的初始化工作需要用到的一些函数
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT Initbody();
	HRESULT InitBodyIndex();
	HRESULT InitCoorMap();

	//处理函数的声明
	void bodyIndexProcess();
	void depthProcess();
	void colorProcess();
	void bodyProcess2();

	void segmentProcess2(int);
	void SegmentAllParts(float,float,float,float,int,int,int);
	void  select2(int);
    
	void depthProcess2();
	short detPeopleDepth3();
	short medianOut(Mat&src,Point &tl,Point &br);//对16位深度图像进行滤波

//构造函数和析构函数
	Kinect();
	~Kinect();

private:
	//需要用到的指针
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

	IInfraredFrameSource* pInfraredSource;
	IInfraredFrameReader* pInfraredReader;
	IInfraredFrame* pInfraredFrame;

	IBodyFrameSource* pBodySource;
	IBodyFrameReader* pBodyReader;
	IBodyFrame* pBodyFrame;
	ICoordinateMapper* pCoordinateMapper;

	//临时！！！！！！！！！！！！！！！
	string timeIntToTimestampString(time_t input);
};
#endif
