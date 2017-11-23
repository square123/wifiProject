#pragma once
#include <windows.h>
#include <cstdio>
#pragma comment(lib,"WS2_32.lib") //很坑爹的头文件相互包含，这里就不使用winsock2.h作为头文件了，因为windows已经包含了。

class mySocketServer
{
public:
	mySocketServer(const int &inputPort,const char* inputIP="0.0.0.0");//默认是通用的IP 
	~mySocketServer();

	void transData(char *recvData,int recvDataLength, const char *sendData="0", int sendDataLength = 1);//默认传输一个0作为回应 recvData是接收的数据
	void sendFile(const char* filePath);//传输文件

	SOCKADDR_IN addrClient;//所连接的客户端地址
	
private:
	WSADATA wsaData;
	SOCKET sockSrv;		
	SOCKADDR_IN addrSrv;
	//一些服务器端要用的数据
	int len;
};

class mySocketClient
{
public:
	mySocketClient(const int &inputPort,const char* inputIP);
	~mySocketClient();

	void transData(char *recvData,int recvDataLength, const char *sendData, int sendDataLength);//为了和客户端的统一都采用相同的参数列表
	void recvFile(const char *filePath);//接收文件

private:
	WSADATA wsaData;
	SOCKADDR_IN addrSrv;
};