#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include <vector>
#include <set>
#include <time.h>

using namespace cv;

//在完善好程序后，将这些函数最后都封装到一个类里面
//class overlapPro
//{
//
//};

struct bwMix //定义连通区域的结构体
{
	Rect bwRect;//矩形区域
	int markLabel;//标记
	Mat bwEdge;//边缘信息
	Point center;//图像的重心
	time_t bwTime;//探针收集到的时刻
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
			temp.bwTime=time(NULL);//获取时间
			temp.markLabel=i;
			temp.bwEdge=contours[i];
			temp.bwRect=boundingRect(contours[i]);
			//添加一个构建重心的函数
			Moments m=moments(contours[i],false);
			temp.center.x=m.m10/m.m00;
			temp.center.y=m.m01/m.m00;
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
	if (src.size()>=1&&src.size()<11)//添加颜色的边界值，不要让值太多，当数量超过时不显示图像
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
	if (nowBox.area()!=0)
	{
		return float(rect.area())/float(nowBox.area());
	}else
	{
		return 0.0;
	}
}


//用旧的矩阵图像标号，更新新的标号的函数 让连续的帧overlap更新，通过结构体向量来传递图像的信息,当没有标号时，想办法去赋值，不能超过最大的数量先假定是10个
//这里先不考虑中断的情况，如果存在中断的情况，应该要有一个缓冲的感觉，比如多次取平均，感觉构造一个表比较好，这样思路比较清晰
void renewVec(vector<bwMix> &src, vector<bwMix> &dst,float thd)
{
	//构建一个表 n*m大小
	int m=src.size();
	int n=dst.size();
	set<int> S;//通过设置s筛选数据
	//构建一维数组，替代二维数组
	Mat table(n,m,CV_32FC1,0.0);//在openCV下使用mat作为2维数组更方便
	for (int i = 0; i < n; i++)
	{
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++)
		{
			 data[j]=rectOverlap(src[j].bwRect,dst[i].bwRect);
			 S.insert(src[j].markLabel);//剔除重复的元素

		}
	}
	//通过表进行查询，完成标号的更新 其中行号n对应的是要更新的标号，m是可以筛选的标号
	for (int i = 0; i < n; i++)//开始更新
	{
		float maxNum=thd;
		int maxIndex=-1;
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++) //找出每个列表中，最大的值
		{
			if (maxNum<data[j])
			{
				maxNum=data[j];
				maxIndex=j;
			}
		}
		//更新存在的标号 要考虑新加入和后来没有的索引
		if (maxIndex!=-1)
		{
			dst[i].markLabel=src[maxIndex].markLabel;
		}else//添加新的标号
		{
			for (int k=0;k<30;k++)//只添加0-19内的元素
			{
				if(S.find(k)==S.end())//添加不存在的标号，当其标号不存在时
				{
					dst[i].markLabel=k;
					S.insert(k);
					break;
				}
			}
		}
	}
}

int charTimeGetSecond(char ttt[14])//获得得到数据的后两位
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}
//轨迹计算公式 根据相邻的重心来输出轨迹 需要考虑到中断的问题，因为轨迹会出现中断，当中断时要输出轨迹，以后再考虑,在轨迹中应该加入滤波的过程，因为图像在运动过程中图像会发生变化
//轨迹可能会出现遗漏的场景，现在就不需要加入判断
//还应该加入时间戳
//加入一个取平均的过程，需要进行处理
void bwMixToOrbit(vector<bwMix> &src) 
{ 
	for (int i = 0; i < src.size(); i++)
	{
		char timeFix[16];
		strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&src[i].bwTime));//时间修复
		cout<<timeFix<<","<<src[i].markLabel<<","<<src[i].center<<endl;
	}
}

//卡尔曼滤波基本算法（可能暂时还不需要）//应该只需要把前面的作为缓冲
void kalmanfilter()
{

}

//轨迹变换矩阵（要根据实地场合去计算）暂时先不考虑
void pointTransform(Point &src,Point &dst)
{

}

//光流法用
void compute_absolute_mat(const Mat& in, Mat & out)  
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