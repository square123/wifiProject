#include "wifi.h"
//wifi����
//�����Ժ�������һ��set����һЩ������wifi�޳����������Զ�����ġ���ʡ����
Wifi::Wifi()
{
	//��Ҫ�޳���mac��ַ���뵽list��
	ifstream infile; 
	infile.open("deny.txt");   
	string s;
	while(getline(infile,s))//�ȶ�ȡһ�е�����
	{
		//cout<<a<<endl;//ֻ��ȡǰ������ݣ���ΪҪ���ǵ��ǲ�ͬʱ��ε����࣬������������
		denyList.insert(s);
	}
	infile.close();             //�ر��ļ������� 

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
	//�޳�denyList�е�Mac��ַ
	auto deIt=find(denyList.begin(),denyList.end(),datatemp.mac2); //����denyList�е�Ԫ�أ����û�ҵ�������end()
	if((datatemp.dtype!="80")&&(int(datatemp.crssi)>RSSITHD)&&(int(datatemp.crssi)!=0)&&(deIt==denyList.end()))//�޳�·������mac��ַ
	{
		//�洢 ͬʱ�����ף��Ժ�����취�޸������
		if(datatemp.mac1=="C8:E7:D8:D4:A3:60")//�������ߵ�Ч��
		{
			selMapDing[datatemp.mac2].push_back(int(datatemp.crssi)); 
		}
	}
}

void Wifi::reSelMacRssi()//����¼�Ľṹ��������������
{
	selMapDing.erase(selMapDing.begin(),selMapDing.end()); //�������
}

//��Ŀwifi������

string Wifi::wifiProcessed()//�жϺ��������      �Ľ�����ɵ������
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
		str=macToString(sel[index].selMumac);
	}
	return str;
}

void  Wifi::wifiProcessed2(map<string,vector<int>> &ss,vector<mapUsed> &dst,ofstream &raw,ofstream &rawPro,string enterTime)//�µķ��� ��ɵ������������ //��������  �������  ����ԭʼ���ݵĲ�������������ʱ��
{
	//����Ҫ�����֤����Ϊ�ļ���������ÿ���ļ��м�����־ѡ�����ʱ������
	raw<<enterTime<<" "<<ss.size()<<endl; //ʱ�� Mac����

	//ԭʼ�������+����
	for (auto const& name:ss)
	{
		mapUsed temp;
		temp.macName=name.first;
		raw<<name.first<<" "<<name.second.size()<<" ";//Mac���� ���� RSSIֵ
		temp.num=name.second.size();
		for (auto const & ttt:name.second) 
		{
			raw<<ttt<<" ";
		}
		raw<<endl;
		int sumTemp=accumulate(name.second.begin(),name.second.end(),0);
		temp.avgRssi=sumTemp/int(name.second.size());
		temp.score=temp.num/(temp.avgRssi==0?1000.0:abs(temp.avgRssi));//�����п��ܳ���Ϊ0��������Ͱ�����滻�޳���
		//cout<<temp.score<<"--------------------------------------!!!!!!!!!!!!!!!!"<<endl;
		dst.push_back(temp);
	}

	//����������
	rawPro<<enterTime<<" "<<ss.size()<<endl; //ʱ�� Mac����
	sort(dst.begin(),dst.end(),[](mapUsed r1,mapUsed r2){return r1.score>r2.score;}); //��󰴴�С���� //��������ʽ�ǿ��Ե����� ��Ϊ�ڷ������Ѿ����ǵ�0�Ĳ���������ں��������ʱ�����ٿ��ǵ�0�����
	for (int it=0;it<dst.size();it++) //������õ���Ϣ���
	{
			rawPro<<dst[it].macName<<" "<<dst[it].avgRssi<<" "<<dst[it].num<<" "<<dst[it].score<<endl;
	}
}

//��������
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
