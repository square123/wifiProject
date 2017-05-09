#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;

void fillHole(const Mat srcBw, Mat &dstBw)//�ο����ϵ���亯����������
{
	Size m_Size = srcBw.size();
	Mat Temp=Mat::zeros(m_Size.height+2,m_Size.width+2,srcBw.type());//��չͼ��
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));//�ǳ������������㷨�ķ�����

	Mat cutImg;//�ü���չ��ͼ��
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	dstBw = srcBw | (~cutImg);
}

void  bwLabel( const Mat &bwImg, Mat &labImg )  //�ο����ϵ���ͨ�����Ǻ���
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
			// ������С�ȼ۱�  
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
	// ���µȼ۶��б�,����С��Ÿ��ظ�����  
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

void bwLabelNext(Mat &src)//��Ϊ������ͨ�����ǵĺ�������
{
	int a[500]={0};//������ͨ����ĸ������Ϊ500��
	for(int i=0;i<src.rows;i++)
	{
		int* data= src.ptr<int>(i);
		for(int j=0;j<src.cols;j++)
		{
			if(data[j]!=0){//Ѱ��Ԫ�ص�����
				for(int k=0;k<500;k++)
				{
					if (data[j]==a[k])
					{
						data[j]=k+1;//���������ֱ�����¸�ֵ��һ����ѭ���������
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


void selectArea(Mat &src,Mat &dst, int minNum,int maxNum)//ѡ����ʷ�Χ����ͨ����
{
	vector<vector<Point>> contours;  
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()<1)  //ȥ���߽�ֵ
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

void compute_absolute_mat(const Mat& in, Mat & out)  //��������
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