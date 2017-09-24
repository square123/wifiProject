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
#include <set>
#include <numeric>
#include <algorithm>
#include <vector>
#include <fstream>//希望输入时间
#include "MncatsWifi.h"
#pragma comment(lib,"WS2_32.lib")
using namespace std;

#define SERVER_PORT 2222 //UDP绑定端口号
#define BUFFER_SIZE 1024 
#define RSSITHD -50   //wifi探针探测的阈值
#define REC_RSSI_COUNT 10 //系统默认存储rssi的种类个数 根据具体情况来分析

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
	set<string> denyList;//定义需要剔除的MAC地址 以后可以通过数组来完成
	map<string,vector<int>> selMapDing;//新的存储格式   //针对数据库可以在以后只存储最可能的前5个数据量，便于保存 //这个写的不太好
	struct mapUsed  //在排序时要使用的格式
	{
		string macName;
		int num;
		int avgRssi;
	};
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
	void  wifiProcessed2(map<string,vector<int>> &ss,vector<mapUsed> &,ofstream &raw,ofstream &rawPro);//新的方法 完成的是排序后的输出
	void reSelMacRssi();
	string charTo02XStr(unsigned char input);
	string macToString(unsigned char Mymac[6]);//完成将char类型转换成字符串
	void getSpecialMac(unsigned char Mymac[6]);//转换字符串的形式的函数
	void getSpecialRssi(char MyRssi);//转换字符串的形式的函数
	char MaxRssi(char rssi1,char rssi2);//返回较大的RSSI值，且该值不能能等于0

};

