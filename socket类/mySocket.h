#pragma once
#include <windows.h>
#include <cstdio>
#pragma comment(lib,"WS2_32.lib") //�ܿӵ���ͷ�ļ��໥����������Ͳ�ʹ��winsock2.h��Ϊͷ�ļ��ˣ���Ϊwindows�Ѿ������ˡ�

class mySocketServer
{
public:
	mySocketServer(const int &inputPort,const char* inputIP="0.0.0.0");//Ĭ����ͨ�õ�IP 
	~mySocketServer();

	void transData(char *recvData,int recvDataLength, const char *sendData="0", int sendDataLength = 1);//Ĭ�ϴ���һ��0��Ϊ��Ӧ recvData�ǽ��յ�����
	void sendFile(const char* filePath);//�����ļ�

	SOCKADDR_IN addrClient;//�����ӵĿͻ��˵�ַ
	
private:
	WSADATA wsaData;
	SOCKET sockSrv;		
	SOCKADDR_IN addrSrv;
	//һЩ��������Ҫ�õ�����
	int len;
};

class mySocketClient
{
public:
	mySocketClient(const int &inputPort,const char* inputIP);
	~mySocketClient();

	void transData(char *recvData,int recvDataLength, const char *sendData, int sendDataLength);//Ϊ�˺Ϳͻ��˵�ͳһ��������ͬ�Ĳ����б�
	void recvFile(const char *filePath);//�����ļ�

private:
	WSADATA wsaData;
	SOCKADDR_IN addrSrv;
};