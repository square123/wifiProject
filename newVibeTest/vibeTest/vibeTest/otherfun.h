#pragma once
#include <iostream>
#include "opencv2/opencv.hpp"
#include <vector>
#include <set>
#include <time.h>

using namespace cv;

//�����ƺó���󣬽���Щ������󶼷�װ��һ��������
//class overlapPro
//{
//
//};

struct bwMix //������ͨ����Ľṹ��
{
	Rect bwRect;//��������
	int markLabel;//���
	Mat bwEdge;//��Ե��Ϣ
	Point center;//ͼ�������
	time_t bwTime;//̽���ռ�����ʱ��
};

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

void hullArea(Mat &src,Mat &dst) //��������ͨ������͹���ϲ�
{
	vector<vector<Point>> contours;  //ͨ���÷���Ҳ�ǿ��Եģ������䵱��ԭ������vector<Point>��Ч������˫ͨ������
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	vector<vector<Point>> hull(contours.size());  
	for (unsigned int i=0;i<contours.size();i++)
	{
		convexHull(Mat(contours[i]),hull[i],false);
	}
	if(contours.size()<1)  //ȥ���߽�ֵ
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

//�����ṹ��ĺ���
void getBwMix(Mat &src,vector<bwMix> &dst)
{
	vector<Mat> contours; //����ط��ú�˼����
	findContours(src,contours,RETR_EXTERNAL,CHAIN_APPROX_NONE); 
	if(contours.size()>=1)  //ȥ���߽�ֵ
	{
		for (int i = 0; i < contours.size(); i++)
		{
			bwMix temp;
			temp.bwTime=time(NULL);//��ȡʱ��
			temp.markLabel=i;
			temp.bwEdge=contours[i];
			temp.bwRect=boundingRect(contours[i]);
			//���һ���������ĵĺ���
			Moments m=moments(contours[i],false);
			temp.center.x=m.m10/m.m00;
			temp.center.y=m.m01/m.m00;
			dst.push_back(temp);
		}
	}
}

//���ݽṹ��ת���ɲ�ɫ����//��ɫ������ �����б����������ļ���������������˻���ָ��ѵĺۼ� �������ļ�������Ϊ10�ˣ�Ҳ��ͨ���ṹ��������ͼ����Ϣ
void bwMixToColorMat(vector<bwMix> &src,Mat &dst)
{
	Scalar globalColor[10]; //������ɫ��
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
	if (src.size()>=1&&src.size()<11)//�����ɫ�ı߽�ֵ����Ҫ��ֵ̫�࣬����������ʱ����ʾͼ��
	{
		for (int i=0;i<src.size();i++)
		{
			vector<Mat> tempMat;
			tempMat.push_back(src[i].bwEdge);//����ת�棬�����������һ������
			drawContours(dst,tempMat,0,globalColor[src[i].markLabel],CV_FILLED,8);//����Ӧ�ı�ǩ����ɫ���
		}
	}
}

//��������ȽϺ���
float rectOverlap(const Rect & preBox,const Rect & nowBox) //ֱ��ʹ��Rect���� ������ʱûɶ��
{ 
	Rect rect=preBox & nowBox;//ȡ��������Ľ���
	if (nowBox.area()!=0)
	{
		return float(rect.area())/float(nowBox.area());
	}else
	{
		return 0.0;
	}
}


//�þɵľ���ͼ���ţ������µı�ŵĺ��� ��������֡overlap���£�ͨ���ṹ������������ͼ�����Ϣ,��û�б��ʱ����취ȥ��ֵ�����ܳ������������ȼٶ���10��
//�����Ȳ������жϵ��������������жϵ������Ӧ��Ҫ��һ������ĸо���������ȡƽ�����о�����һ����ȽϺã�����˼·�Ƚ�����
void renewVec(vector<bwMix> &src, vector<bwMix> &dst,float thd)
{
	//����һ���� n*m��С
	int m=src.size();
	int n=dst.size();
	set<int> S;//ͨ������sɸѡ����
	//����һά���飬�����ά����
	Mat table(n,m,CV_32FC1,0.0);//��openCV��ʹ��mat��Ϊ2ά���������
	for (int i = 0; i < n; i++)
	{
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++)
		{
			 data[j]=rectOverlap(src[j].bwRect,dst[i].bwRect);
			 S.insert(src[j].markLabel);//�޳��ظ���Ԫ��

		}
	}
	//ͨ������в�ѯ����ɱ�ŵĸ��� �����к�n��Ӧ����Ҫ���µı�ţ�m�ǿ���ɸѡ�ı��
	for (int i = 0; i < n; i++)//��ʼ����
	{
		float maxNum=thd;
		int maxIndex=-1;
		float* data=table.ptr<float>(i);
		for (int j = 0; j < m; j++) //�ҳ�ÿ���б��У�����ֵ
		{
			if (maxNum<data[j])
			{
				maxNum=data[j];
				maxIndex=j;
			}
		}
		//���´��ڵı�� Ҫ�����¼���ͺ���û�е�����
		if (maxIndex!=-1)
		{
			dst[i].markLabel=src[maxIndex].markLabel;
		}else//����µı��
		{
			for (int k=0;k<30;k++)//ֻ���0-19�ڵ�Ԫ��
			{
				if(S.find(k)==S.end())//��Ӳ����ڵı�ţ������Ų�����ʱ
				{
					dst[i].markLabel=k;
					S.insert(k);
					break;
				}
			}
		}
	}
}

int charTimeGetSecond(char ttt[14])//��õõ����ݵĺ���λ
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}
//�켣���㹫ʽ �������ڵ�����������켣 ��Ҫ���ǵ��жϵ����⣬��Ϊ�켣������жϣ����ж�ʱҪ����켣���Ժ��ٿ���,�ڹ켣��Ӧ�ü����˲��Ĺ��̣���Ϊͼ�����˶�������ͼ��ᷢ���仯
//�켣���ܻ������©�ĳ��������ھͲ���Ҫ�����ж�
//��Ӧ�ü���ʱ���
//����һ��ȡƽ���Ĺ��̣���Ҫ���д���
void bwMixToOrbit(vector<bwMix> &src) 
{ 
	for (int i = 0; i < src.size(); i++)
	{
		char timeFix[16];
		strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&src[i].bwTime));//ʱ���޸�
		cout<<timeFix<<","<<src[i].markLabel<<","<<src[i].center<<endl;
	}
}

//�������˲������㷨��������ʱ������Ҫ��//Ӧ��ֻ��Ҫ��ǰ�����Ϊ����
void kalmanfilter()
{

}

//�켣�任����Ҫ����ʵ�س���ȥ���㣩��ʱ�Ȳ�����
void pointTransform(Point &src,Point &dst)
{

}

//��������
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