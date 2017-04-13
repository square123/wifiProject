#include "WifiProbe.h"
#include "myKinect.h"
//������
int main(int argc, char** argv )
{
	Kinect kinect;  
	Probe probe;
	kinect.outfile.open("kinectdata.csv",fstream::app);//��ʵ��һ���ܲ����˼·
	//Mat depthDeNoise(kinect.DepthHeight, kinect.DepthWidth, CV_16UC1);
	probe.InitProbe();
	kinect.InitKinect();
	/*kinect.InitDepth();
	kinect.InitColor();*/
	//kinect.InitCoorMap();
	kinect.Initbody();
	while(1){
		//kinect.colorProcess();
		//kinect.depthProcess();
		probe.probeProcess();
		kinect.bodyLocation();
		imshow("haha",kinect.showOrbit);
		//imshow("depth",kinect.depthMat8);
		//imshow("color",kinect.colorHalfSizeMat);
		if(cv::waitKey(3)==27)
		{
			break;
		}
	}
	kinect.~Kinect();
	probe.~Probe();
}
