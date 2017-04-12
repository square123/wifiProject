#include "myKinect.h"
//Ö÷³ÌÐò
int main(int argc, char** argv )
{
	Kinect kinect;  
	kinect.outfile.open("data.csv",fstream::app);
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
