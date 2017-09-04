#include "WifiProbe.h"
#include "myKinect.h"
//主程序

DWORD WINAPI MyThreadProc1(LPVOID lpParameter);
DWORD WINAPI MyThreadProc2(LPVOID lpParameter);

int main(int argc, char** argv )
{
	HANDLE handle1,handle2;
	handle1 = CreateThread(NULL,0,MyThreadProc1,NULL,0,NULL);
	handle2 = CreateThread(NULL,0,MyThreadProc2,NULL,0,NULL);
	WaitForSingleObject(handle1,INFINITE);//加了这句就能同时运行了。
	WaitForSingleObject(handle2,INFINITE);
	CloseHandle(handle1);
	CloseHandle(handle2);
	return 0;
}

DWORD WINAPI MyThreadProc1(LPVOID lpParameter)
{
	Kinect kinect;  
	kinect.outfile.open("kinectdata.csv",fstream::app);//其实是一个很不错的思路
	kinect.InitKinect();
	kinect.InitColor();
	kinect.Initbody();
	kinect.InitCoorMap();
	kinect.InitBodyIndex();
	while(1){
		kinect.colorProcess();
		kinect.bodyIndexProcess();
		kinect.CoordinateMapperProcess();

		kinect.bodyLocation();
		imshow("xx",kinect.bodyIndexMat);
		//imshow("haha",kinect.showOrbit);
		imshow("h",kinect.CoordinateMapperMat);
		if(cv::waitKey(3)==27)
		{
			break;
		}
	}
	return 0;
}

DWORD WINAPI MyThreadProc2(LPVOID lpParameter)
{
	Probe probe;
	probe.InitProbe();
	while(1){
		probe.probeProcess();
		if(cv::waitKey(3)==27)
		{
			break;
		}
	}
	return 0;
}
