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
#include <fstream>//ϣ������ʱ��
#include "MncatsWifi.h"
#include "myCsv.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define SERVER_PORT 2222 //UDP�󶨶˿ں�
#define BUFFER_SIZE 1024 
#define RSSITHD -30   //wifi̽��̽�����ֵ
#define REC_RSSI_COUNT 10 //ϵͳĬ�ϴ洢rssi���������

  //���� �Ѳ���
// struct cliprobeData { //̽�뷵�����ݸ�ʽ  
// 	unsigned char Apmac[6];          //Դ AP �� MAC ��ַ
// 	unsigned short int Vendorid;     //���� ID
// 	unsigned short int Reserved1;    //����λ
// 	unsigned char Bssid[6];          //AP �� BSSID
// 	unsigned char Radiotype;         //������Ϣ�����ߵ����� 0x01��802.11b
// 	//0x02��802.11g
// 	//0x03��802.11a
// 	//0x04��802.11n
// 	//0x05��802.11ac
// 	unsigned char Channel;           //AP����MU����ʱ���ڵ��ŵ�
// 	unsigned char Associated;        //MU�Ƿ����ӵ� AP��0x01���� 0x02����
// 	unsigned char Messagetype;       //�������� 0x00��Probe Request 0x01��Association Request
// 	char Timestamp[14];              //����MU��Ϣ��ʱ��㣬���뼶��ʱ���
// 	unsigned short int Reserved2;    //����λ
// 	unsigned char Mutype;            //MU�����ͣ�0x01��δ֪���� 0x02��MU
// 	unsigned short int Reserved3;    //����λ
// 	char Rssi;                       //����������dBmΪ��λ��RSSI ����ֵ ʮ���ƣ�-128 ��127
// 	unsigned short int Reserved4;    //����λ
// 	unsigned char Noisefloor;        //����������dBmΪ��λ�ĵ�������ֵ
// 	unsigned short int Reserved5;    //����λ
// 	unsigned char Datarate;          //������Ϣ�����ߵ����ͣ�0x01��802.11b
// 	//0x02��802.11g
// 	//0x03��802.11a
// 	//0x04��802.11n
// 	//0x05��802.11ac
// 	unsigned char MPDUflags;         //����MPDU��flag���ȷ��MU�����Ƿ�Я��Frame Control��Sequence Control�ֶ�
// 	//1������Я����Ӧ�ֶΣ�0������Я����Ӧ�ֶΣ�
// 	unsigned char Mumac[6];          //�����ź�MU��MAC��ַ
// 	unsigned short int Framecontrol;   //MPDU�С�FrameControl���ֶ�
// 	unsigned short int Sequencecontrol;//MPDU �С�SequenceControl���ֶ�
// 	unsigned short int Reserved6;      //����λ
// };

//float meanRssiSpecial(float lastOutput,float input,int num)//ֱ�Ӽ���ƽ��ֵ,��ʱ�任ȡ�ռ䣬���ϴ���֪��ֵ��ͳ�ƵĴ�����ֱ����ü���ֵ��ƽ����
//{														   //�������øú������ã���float����̫�鷳�ˣ�ͨ��ͳ�ƺ�ֱ��ȥ���ƺ�����Ҳ���Щ
//	float avgNum;
//	float fnum;
//	fnum=float(num);
//	if(num==1)
//	{
//		avgNum=input;
//	}
//	else
//	{
//		avgNum=((fnum-1.0)/fnum)*lastOutput+(1.0/fnum)*input;
//	}
//	printf("��ֵ�������ص�floatֵ��%d\n",avgNum);
//	return avgNum;
//}

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
	void reSelMacRssi();
	string charTo02XStr(unsigned char input);
	string macToString(unsigned char Mymac[6]);//��ɽ�char����ת�����ַ���
	void getSpecialMac(unsigned char Mymac[6]);//ת���ַ�������ʽ�ĺ���
	void getSpecialRssi(char MyRssi);//ת���ַ�������ʽ�ĺ���
	char MaxRssi(char rssi1,char rssi2);//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
	void mobileManuOutput(mncatsWifi &Probedata);//����ֻ������ĳ���

private:
	//mac�볧�̲��Ҳ���
	std::map<std::string,std::string> mobileManu;//Mac��ӳ���ϵ��

};

