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
#include "MncatsWifi.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define SERVER_PORT 2222 //UDP�󶨶˿ں�
#define BUFFER_SIZE 1024 
#define RSSITHD -50   //wifi̽��̽�����ֵ
#define REC_RSSI_COUNT 10 //ϵͳĬ�ϴ洢rssi��������� ���ݾ������������

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
	};
	unsigned char zeroMac[6];
	WSADATA wsd;//WSADATA ����
	SOCKET s;//Ƕ����
	FILE *fp;
	sockaddr_in servAddr;//��������ַ
	//struct cliprobeData *packageData;
	sockaddr_in clientAddr; 
	Wifi();
	~Wifi();
	void InitWifi();
	void wifiProcess();
	string wifiProcessed();
	void  wifiProcessed2(map<string,vector<int>> &ss,vector<mapUsed> &,ofstream &raw,ofstream &rawPro);//�µķ��� ��ɵ������������
	void reSelMacRssi();
	string charTo02XStr(unsigned char input);
	string macToString(unsigned char Mymac[6]);//��ɽ�char����ת�����ַ���
	void getSpecialMac(unsigned char Mymac[6]);//ת���ַ�������ʽ�ĺ���
	void getSpecialRssi(char MyRssi);//ת���ַ�������ʽ�ĺ���
	char MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0

};

