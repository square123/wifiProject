#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include <opencv2/photo/photo.hpp>//inpaint函数的头文件
#include <iostream>
#include <strsafe.h>
#include <string>
#include <kinect.h>
#include<fstream>
#include<math.h>

#define FilePath "H://testColor//"
#define saveDepth 1
#define saveColor 2
#define PI 3.1415926
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
	Mat CoordinateMapperMat;//匹配后的矩阵
	Mat CoordinateMapperMat_TM;//掩模后的函数，用于显示结果
	Mat TwoMat;//二值图像矩阵
	Mat TwoMatTemp;
	Mat depthDeTemp;//深度图像平均去噪的矩阵
	Mat showOrbit;
	HRESULT hResult ;
	HRESULT InitKinect();
	HRESULT InitDepth();
	HRESULT InitColor();
	HRESULT InitCoorMap();
	HRESULT Initbody();
	ofstream outfile;//用于存储轨迹数据的
	void colorProcess();
	void depthProcess();//该方法在读取16位时，内存会出现问题,采用的是整体赋值
	void depthProcess2();//该方法没有问题，采用的是分别赋值
	void kincetSave(Mat a,string savePath,int opt);//有两种模式选择，保存彩色或深度
	void kinectSaveAll(string savePath);//同时保存彩色和深度
	void bodyLocation();
	Kinect();
	~Kinect();

private:
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

//主程序
int main(int argc, char** argv )
{
	Kinect kinect;  
	kinect.outfile.open("data.txt",fstream::app);
	//Mat depthDeNoise(kinect.DepthHeight, kinect.DepthWidth, CV_16UC1);
	kinect.InitKinect();
	kinect.InitDepth();
	kinect.InitColor();
	kinect.InitCoorMap();
	kinect.Initbody();
	while(1){
		kinect.colorProcess();
		kinect.depthProcess();
		kinect.bodyLocation();
		imshow("haha",kinect.showOrbit);
		imshow("depth",kinect.depthMat8);
		imshow("color",kinect.colorHalfSizeMat);
		if(waitKey(3)==27)
		{
			break;
		}
	}
	kinect.~Kinect();

}

Kinect::Kinect()  
{
	pColorFrame = nullptr;
	pDepthFrame = nullptr;
	colorHalfSizeMat.create(ColorHeight/2, ColorWidth/2, CV_8UC4);
	colorMat.create( ColorHeight, ColorWidth, CV_8UC4);
	depthMat16.create(DepthHeight, DepthWidth, CV_16UC1);
	depthMat8.create(DepthHeight, DepthWidth, CV_8UC1 );
	
	saveTmpIntClr=0;
	saveTmpIntDep=0;
}
Kinect::~Kinect()
{
	outfile.close();
	SafeRelease( pDepthSource );
	SafeRelease( pDepthReader );
	SafeRelease( pDepthDescription );
	SafeRelease( pColorSource );
	SafeRelease( pColorReader );
	SafeRelease( pColorDescription );
	SafeRelease( pCoordinateMapper );
	SafeRelease( pBodySource );
	SafeRelease( pBodyReader );
	if ( pSensor )
	{
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();
}
HRESULT Kinect::InitKinect()
{
	hResult = GetDefaultKinectSensor( &pSensor );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:GetDefaultKinectSensor" <<endl;
		return -1;
	}
	hResult = pSensor->Open();
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::Open()" <<endl;
		return -1;
	}
}
HRESULT Kinect::InitColor()//必须先InitKinect()
{
	hResult = pSensor->get_ColorFrameSource( &pColorSource );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_ColorFrameSource()"<<endl;
		return -1;
	}
	hResult = pColorSource->OpenReader( &pColorReader );     
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
		return -1; 
	}

}
HRESULT Kinect::InitDepth()//必须先InitKinect()
{
	hResult = pSensor->get_DepthFrameSource( &pDepthSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_DepthFrameSource()"<<endl;
		return -1;
	}
	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
		return -1;
	}
}
HRESULT Kinect::InitCoorMap()
{
	hResult = pSensor->get_CoordinateMapper( &pCoordinateMapper );     
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_CoordinateMapper()"<<endl;
		return -1;
	}
}
HRESULT Kinect::Initbody()
{

	hResult = pSensor->get_BodyFrameSource( &pBodySource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_BodyFrameSource()" << std::endl;
		return -1;
	}
	hResult = pBodySource->OpenReader( &pBodyReader ); 
	if( FAILED( hResult ) ){
		std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return -1;
	}
}

void Kinect::depthProcess()//16位显示会报错 不用该函数处理
{
	unsigned int depthBufferSize = DepthWidth * DepthHeight * sizeof( unsigned short );
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<UINT16**>(&depthMat16.data));
		if ( SUCCEEDED(hResult) )
		{
			depthMat16.convertTo(depthMat8, CV_8U, -255.0f/8000.0f, 255.0f );
		}

	}
	SafeRelease( pDepthFrame );  //这个frame一定要释放，不然无法更新
}
void Kinect::depthProcess2()
{
	UINT16 *pBuffer = NULL;
	UINT nBufferSize = 0; 
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		for(int i=0;i<DepthHeight;i++)
		{
			ushort* ptrDepth16 = depthMat16.ptr<ushort>(i);  // 16
			uchar* ptrDepth8 = depthMat8.ptr<uchar>(i);  //8
			for(int j=0;j<DepthWidth;j++)
			{
				USHORT depth = *pBuffer;  //16   以两个字节为单位进行读取
				ptrDepth16[j] = depth;        //直接存储显示
				ptrDepth8[j] = depth%256; //转为8位存储显示
				pBuffer++;               //16   以两个字节为单位进行读取
			}
		}
	}
	SafeRelease( pDepthFrame );  //这个frame一定要释放，不然无法更新
}
void Kinect::colorProcess()
{
	unsigned int colorBufferSize = ColorWidth * ColorHeight * 4 * sizeof( unsigned char );
	hResult = pColorReader->AcquireLatestFrame( &pColorFrame );      
	if ( SUCCEEDED(hResult) )
	{
		hResult = pColorFrame->CopyConvertedFrameDataToArray( colorBufferSize,
			reinterpret_cast<BYTE*> (colorMat.data), ColorImageFormat::ColorImageFormat_Bgra );
		if ( SUCCEEDED(hResult) )
		{
			resize( colorMat,colorHalfSizeMat , Size(), 0.5, 0.5 );
		}
	}
	SafeRelease( pColorFrame );  //这个frame一定要释放，不然无法更新
}
void Kinect::kincetSave(Mat a,string savePath,int opt)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	char optTemp[2];
	sprintf_s(optTemp,sizeof(optTemp),"%d",opt);
	string savePathTemp=savePath;
	switch (opt)
	{
	case saveDepth:
		{
			if( GetKeyState( VK_NUMPAD1 ) < 0 ){
				sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%03d",saveTmpIntDep); 
				savePathTemp = savePathTemp +  optTemp +"_"+ saveTmpStrDep + ".png";
				imwrite(savePathTemp,a);//存储图像
				saveTmpIntDep++;   
				savePathTemp =savePath;
			}
			break;
		}
	case saveColor:
		{
			if( GetKeyState( VK_NUMPAD2 ) < 0 ){
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
				savePathTemp = savePathTemp + optTemp +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,a);//存储图像
				saveTmpIntClr++;   
				savePathTemp =savePath;
			}
			break;
		}
	}
}
void Kinect::kinectSaveAll(string savePath)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	string savePathTemp=savePath;
	if( GetKeyState( VK_RETURN) < 0 ){
				sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%03d",saveTmpIntDep); 
				savePathTemp = savePathTemp +"depth" +"_"+ saveTmpStrDep + ".png";
				imwrite(savePathTemp,depthDeTemp);//存储去噪的深度图像
				//imwrite(savePathTemp,depthMat8);//存储去噪的深度图像
				saveTmpIntDep++;   
				savePathTemp =savePath;
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
				savePathTemp = savePathTemp+"color" +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,colorHalfSizeMat);//存储彩色图像
				saveTmpIntClr++;   
				savePathTemp =savePath;
			}
}
void Kinect::bodyLocation()
{
	cv::Vec3b color[BODY_COUNT];      //#define BODY_COUNT=6
	color[0] = cv::Vec3b( 255,   0,   0 );
	color[1] = cv::Vec3b(   0, 255,   0 );
	color[2] = cv::Vec3b(   0,   0, 255 );
	color[3] = cv::Vec3b( 255, 255,   0 );
	color[4] = cv::Vec3b( 255,   0, 255 );
	color[5] = cv::Vec3b(   0, 255, 255 );

	pBodyFrame = nullptr;
    showOrbit.create(500,500,CV_8UC4);
	hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
	if( SUCCEEDED( hResult ) )
	{
		IBody* pBody[BODY_COUNT] = { 0 }; //默认的是 6 个骨骼 ，初始化所有的骨骼信息
		hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );//更新骨骼数据，
		if( SUCCEEDED( hResult ) ){
			for( int count = 0; count < BODY_COUNT; count++ ){  //count数从0到5，6个人找骨骼来显示
				BOOLEAN bTracked = false;    //初始化“能追踪到人体”的值为否。
				hResult = pBody[count]->get_IsTracked( &bTracked );  //确认能着追踪到人体。
				
				
				if( SUCCEEDED( hResult ) && bTracked ){
					Joint joint[JointType::JointType_Count];   //取得人体Joint(关节)。JointType是一个枚举类型，不同位置的关节点都是不同的标号表示的。count是一个数值25。
					hResult = pBody[ count ]->GetJoints( JointType::JointType_Count, joint );  //取得人体Joint(关节)。
					//cout<<"索引为"<<count<<"的坐标： "<<"X="<<joint[ JointType_SpineMid].Position.X<<" "<<"Y="<<joint[ JointType_SpineMid].Position.Y<<" "<<"Z="<<joint[ JointType_SpineMid].Position.Z<<endl;
					
					//坐标变换
					float xx,yy,zz;
					xx=joint[ JointType_SpineMid].Position.X;
					yy=(cos(-27.0/180*PI)*joint[ JointType_SpineMid].Position.Y+sin(-27.0/180*PI)*joint[JointType_SpineMid].Position.Z);
					zz=((-1)*sin(-27.0/180*PI)*joint[ JointType_SpineMid].Position.Y+cos(-27.0/180*PI)*joint[ JointType_SpineMid].Position.Z);
					cout<<"x="<<xx<<"z="<<zz<<endl;
					outfile<<count<<","<<xx<<","<<yy<<","<<zz<<endl;
					//坐标显示部分
					float a,b;
					//a=joint[ JointType_SpineMid].Position.X*100;
					//b=joint[ JointType_SpineMid].Position.Z*100;
					a=xx*100;
					b=zz*100;
					Point center;
					center.x=250-int(a);
					center.y=500-int(b);
					circle(showOrbit,center,3,static_cast< cv::Scalar >( color[count] ),-1,CV_AA);
					}
			}
		}

		for( int count = 0; count < BODY_COUNT; count++ ){   //循环安全释放6个名为body的指针
				SafeRelease( pBody[count] );
			}
	}
	SafeRelease( pBodyFrame );  //这个frame一定要释放，不然无法更新	
}
