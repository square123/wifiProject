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
#pragma comment(lib,"WS2_32.lib")
using namespace std;

//以后可以调整的
#define RSSITHD -50   //wifi探针探测的阈值
#define SERVER_PORT 2222 //UDP绑定端口号

#define BUFFER_SIZE 1024 
#define REC_RSSI_COUNT 10 //系统默认存储rssi的种类个数 根据具体情况来分析

//封装的探针数据类型格式，或者将所有的数据格式都封装成和这个类似的格式
class mncatsWifi 
{
public :
	std::string initData;//用来存储char转换后的字符串
	std::string mac1;//存储探针的mac码 0-11
	std::string mac2;//存储探测的mac码 13-24
	std::string rssi;//存储Rssi值 26-28
	std::string dtype;//信号类型 30-31 //以后再分析
	std::string wifidate;//日期 33-42
	std::string wifitime;//时间 44-51
	std::string timeStamp;//综合的字符串
	unsigned char cmac2[6];//存放mac2码的数组
	char crssi;//char格式的rssi
	char Timestamp[14];//接口的时间格式

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

//需要根据新探针来修改，之前写的还是不够好，应该要避免一些耦合的因素，将与类相关的格式都维持一致
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
		double score; //后面要计算的分数
	};
	unsigned char zeroMac[6];
	WSADATA wsd;//WSADATA 变量
	SOCKET s;//嵌套字
	FILE *fp;
	sockaddr_in servAddr;//服务器地址
	sockaddr_in clientAddr; 
	Wifi();
	~Wifi();
	void InitWifi();
	void wifiProcess();
	string wifiProcessed();
	void  wifiProcessed2(map<string,vector<int>> &ss,vector<mapUsed> &,ofstream &raw,ofstream &rawPro,string srcTime);//新的方法 完成的是排序后的输出
	void reSelMacRssi();
	string charTo02XStr(unsigned char input);
	string macToString(unsigned char Mymac[6]);//完成将char类型转换成字符串
	char MaxRssi(char rssi1,char rssi2);//返回较大的RSSI值，且该值不能能等于0
};

