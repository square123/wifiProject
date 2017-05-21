#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include <vector>

using namespace cv;

struct bwMix //定义连通区域的结构体
{
	Rect bwRect;//矩形区域
	int markLabel;//标记
	Mat bwEdge;//边缘信息
	Point center;//图像的重心
};

void fillHole(const Mat srcBw, Mat &dstBw)//参考网上的填充函数，很巧妙
{
	Size m_Size = srcBw.size();
	Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//延展图像
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));//非常巧妙，用了填充算法的反运算

	Mat cutImg;//裁剪延展的图像
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

void  bwLabel( const Mat &bwImg, Mat &labImg )  //参考网上的连通区域标记函数
{  
	assert( bwImg.type()==CV_8UC1 );  
	labImg.create( bwImg.size(), CV_32SC1 ); 
	labImg = Scalar(0);  
	labImg.setTo( Scalar(1), bwImg );  
	assert( labImg.isContinuous() );  
	const int Rows = bwImg.rows - 1, Cols = bwImg.cols - 1;  
	int label = 1;   
	vector<int> labelSet;  
	labelSet.push_back(0);    
	labelSet.push_back(1);    
	// the first pass  
	int *data_prev = (int*)labImg.data; // 0-th row : int* data_prev = labImg.ptr<int>(i-1);  
	int *data_cur = (int*)( labImg.data + labImg.step ); // 1-st row : int* data_cur = labImg.ptr<int>(i);  
	for( int i=1; i<Rows; i++ ){  
		data_cur++;  
		data_prev++;  
		for( int j=1; j<Cols; j++, data_cur++, data_prev++ ){  
			if( *data_cur!=1 )  
				continue;  
			int left = *(data_cur-1);  
			int up = *data_prev;  
			int neighborLabels[2];  
			int cnt = 0;  
			if( left>1 )  
				neighborLabels[cnt++] = left;  
			if( up>1 )  
				neighborLabels[cnt++] = up;  
			if( !cnt ){  
				labelSet.push_back( ++label );  
				labelSet[label] = label;  
				*data_cur = label;  
				continue;  
			}  
			int smallestLabel = neighborLabels[0];  
			if( cnt==2 && neighborLabels[1]<smallestLabel )  
				smallestLabel = neighborLabels[1];  
			*data_cur = smallestLabel;  
			// 保存最小等价表  
			for( int k=0; k<cnt; k++ ){  
				int tempLabel = neighborLabels[k];  
				int& oldSmallestLabel = labelSet[tempLabel];  
				if( oldSmallestLabel > smallestLabel ){                            
					labelSet[oldSmallestLabel] = smallestLabel;  
					oldSmallestLabel = smallestLabel;  
				}                         
				else if( oldSmallestLabel<smallestLabel )  
					labelSet[smallestLabel] = oldSmallestLabel;  
			}  
		}  
		data_cur++;  
		data_prev++;  
	}  
	// 更新等价对列表,将最小标号给重复区域  
	for( size_t i = 2; i < labelSet.size(); i++ ){  
		int curLabel = labelSet[i];  
		int preLabel = labelSet[curLabel];  
		while( preLabel!=curLabel ){  
			curLabel = preLabel;  
			preLabel = labelSet[preLabel];  
		}  
		labelSet[i] = curLabel;  
	}  
	// second pass  
	data_cur = (int*)labImg.data;  
	for( int i=0; i<Rows; i++ ){  
		for( int j=0; j<bwImg.cols-1; j++, data_cur++ )  
			*data_cur = labelSet[ *data_cur ];    
		data_cur++;  
	}  
}

void bwLabelNext(Mat &src)//作为上述连通区域标记的后续处理
{
	int a[500]={0};//设置连通区域的个数最多为500个
	for(int i=0;i<src.rows;i++)
	{
		int* data= src.ptr<int>(i);
		for(int j=0;j<src.cols;j++)
		{
			if(data[j]!=0){//寻找元素的种类
				for(int k=0;k<500;k++)
				{
					if (data[j]==a[k])
					{
						data[j]=k+1;//这里简化运算直接重新赋值，一次性循环处理完成
						break;
					}
					else if(a[k]==0)
					{
						a[k]=data[j];
						data[j]=k+1;
						break; 
					}
				}
			}
		}
	}
}


void selectArea(Mat &src,Mat &dst, int minNum,int maxNum)//选择合适范围的连通区域
{
	vector<vector<Point>> contours;  
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()<1)  //去除边界值
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=src.clone();
		for (int i = 0; i < contours.size(); i++)
		{
			if (maxNum>0?(contourArea(contours[i])>maxNum||contourArea(contours[i])<minNum):(contourArea(contours[i])<minNum))
			{
				drawContours(dst,contours,i,0,CV_FILLED);
			} 
			else
			{
				drawContours(dst,contours,i,255,CV_FILLED);
			}
		}
	}
}

void hullArea(Mat &src,Mat &dst) //用来将连通区域用凸包合并
{
	vector<vector<Point>> contours;  //通过该方法也是可以的，其矩阵充当了原来类似vector<Point>的效果，是双通道矩阵
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	vector<vector<Point>> hull(contours.size());  
	for (unsigned int i=0;i<contours.size();i++)
	{
		convexHull(Mat(contours[i]),hull[i],false);
	}
	if(contours.size()<1)  //去除边界值
	{
		dst=Mat::zeros(src.size(),CV_8UC1);
	}else
	{
		dst=src.clone();
		for (int i = 0; i < contours.size(); i++)
		{
			drawContours(dst,hull,i,(255),CV_FILLED);
		}
	}
}

//构建结构体的函数
void getBwMix(Mat &src,vector<bwMix> &dst)
{
	vector<Mat> contours; //这个地方好好思考下
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()>=1)  //去除边界值
	{
		for (int i = 0; i < contours.size(); i++)
		{
			bwMix temp;
			temp.markLabel=i;
			temp.bwEdge=contours[i];
			temp.bwRect=boundingRect(contours[i]);
			//添加一个构建重心的函数
			Moments m=moments(contours[i],false);
			temp.center.x=m.m10/m.m00;
			temp.center.y=m.m01/m.m00;
			cout<<temp.center<<endl;
			dst.push_back(temp);
		}
	}
}

//根据结构体转换成彩色矩阵//颜色处理函数 进行列表化，控制最多的检测人数，不考虑人会出现割裂的痕迹 定义最多的检测的人数为10人，也是通过结构体来传递图像信息
void bwMixToColorMat(vector<bwMix> &src,Mat &dst)
{
	Scalar globalColor[10]; //构建颜色表
	globalColor[0] = Scalar( 67, 128, 6 );
	globalColor[1] = Scalar( 84, 80, 153 );
	globalColor[2] = Scalar( 57, 60, 79 );
	globalColor[3] = Scalar( 255, 255, 0 );
	globalColor[4] = Scalar( 61, 63, 17 );
	globalColor[5] = Scalar( 155, 175, 131 );
	globalColor[6] = Scalar( 101, 67, 254 );
	globalColor[7] = Scalar( 154, 157, 252 );
	globalColor[8] = Scalar( 173, 205, 249 );
	globalColor[9] = Scalar( 169, 200, 200 );
	//dst=Mat::zeros(dstrows,dstcols,CV_8UC3);
	if (src.size()>=1&&src.size()<11)
	{
		for (int i=0;i<src.size();i++)
		{
			vector<Mat> tempMat;
			tempMat.push_back(src[i].bwEdge);//做了转存，将变量变成了一个整体
			drawContours(dst,tempMat,0,globalColor[src[i].markLabel],CV_FILLED,8);//将对应的标签的颜色填充
		}
	}
}

//矩形区域比较函数
float rectOverlap(const Rect & preBox,const Rect & nowBox) //直接使用Rect函数 好像暂时没啥用
{ 
	Rect rect=preBox & nowBox;//取两个矩阵的交集
	if (preBox.area()!=0)
	{
		return rect.area()/preBox.area();
	}else
	{
		return 0.0;
	}
}


//用旧的矩阵图像标号，更新新的标号的函数 让连续的帧overlap更新，通过结构体向量来传递图像的信息,当没有标号时，想办法去赋值，不能超过最大的数量先假定是10个
//这里先不考虑中断的情况，如果存在中断的情况，应该要有一个缓冲的感觉，比如多次取平均，感觉构造一个表比较好，这样思路比较清晰
void renewVec(vector<bwMix> &src, vector<bwMix> &dst)
{
	//构建一个表
	int m=src.size();
	int n=dst.size();
	//构建一维数组，替代二维数组

	cv::Mat table(m,n,CV_32FC1,0.0);


}


//轨迹计算公式 根据相邻的重心来输出轨迹 需要考虑到中断的问题，因为轨迹会出现中断，当中断时要输出轨迹，以后再考虑,在轨迹中应该加入滤波的过程，因为图像在运动过程中图像会发生变化


void compute_absolute_mat(const Mat& in, Mat & out)  //光流法用
{  
	if (out.empty()){  
		out.create(in.size(), CV_32FC1);  
	}  
	const Mat_<Vec2f> _in = in;  
	for (int i = 0; i < in.rows; ++i){  
		float *data = out.ptr<float>(i);  
		for (int j = 0; j < in.cols; ++j){  
			double s = _in(i, j)[0] * _in(i, j)[0] + _in(i, j)[1] * _in(i, j)[1];  
			if (s>1){  
				data[j] = std::sqrt(s);  
			}  
			else{  
				data[j] = 0.0;  
			}  

		}  
	}  
}  