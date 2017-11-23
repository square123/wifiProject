#include "mySocket.h"

mySocketServer::mySocketServer(const int &inputPort,const char* inputIP)
{
	len = sizeof(SOCKADDR);
	//��������  setsocket()
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
	}

	sockSrv = socket(AF_INET, SOCK_STREAM, 0);//�������ڼ������׽���

	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(inputPort); //1024���ϵĶ˿ں�
	addrSrv.sin_addr.S_un.S_addr = (inputIP=="0.0.0.0"?htonl(INADDR_ANY):inet_addr(inputIP));//��������ip��ַĬ����ͨ�õ�

	//�������˵�һЩ���� bind()
	int retVal = bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if(retVal == SOCKET_ERROR){
		printf("Failed bind:%d\n", WSAGetLastError());
	}

	//�������˵�һЩ���� listen()
	if(listen(sockSrv,10) ==SOCKET_ERROR){
		printf("Listen failed:%d", WSAGetLastError());
	}
}

mySocketServer::~mySocketServer()
{
	closesocket(sockSrv);//����������Ӧ��Ҫ�ر����Լ���socket
	WSACleanup();
}

void mySocketServer::sendFile(const char* filePath )
{
	FILE *fp=fopen(filePath,"rb");
	if (fp != NULL)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR *) &addrClient, &len);
		char buffer[1024]={0};
		int nCount;
		while ((nCount = fread(buffer,1,1024,fp))>0)
		{
			send(sockConn,buffer,nCount,0);
		}
		shutdown(sockConn,0x01);//SD_SEND Ϊ�˺������Ĳ���ͻ
		recv(sockConn,buffer,1024,0);
		closesocket(sockConn);
	}
	fclose(fp);
}

void mySocketServer::transData(char *recvData, int recvDataLength, const char *sendData, int sendDataLength)//ֻ�������һ������
{
	SOCKET sockConn = accept(sockSrv, (SOCKADDR *) &addrClient, &len);
	if(sockConn == SOCKET_ERROR){
		printf("Accept failed:%d", WSAGetLastError());
	}

	if(strlen(sendData)<=sendDataLength){
		//����Ӧ������ send()
		int iSend = send(sockConn, sendData, sendDataLength , 0);
		if(iSend == SOCKET_ERROR)
		{
			printf("send failed");
		}
	}else
	{
		printf("The sendLength is wrong");
	}

	//�������� recv()
	if (recvDataLength>=1)
	{
		recv(sockConn, recvData, recvDataLength, 0);
	}else
	{
		printf("The recvLength is wrong");
	}

	//�رձ���ѭ��������socket
	closesocket(sockConn);
}

mySocketClient::mySocketClient(const int &inputPort,const char* inputIP)
{
	//�������� setsocket()
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
	}

	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(inputPort);
	addrSrv.sin_addr.S_un.S_addr = inet_addr(inputIP);
}

mySocketClient::~mySocketClient()
{
	WSACleanup();
}

void mySocketClient::recvFile(const char *filePath)
{
	FILE *fp=fopen(filePath,"wb");
	SOCKET sockClient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv));
	char buffer[1024]={0};
	int nCount;
	while ((nCount = recv(sockClient,buffer,1024,0))>0)
	{
		fwrite(buffer,nCount,1,fp);
	}
	closesocket(sockClient);
	fclose(fp);
}

void mySocketClient::transData(char *recvData,int recvDataLength, const char *sendData, int sendDataLength)
{
	SOCKET sockClient;
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if(SOCKET_ERROR == sockClient){
		printf("Socket() error:%d", WSAGetLastError());
	}

	//һ��һ��
	//���ӷ����� connect()			
	//�������� recv()
	if (recvDataLength>=1)
	{
		if(connect(sockClient, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET){
			printf("Connect failed:%d", WSAGetLastError());
		}
		else
		{
			recv(sockClient, recvData, recvDataLength, 0);
		}
	}else
	{
		printf("The recvLength is wrong");
	}
	//�������� send()
	if(strlen(sendData)<=sendDataLength)
	{
		send(sockClient, sendData, sendDataLength, 0);
	}
	else
	{
		printf("The sendLength is wrong");
	}
	//�ͻ���ֻ����һ��Ƕ���֣����Բ�Ҫ���׹ر� 
	closesocket(sockClient);
}
