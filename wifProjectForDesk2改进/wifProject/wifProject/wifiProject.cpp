#include"myKinect.h"
#include"wifi.h"
#include <ctime> //����ʱ�亯��
#include<sstream> //��������ת��
#include <fstream>
#define peopleDisThd 2500 //�������˵������ֵ ����ruler������
#define peopleTimThd 10  //ɸѡʱ����ֵ
#define screenFilePath "E://projectTest//"

/*/////////////////////////////////////////////////////////////////////////
����ʹ�ò��裺
	1.�Ƚ����Ե�IP���ú� sokit����
	2.����rulerͳ������
	3.����ֵ�;����������ú�
//////////////////////////////////////////////////////////////////////////
	��Ҫ������ ͳ���ֻ������������
	ͳ��ÿ����Ա������ʱ��
/////////////////////////////////////////////////////////////////////////*/
string screeenShot(Mat &src, time_t t)//��ʱ���������ļ���
{
	string savePathTemp=screenFilePath;
	stringstream sstr2;
	sstr2<<t;
	string str2;
	sstr2>>str2;
	savePathTemp=savePathTemp+"quan//"+str2+ ".png";
	imwrite(savePathTemp,src);
	savePathTemp=string(screenFilePath)+"ding//"+str2+ ".png";
	imwrite(savePathTemp,src);
	return savePathTemp;
}

int main()
{
	time_t t1, t2, tTemp;
	Kinect kinect;
	Wifi wifi;
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
	ofstream outfile,outfileDing,outfileRaw,outfileDingRaw;
	while(1){
		kinect.colorProcess();
		//�������Ҫ�������ж���
		if((kinect.detPeopleDepth3())<peopleDisThd)//������������Ĳ��� ����������Ӧ����ֵ��ͻȻ�ļ�С ����һ�����ҵı仯����ʱ��ʼ��ʱ�������д���
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
			    connectImageStr=screeenShot(kinect.colorHalfSizeMat,t1);//����ͼƬͬʱ��·������	
				outfile.open(string(screenFilePath)+"quan//"+str2+".txt",ios::app); //���½���һ����
				outfileRaw.open(string(screenFilePath)+"quan//"+str2+"_raw"+".txt",ios::app); //���½���һ����
				outfileDing.open(string(screenFilePath)+"ding//"+str2+".txt",ios::app); //���½���һ����
				outfileDingRaw.open(string(screenFilePath)+"ding//"+str2+"_raw"+".txt",ios::app); //���½���һ����
			}
			//wifiҪ��ʼ��¼����
			wifi.wifiProcess(); 
		}
		else//�����뿪������Ĳ���
		{
			screenFlag=true;//���½���־��Ϊtrue
		    time(&t2);//��ȡ���ڵ�ʱ��
			if((t1!=tTemp)&&((t2-t1)<peopleTimThd))//�����˽�������ʱ��϶̵Ĳ���
			{
				tTemp=t1;
				remove(connectImageStr.c_str());//c_str charת����str
				remove(string(string(screenFilePath)+"quan//"+str2+".png").c_str());
				wifi.reSelMacRssi();//����������
				outfile.close();//�ر��ļ�
				outfileRaw.close();//�ر��ļ�
				outfileDing.close();//�ر��ļ�
				outfileDingRaw.close();//�ر��ļ�
				remove(string(string(screenFilePath)+"quan//"+str2+".txt").c_str());//ɾ���ļ�
				remove(string(string(screenFilePath)+"quan//"+str2+"_raw"+".txt").c_str());//ɾ���ļ�
				remove(string(string(screenFilePath)+"ding//"+str2+".txt").c_str());//ɾ���ļ�
				remove(string(string(screenFilePath)+"ding//"+str2+"_raw"+".txt").c_str());//ɾ���ļ�
			}
			else//��ʱ���㹻��ʱ�����ɹ����Ĳ���
			{
				if(wifiFlag==true)//ֻ����һ��
				{
					vector<Wifi::mapUsed> outdata,outdataDing;
					wifi.wifiProcessed2(wifi.selMap,outdata,outfileRaw);//�����ݽ��д�������,ͬʱ��ԭʼ���ݱ�����
					wifi.wifiProcessed2(wifi.selMapDing,outdataDing,outfileDingRaw);//�����ݽ��д�������,ͬʱ��ԭʼ���ݱ�����
					for (int it=0;it<outdata.size();it++) //������õ���Ϣ���
					{
						if (outdata[it].avgRssi!=0)//�޳�0�����
						{
							outfile<<outdata[it].macName<<" "<<outdata[it].avgRssi<<" "<<outdata[it].num<<endl;
						}
					}
					for (int it=0;it<outdataDing.size();it++) //������õ���Ϣ���
					{
						if (outdataDing[it].avgRssi!=0)//�޳�0�����
						{
							outfileDing<<outdataDing[it].macName<<" "<<outdataDing[it].avgRssi<<" "<<outdataDing[it].num<<endl;
						}
					}
					wifi.reSelMacRssi();//����������
					outfile.close();//�ر��ļ�
					outfileDing.close();
				}
				wifiFlag=false;
			}		
		}
		if(waitKey(3)==27)
		{
			break;
		}
	}
	exit(0);
}
