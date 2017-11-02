//#include<winsock2.h>
#pragma once

#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//�����Ǹ�ʽ��16���Ƶ�����ļ�
//#include<time.h>//����ʱ��
#include <map>
#include <set>
#include <numeric>
#include <algorithm>
#include <vector>
#include <fstream>//ϣ������ʱ��
#pragma comment(lib,"WS2_32.lib")
using namespace std;

//�Ժ���Ե�����
#define RSSITHD -50   //wifi̽��̽�����ֵ
#define SERVER_PORT 2222 //UDP�󶨶˿ں�

#define BUFFER_SIZE 1024 
#define REC_RSSI_COUNT 10 //ϵͳĬ�ϴ洢rssi��������� ���ݾ������������

//��װ��̽���������͸�ʽ�����߽����е����ݸ�ʽ����װ�ɺ�������Ƶĸ�ʽ
class mncatsWifi 
{
public :
	std::string initData;//�����洢charת������ַ���
	std::string mac1;//�洢̽���mac�� 0-11
	std::string mac2;//�洢̽���mac�� 13-24
	std::string rssi;//�洢Rssiֵ 26-28
	std::string dtype;//�ź����� 30-31 //�Ժ��ٷ���
	std::string wifidate;//���� 33-42
	std::string wifitime;//ʱ�� 44-51
	std::string timeStamp;//�ۺϵ��ַ���
	unsigned char cmac2[6];//���mac2�������
	char crssi;//char��ʽ��rssi
	char Timestamp[14];//�ӿڵ�ʱ���ʽ

	mncatsWifi(std::string &s):mac1(s.substr(0,12)),mac2(s.substr(13,12)),rssi(s.substr(26,3)),dtype(s.substr(30,2)),\
		wifidate(s.substr(33,10)),wifitime(s.substr(44,8)),timeStamp(0)
	{
		wifiReform(*this);
		crssi=atoi(rssi.c_str());
		memcpy(Timestamp,timeStamp.c_str(),sizeof(unsigned char)*14);
		for (int i = 0; i < 6; i++)
			cmac2[i]=strtol(mac2.substr(i*3,2).c_str(),NULL,16);
	}

	mncatsWifi(char ss[]):initData(ss),mac1(initData.substr(0,12)),mac2(initData.substr(13,12)),\
		rssi(initData.substr(26,3)),dtype(initData.substr(30,2)),wifidate(initData.substr(33,10)),\
		wifitime(initData.substr(44,8)){
			wifiReform(*this);
			crssi=atoi(rssi.c_str());
			memcpy(Timestamp,timeStamp.c_str(),sizeof(unsigned char)*14);
			for (int i = 0; i < 6; i++)
				cmac2[i]=strtol(mac2.substr(i*3,2).c_str(),NULL,16);	
	}

private:
	mncatsWifi& wifiReform(mncatsWifi & temp1)
	{
		for (auto &i:temp1.mac2)
		{
			i=toupper(i);
		}
		temp1.mac1=temp1.mac1.substr(0,2)+":"+temp1.mac1.substr(2,2)+":"+temp1.mac1.substr(4,2)+":"+temp1.mac1.substr(6,2)+":"+temp1.mac1.substr(8,2)+":"+temp1.mac1.substr(10,2);
		temp1.mac2=temp1.mac2.substr(0,2)+":"+temp1.mac2.substr(2,2)+":"+temp1.mac2.substr(4,2)+":"+temp1.mac2.substr(6,2)+":"+temp1.mac2.substr(8,2)+":"+temp1.mac2.substr(10,2);
		temp1.timeStamp=temp1.wifidate.substr(0,4)+temp1.wifidate.substr(5,2)+temp1.wifidate.substr(8,2)+temp1.wifitime.substr(0,2)+\
			temp1.wifitime.substr(3,2)+temp1.wifitime.substr(6,2);
		return temp1;
	}
};

//��Ҫ������̽�����޸ģ�֮ǰд�Ļ��ǲ����ã�Ӧ��Ҫ����һЩ��ϵ����أ���������صĸ�ʽ��ά��һ��
class Wifi
{
public:

	struct selMacRssi//����ýṹ��������ͳ�Ƴ���RSSIֵ�����������ʹ�ñ䳤�Ĵ洢�����ڶ�ʱ���ڲ�����ֺܶ�Mac��ַС��ĳ��ֵ����������ֻ����10��ֵ
	{
		unsigned char selMumac[6];      //�����ź�MU��MAC��ַ
		int sumRssi;                    //����RSSI���ܺ�
		char maxRssi;                   //����RSSI�����ֵ
		int num;						//ͳ�Ƹ�RSSI���ֵĴ���
	}sel[REC_RSSI_COUNT];//����Ҫ�洢�Ľṹ������
	set<string> denyList;//������Ҫ�޳���MAC��ַ �Ժ����ͨ�����������
	map<string,vector<int>> selMapDing;//�µĴ洢��ʽ   //������ݿ�������Ժ�ֻ�洢����ܵ�ǰ5�������������ڱ��� //���д�Ĳ�̫��
	struct mapUsed  //������ʱҪʹ�õĸ�ʽ
	{
		string macName;
		int num;
		int avgRssi;
		double score; //����Ҫ����ķ���
	};
	unsigned char zeroMac[6];
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	FILE *fp;
	sockaddr_in servAddr;//��������ַ
	sockaddr_in clientAddr; 
	Wifi();
	~Wifi();
	void InitWifi();
	void wifiProcess();
	string wifiProcessed();
	void  wifiProcessed2(map<string,vector<int>> &ss,vector<mapUsed> &,ofstream &raw,ofstream &rawPro,string srcTime);//�µķ��� ��ɵ������������
	void reSelMacRssi();
	string charTo02XStr(unsigned char input);
	string macToString(unsigned char Mymac[6]);//��ɽ�char����ת�����ַ���
	char MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
};

