#ifndef _KINECT_H_   //����������䣨��ֹ�����ظ�������
#define _KINECT_H_

//�õ���ͷ�ļ�
#include<stdio.h>
#include<conio.h>
#include<opencv2/opencv.hpp>
#include <iostream> 
#include <Kinect.h>
#include<cmath>
#include<ctime>

using namespace cv;   //�����ռ�
using namespace std;

#define peopleDisThd 1500
//��̨�������� ���� ��������
#define rec_tl Point(202,150)
#define rec_br Point(301,202)

//�ɼ�����
#define rec_TL Point(240,0)
#define rec_BR Point(720,539)

//��ȫ�ͷ�ָ�����ģ��
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)   //��������
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
	int Num;         //ȫ������������Ŀ
	int NumOnce;     //ͳ��ĳһ����������������Ķ���֡
	int Temp;        //��¼�ϴκ��´ε������ŵı仯

	static const int DepthWidth = 512;    //���ͼ�ĳ���
	static const int DepthHeight = 424;   //���ͼ�Ŀ��
	static const int ColorWidth = 1920;   //��ɫͼ�ĳ���
	static const int ColorHeight = 1080;  //��ɫͼ�Ŀ��
   
	vector<int> compression_params;
	char chari[1000]; 
	string dst;
	string dst_temp;


	float DepthShow[6];   //����ʵʱ��ʾ���������˵����ֵ
	float DepthSort[6];   //���ڸ��������˵����ֵ������ȴ�������һ������

	//24��������״̬��־λ
	int state[6][25];

	//24�������ڵ�������
	float coord_x[6][25],coord_y[6][25];

	time_t T_enter;  //������������ֵ�仯������ֵ���ڼ�¼��ʱ��
	time_t T_leave;  //����ĳ���������������ɸѡ����ʱ��¼��ʱ��
	

	bool T_enter_flag;   //���������жϽ����뿪ʱ�̵�2����־λ   
	bool T_leave_flag;

	Mat img;    //1920x1080�Ĳ�ͼ
	Mat color;  //��Сһ��Ĳ�ͼ
	Mat bodyMat;     //���ϸ�����Ϣ�Ĳ�ɫǰ��ͼ
	Mat depthMat8;  //8λ��ʾ�����ͼ
	Mat depthMat16;  //16λ��ʾ�����ͼ
	Mat bodyIndexMat;   //bodyindex��ǰ������ͼ
	Mat bodyIndexMatInt;

	unsigned char* pBodyIndexBuffer;  //��2��������д�������Ϊ����Ӻ�������Ҫ�õ�
	UINT16 *pDepthBuffer;
	
	//Kienct�ĳ�ʼ��������Ҫ�õ���һЩ����
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT Initbody();
	HRESULT InitBodyIndex();
	HRESULT InitCoorMap();

	//������������
	void bodyIndexProcess();
	void depthProcess();
	void colorProcess();
	void bodyProcess2();

	void segmentProcess2(int);
	void SegmentAllParts(float,float,float,float,int,int,int);
	void  select2(int);
    
	void depthProcess2();
	short detPeopleDepth3();
	short medianOut(Mat&src,Point &tl,Point &br);//��16λ���ͼ������˲�

//���캯������������
	Kinect();
	~Kinect();

private:
	//��Ҫ�õ���ָ��
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

	//��ʱ������������������������������
	string timeIntToTimestampString(time_t input);
};
#endif
