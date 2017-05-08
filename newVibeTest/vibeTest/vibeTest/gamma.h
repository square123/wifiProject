#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

void gammaCorrection (Mat& src, float fGamma)  //伽马校正 参考网上的程序
{  
    CV_Assert(src.data);  
    CV_Assert(src.depth() != sizeof(uchar));  
    // build look up table  
    unsigned char lut[256];  
    for( int i = 0; i < 256; i++ )  
    {  
        lut[i] = saturate_cast<uchar>(pow((float)(i/255.0), fGamma) * 255.0f);  
    }  
    Mat dst = src.clone();  
    const int channels = dst.channels();  
    switch(channels)  
    {  
        case 1:  
            {  
                MatIterator_<uchar> it, end;  
                for( it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++ )  
                    *it = lut[(*it)];  
                break;  
            }  
        case 3:   
            {  
                MatIterator_<Vec3b> it, end;  
                for( it = dst.begin<Vec3b>(), end = dst.end<Vec3b>(); it != end; it++ )  
                {  
                    (*it)[0] = lut[((*it)[0])];  
                    (*it)[1] = lut[((*it)[1])];  
                    (*it)[2] = lut[((*it)[2])];  
                }  
                break;  
            }  
    }  
	src=dst.clone();
}  