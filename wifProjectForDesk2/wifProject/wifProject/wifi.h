//#include<winsock2.h>
#pragma once

#include<stdio.h>
#include<windows.h>
#include<string>
#include<tchar.h>
#include<iostream>
#include <iomanip>//可能是格式化16进制的输出文件
//#include<time.h>//加入时间
#include <map>
#include <fstream>//希望输入时间
#include "MncatsWifi.h"
#include "myCsv.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define SERVER_PORT 2222 //UDP绑定端口号
#define BUFFER_SIZE 1024 
#define RSSITHD -30   //wifi探针探测的阈值
#define REC_RSSI_COUNT 10 //系统默认存储rssi的种类个数

  //旧类 已不用
// struct cliprobeData { //探针返回数据格式  
// 	unsigned char Apmac[6];          //源 AP 的 MAC 地址
// 	unsigned short int Vendorid;     //厂商 ID
// 	unsigned short int Reserved1;    //保留位
// 	unsigned char Bssid[6];          //AP 的 BSSID
// 	unsigned char Radiotype;         //接收消息的无线电类型 0x01：802.11b
// 	//0x02：802.11g
// 	//0x03：802.11a
// 	//0x04：802.11n
// 	//0x05：802.11ac
// 	unsigned char Channel;           //AP接收MU报文时所在的信道
// 	unsigned char Associated;        //MU是否连接到 AP：0x01：是 0x02：否
// 	unsigned char Messagetype;       //报文类型 0x00：Probe Request 0x01：Association Request
// 	char Timestamp[14];              //接收MU消息的时间点，毫秒级的时间戳
// 	unsigned short int Reserved2;    //保留位
// 	unsigned char Mutype;            //MU的类型：0x01：未知类型 0x02：MU
// 	unsigned short int Reserved3;    //保留位
// 	char Rssi;                       //测量到的以dBm为单位的RSSI 整数值 十进制：-128 到127
// 	unsigned short int Reserved4;    //保留位
// 	unsigned char Noisefloor;        //测量到的以dBm为单位的底噪整数值
// 	unsigned short int Reserved5;    //保留位
// 	unsigned char Datarate;          //接收消息的无线电类型：0x01：802.11b
// 	//0x02：802.11g
// 	//0x03：802.11a
// 	//0x04：802.11n
// 	//0x05：802.11ac
// 	unsigned char MPDUflags;         //根据MPDU的flag标记确定MU报文是否携带Frame Control和Sequence Control字段
// 	//1：代表携带对应字段，0：代表不携带对应字段：
// 	unsigned char Mumac[6];          //发射信号MU的MAC地址
// 	unsigned short int Framecontrol;   //MPDU中“FrameControl”字段
// 	unsigned short int Sequencecontrol;//MPDU 中“SequenceControl”字段
// 	unsigned short int Reserved6;      //保留位
// };

//float meanRssiSpecial(float lastOutput,float input,int num)//直接计算平均值,用时间换取空间，用上次已知的值和统计的次数，直接求得几个值的平均数
//{														   //后来觉得该函数不好，用float或许太麻烦了，通过统计和直接去求似乎精度也会高些
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
//	printf("均值函数返回的float值：%d\n",avgNum);
//	return avgNum;
//}

class Wifi
{
public:

	struct selMacRssi//定义该结构体数组来统计出现RSSI值的情况，避免使用变长的存储，由于短时间内不会出现很多Mac地址小于某阈值，所以这里只设置10个值
	{
		unsigned char selMumac[6];      //发射信号MU的MAC地址
		int sumRssi;                    //返回RSSI的总和
		char maxRssi;                   //返回RSSI的最大值
		int num;						//统计该RSSI出现的次数
	}sel[REC_RSSI_COUNT];//定义要存储的结构体数组
	unsigned char zeroMac[6];
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	FILE *fp;
	sockaddr_in servAddr;//服务器地址
	//struct cliprobeData *packageData;
	sockaddr_in clientAddr; 
	Wifi();
	~Wifi();
	void InitWifi();
	void wifiProcess();
	string wifiProcessed();
	void reSelMacRssi();
	string charTo02XStr(unsigned char input);
	string macToString(unsigned char Mymac[6]);//完成将char类型转换成字符串
	void getSpecialMac(unsigned char Mymac[6]);//转换字符串的形式的函数
	void getSpecialRssi(char MyRssi);//转换字符串的形式的函数
	char MaxRssi(char rssi1,char rssi2);//返回较大的RSSI值，且该值不能能等于0
	void mobileManuOutput(mncatsWifi &Probedata);//输出手机网卡的厂商

private:
	//mac码厂商查找部分
	std::map<std::string,std::string> mobileManu;//Mac码映射关系表

};

