#include "wifi.h"
//wifi����

Wifi::Wifi()
{
	memset(&zeroMac,0,sizeof(unsigned char)*6);//����һ��ȫΪ�����
	memset(&sel,0,sizeof(selMacRssi)*REC_RSSI_COUNT);
}

Wifi::~Wifi()
{
	closesocket(s);
	::WSACleanup();
}

void Wifi::InitWifi()//��ʼ��server��������
{
	//�ı���ʹ��getchar(),openCVʹ��waitkey,����ȡ���̰���ʱ����ͨ�õ�GetKeyState(), system("pause")Ҳ��һ���ܺõ�ѡ��
	/*����winsock�ļ�*/
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
	}
	//����Ƕ����
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	//��Ƕ����

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);//port
	servAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip 

	if(bind(s,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("bind() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	//��ѡ Mac���̲��Ҳ���
	Csv csv("������ֻ�Ʒ�ƶ�Ӧ��.csv");//��Ʒ�ƶ�Ӧ���Ǻ�ȫ����ʱ��ɾ���Ĳ��֣��´ο���ֻ����Ҫ�ֻ���Mac��ַ��¼�������ɡ�
	for (auto i=csv.table.begin();i!=csv.table.end();i++)
	{
		auto i1=i->begin();
		auto i2=i->begin()+1;
		mobileManu[*i1]=*i2;
	}
}

void Wifi::mobileManuOutput(mncatsWifi &Probedata)//����ֻ������Ķ�Ӧ����
{
	std::string mac;
	mac=Probedata.mac2.substr(0,2)+Probedata.mac2.substr(3,2)+Probedata.mac2.substr(6,2);
	auto itt = mobileManu.find(mac);
	if (itt != mobileManu.end())
	{
		std::cout<<itt->second;
	}
	else
	{
		std::cout<<"����Ʒ��";
	}
}

void Wifi::wifiProcess()//��¼Macֵ�Ͷ�Ӧ��RSSIֵ
{
	int clientAddrLength = sizeof(clientAddr); 
	char buffer[BUFFER_SIZE];
	ZeroMemory(buffer, BUFFER_SIZE); 
	//�������
	if (recvfrom(s,buffer,BUFFER_SIZE,0,(SOCKADDR *)&clientAddr,&clientAddrLength) == SOCKET_ERROR)
	{
		printf("recvfrom() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	mncatsWifi datatemp=mncatsWifi(buffer);//��ʽ������
	if((datatemp.dtype!="80")&&(int(datatemp.crssi)>RSSITHD))
	{
		cout<<"�Ѽ�⵽�ź�:\n"<<endl;
		//printf("%02X:%02X:%02X:%02X:%02X:%02X\n",packageData->Mumac[0],packageData->Mumac[1],\
		//packageData->Mumac[2],packageData->Mumac[3],packageData->Mumac[4],packageData->Mumac[5]);
		//printf( "%d\n",packageData->Rssi);
		mobileManuOutput(datatemp);//���Ʒ��
		for(int i=0;i<REC_RSSI_COUNT;i++)
		{
			if(memcmp(sel[i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//����⵽����Ϊ��ʱ���Ѳ��ֵ��ֵ��������
			{
				memcpy(sel[i].selMumac,datatemp.cmac2,sizeof(unsigned char)*6);
				sel[i].num=1;
				sel[i].maxRssi=datatemp.crssi;
				sel[i].sumRssi=int(datatemp.crssi);
				break;//����ѭ��
			}
			else if(memcmp(datatemp.cmac2,sel[i].selMumac,sizeof(unsigned char)*6)==0)//�Ƚ��Ƿ�������������,memcmp ��ͬ����0,��ͬ���ط���
			{
				sel[i].num=sel[i].num+1;
				sel[i].maxRssi=MaxRssi(sel[i].maxRssi,datatemp.crssi);
				sel[i].sumRssi=sel[i].sumRssi+int(datatemp.crssi);
				//cout<<sel[i].sumRssi<<endl;//�������
				//cout<<sel[i].maxRssi<<endl;
				break;//����ѭ��
			}
		}
	}
}

void Wifi::reSelMacRssi()//����¼�Ľṹ��������������
{
	memset(&sel,0,sizeof(selMacRssi)*REC_RSSI_COUNT);
}

string Wifi::wifiProcessed()//�жϺ��������
{
	int rssiTemp=-100;
	int index=0;
	int del;
	string str;
	for(int i=0;i<REC_RSSI_COUNT;i++)
	{
		if(memcmp(sel[i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//����⵽����Ϊ��ʱ����������
		{
			break;
		}
		else //��¼���ֵ�ĳ���
		{
			del=sel[i].sumRssi/sel[i].num;
			if(del>rssiTemp)
			{
				rssiTemp=del;
				index=i;
			}
		}
	}
	if(memcmp(sel[index].selMumac,zeroMac,sizeof(unsigned char)*6)==0)
	{
		printf("�Բ���,ϵͳδƥ�䵽Mac��ַ\n");
		str="0";
	}
	else
	{
		//������ֵ�Ĳ���
		printf("ƥ�䵽������ܵ�mac�룺\n");
		printf("%02X:%02X:%02X:%02X:%02X:%02X\n",sel[index].selMumac[0],sel[index].selMumac[1],\
			sel[index].selMumac[2],sel[index].selMumac[3],sel[index].selMumac[4],sel[index].selMumac[5]);
		//���Խ�ȫ���ɼ������ź�������Ӷ��������ǰ�����Ĺ���

		//printf("��Ӧ��ƽ���ź�ֵ��%d\n",rssiTemp);
		//printf("��Ӧ������ź�ֵ��%d\n",sel[index].maxRssi);
		str=macToString(sel[index].selMumac);
	}
	return str;
}

string Wifi::charTo02XStr(unsigned char input)//��char����ת����02X�ַ�����
{
	int high,low;
	char out[2]={0};

	high=(input&240)>>4;//����λ
	low=input&15;		//����λ
	if (high>=10)
	{
		out[0]=(high-10)+'A';
	}
	else
	{
		out[0]=(high)+'0';
	}
	if (low>=10)
	{
		out[1]=(low-10)+'A';
	}
	else
	{
		out[1]=(low)+'0';
	}
	string output(out,2);//�ڶ���������������string�ĳ��ȣ���Ȼ���������
	return output;
}

string Wifi::macToString(unsigned char Mymac[6])//��ɽ�char����ת�����ַ���
{
	string output,temp;
	output=charTo02XStr(Mymac[0]);
	for(int i=1;i<6;i++)
	{
		output=output+"_"+charTo02XStr(Mymac[i]);
	}
	return output;
}

void Wifi::getSpecialMac(unsigned char Mymac[6])//ת���ַ�������ʽ�ĺ���
{
	//unsigned char Mymac[6]={0};//b0,e2,35,2b,da,e0
	cout<<"����Ҫ�ض�ʶ����ַ�����ʽ��������ð�Ÿ�ʽ���룬��ΪСд��"<<endl;
	scanf("%02x:%02x:%02x:%02x:%02x:%02x",&Mymac[0],&Mymac[1],&Mymac[2],&Mymac[3],&Mymac[4],&Mymac[5]);
	cout<<"���ת������ʽ��"<<endl;
	printf("%hhu,%hhu,%hhu,%hhu,%hhu,%hhu\n",unsigned char(Mymac[0]),Mymac[1],Mymac[2],Mymac[3],Mymac[4],Mymac[5]);
	//system("pause");
}

void Wifi::getSpecialRssi(char MyRssi)//ת���ַ�������ʽ�ĺ���
{
	cout<<"������С��dB��"<<endl;
	scanf("%d",MyRssi);
	cout<<"���ת������ʽ��"<<endl;
	printf("%d\n",MyRssi);
	//system("pause");
}

char Wifi::MaxRssi(char rssi1,char rssi2)//���ؽϴ��RSSIֵ���Ҹ�ֵ�����ܵ���0
{
	char output;
	char t_1,t_2;
	t_1=rssi1;
	t_2=rssi2;
	if(int(t_1)==0)
	{
		output=rssi2;
	}
	else if(int(t_2)==0)
	{
		output=rssi1;
	}
	else
	{
		output=int(t_1)>int(t_2)?rssi1:rssi2;
	}
	return output;
}
