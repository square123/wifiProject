#include"myKinect.h"
#include"wifi.h"
#include <ctime> //����ʱ�亯��
#include<sstream> //��������ת��
#define peopleDisThd 1400 //�������˵������ֵ�������ʵ�����
#define peopleTimThd 15  //ɸѡʱ����ֵ
#define screenFilePath "H://testbody//"
//����ƻ���
//1.�����ͨ��������ж���ʼ������									��ɣ�
//2.��ɽ�ͼ�Ĺ��ܣ���ֻ��һ�Σ���Ҫ������ʱ��϶̵����ݼ�¼      ��ɣ�
//3.���ɼ�mac�ĳ���д�ɺ���											��ɣ�
//4.��Mac�Ĳɼ���һ���Ż�������ʹ�ýṹ�壬���ƥ��					��ɣ�
//5.��������ȼ��Ŀ�ȷ�Χ��С��ֻ���С�����
//6.��߳�����ٶ�
//7.����ֱ����kinect�������
//8*.���ո��ݳ����Ч�����������װ�ɽ���

//ʹ�ùؽڵ���õķ�����¼���Ͼ�Ҳ���ǻ۽ᾧ��������¼
//float avgDet(float a[])//ȡ��ֵ����
//{
//	//int n=sizeof(a)/sizeof(a[0]);//���������Ӧ����ô�ã���Ϊ�˼��ֱ��ȥ������֪�����鳤��6
//	float sum=0;
//	for(int i=0;i<10;i++)
//	{
//		sum=sum+a[i];
//	}
//	return(sum/10.0);
//}
//float medDet(float a[])//ȡ��ֵ����
//{
//	int n=10;
//	for (int i=1; i<n; i++)
//    {
//        for (int j=0; j<n-1; j++)
//        {
//            if (a[j]>a[j+1])
//            {
//                swap(a[j],a[j+1]); 
//            }
//        }
//    }
//	return a[(n+1)/2];
//}

string screeenShot(Mat &src, time_t t)//��ʱ���������ļ���
{
	string savePathTemp=screenFilePath;
	stringstream sstr2;
	sstr2<<t;
	string str2;
	sstr2>>str2;
	savePathTemp=savePathTemp+ "scene_" +str2+ ".png";
	imwrite(savePathTemp,src);
	return savePathTemp;
}

//��������Ϊlength������ַ���  

//������
int main(int argc, char** argv )
{
	time_t t1, t2, tTemp;
	Kinect kinect;
	Wifi wifi;
	//���ùؽڵ�ķ��������ȶ����Բ���
	//float peopleDet;
	//float smoothDet[10]={8.0,8.0,8.0,8.0,8.0,8.0,8.0,8.0,8.0,8.0};//��Ҫ���������ĳ�ʼ��

	//kinect ��ʼ��
	kinect.InitKinect();
	kinect.InitColor();
	kinect.InitBodyIndex();
	kinect.InitDepth();
	//wifi��ʼ��
	wifi.InitWifi();
 	bool screenFlag=true;//���ձ�־λ
	bool wifiFlag=false;//wifi�����־λ

    //ʱ���ʼ��
	time(&t1);
	tTemp=t1;

	//�����������
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	string connectImageStr;
	string str2;
	while(1){
		kinect.colorProcess();
		//���ùؽڵ�ķ��������ȶ����Բ���
		//smoothDet[i%10]=kinect.detPeopleDepth();//ƽ�����ܳ��ֵ�������,���������������ʹ��,Ȼ�����ֲ����Ǻܺ���
		//imshow("aa",kinect.colorHalfSizeMat);
		//cout<<"ʵʱ�����ȣ�"<<smoothDet[i%10]<<endl;
		//cout<<"�˲�������"<<medDet(smoothDet)<<endl;//��ֵ�˲���ƽ���˲�Ч����
		//_sleep(100);
		//if(medDet(smoothDet)< peopleDetThd){
	
		if(kinect.detPeopleDepth2()< peopleDisThd)//������������Ĳ���
		{
			wifiFlag=true;
			if (screenFlag==true)
			{
				screenFlag=false;
				time(&t1);
				stringstream sstr2;
				sstr2<<t1;
				sstr2>>str2;
				//��ͼ���� ֻ����һ��
				//cout<<"������"<<endl;
				//cout<<t1<<endl;
			    connectImageStr=screeenShot(kinect.colorHalfSizeMat,t1);//����ͼƬͬʱ��·������				
			}
			//wifiҪ��ʼ��¼����
			wifi.wifiProcess();
		}
		else//�����뿪������Ĳ���
		{
			screenFlag=true;//���½���־��Ϊtrue
		    time(&t2);
			if((t1!=tTemp)&&((t2-t1)<peopleTimThd))//�����˽�������ʱ��϶̵Ĳ���
			{
				tTemp=t1;
				remove(connectImageStr.c_str());//c_str charת����str
				wifi.reSelMacRssi();//����������
			}
			else//��ʱ���㹻��ʱ�����ɹ����Ĳ���
			{
				if(wifiFlag==true)//ֻ����һ��
				{
					string macImageStr;
					//��wifi���ݽ��в���
					macImageStr=wifi.wifiProcessed();
					//���������������ص�ָ����������ʲô�����������ǽ���Ƭ������
					if(macImageStr!="0")
					{
						macImageStr=screenFilePath+str2+"_"+macImageStr+ ".png";
						rename(connectImageStr.c_str(),macImageStr.c_str());
					}	
					wifi.reSelMacRssi();//����������
				}
				wifiFlag=false;
			}		
		}

		//���ùؽڵ�ķ��������ȶ����Բ���
		//i++;//����
		//if(i==60000) i=0;//�������

		if(waitKey(3)==27)
		{
			break;
		}
	}
	wifi.~Wifi();
	kinect.~Kinect();
	exit(0);
}
