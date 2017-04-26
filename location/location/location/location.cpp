#include "WifiProbe.h"
#include "myKinect.h"
//������
int main(int argc, char** argv )
{
	Kinect kinect;  
	Probe probe;
	kinect.outfile.open("kinectdata.csv",fstream::app);//��ʵ��һ���ܲ����˼·
	probe.InitProbe();
	kinect.InitKinect();
	kinect.Initbody();
	while(1){
		probe.probeProcess();
		kinect.bodyLocation();
		imshow("haha",kinect.showOrbit);
		if(cv::waitKey(3)==27)
		{
			break;
		}
	}
	kinect.~Kinect();
	probe.~Probe();
}
