#include "wifi.h"
//wifi的类
//可能以后，先设置一个set集将一些常见的wifi剔除掉，可以自动处理的。节省人力
Wifi::Wifi()
{
	//将要剔除的mac地址读入到list中
	ifstream infile; 
	infile.open("deny.txt");   
	string s;
	while(getline(infile,s))//先读取一行的数据
	{
		//cout<<a<<endl;//只读取前面的数据，因为要考虑的是不同时间段的种类，而不是其他的
		denyList.insert(s);
	}
	infile.close();             //关闭文件输入流 

	memset(&zeroMac,0,sizeof(unsigned char)*6);//定义一个全为零的量
	memset(&sel,0,sizeof(selMacRssi)*REC_RSSI_COUNT);
}

Wifi::~Wifi()
{
	closesocket(s);
	::WSACleanup();
}

void Wifi::InitWifi()//初始化server服务器端
{
	//文本类使用getchar(),openCV使用waitkey,当获取键盘按键时采用通用的GetKeyState(), system("pause")也是一个很好的选择
	/*加载winsock文件*/
	if(WSAStartup(MAKEWORD(2,2),&wsd)!=0){
		printf("WSAStartup failed\n");
	}
	//创建嵌套字
	s=socket(AF_INET, SOCK_DGRAM, 0);
	if(s==INVALID_SOCKET){   
		printf("Failed socket() %d\n",WSAGetLastError());
		::WSACleanup();
		system("pause");
	}
	//绑定嵌套字

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERVER_PORT);//port
	servAddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);//ip 

	if(bind(s,(SOCKADDR *)&servAddr,sizeof(servAddr)) == SOCKET_ERROR){
		printf("bind() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	//可选 Mac厂商查找部分
	Csv csv("精简版手机品牌对应表.csv");//该品牌对应表不是很全，当时有删掉的部分，下次可以只把主要手机的Mac地址记录下来即可。
	for (auto i=csv.table.begin();i!=csv.table.end();i++)
	{
		auto i1=i->begin();
		auto i2=i->begin()+1;
		mobileManu[*i1]=*i2;
	}
}

void Wifi::mobileManuOutput(mncatsWifi &Probedata)//输出手机网卡的对应厂商
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
		std::cout<<"其他品牌";
	}
}

void Wifi::wifiProcess()//记录Mac值和对应的RSSI值
{
	int clientAddrLength = sizeof(clientAddr); 
	char buffer[BUFFER_SIZE];
	ZeroMemory(buffer, BUFFER_SIZE); 
	//定义接收
	if (recvfrom(s,buffer,BUFFER_SIZE,0,(SOCKADDR *)&clientAddr,&clientAddrLength) == SOCKET_ERROR)
	{
		printf("recvfrom() failed: %d\n",WSAGetLastError());
		closesocket(s);
		::WSACleanup();
		system("pause");
	}
	mncatsWifi datatemp=mncatsWifi(buffer);//格式化数据
	//剔除denyList中的Mac地址
	auto deIt=find(denyList.begin(),denyList.end(),datatemp.mac2); //查找denyList中的元素，如果没找到，返回end()
	if((datatemp.dtype!="80")&&(int(datatemp.crssi)>RSSITHD)&&(int(datatemp.crssi)!=0)&&(deIt==denyList.end()))//剔除路由器的mac地址
	{
		//存储 同时来两套，以后再想办法修改这个类
		if(datatemp.mac1=="C8:E7:D8:D4:A3:60")//有向天线的效果
		{
			selMapDing[datatemp.mac2].push_back(int(datatemp.crssi)); 
		}
		else if (datatemp.mac1=="C8:E7:D8:D4:A3:02")//全向天线的效果
		{
			selMap[datatemp.mac2].push_back(int(datatemp.crssi)); 
		}
	}
}

void Wifi::reSelMacRssi()//将记录的结构体数组重新置零
{
	selMap.erase(selMap.begin(),selMap.end()); //清空数据
	selMapDing.erase(selMapDing.begin(),selMapDing.end()); //清空数据
}

//项目wifi处理函数

string Wifi::wifiProcessed()//判断和输出程序      改进后完成的是输出
{
	int rssiTemp=-100;
	int index=0;
	int del;
	string str;
	for(int i=0;i<REC_RSSI_COUNT;i++)
	{
		if(memcmp(sel[i].selMumac,zeroMac,sizeof(unsigned char)*6)==0)//当检测到区域为零时，跳出程序
		{
			break;
		}
		else //记录最大值的程序
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
		printf("对不起,系统未匹配到Mac地址\n");
		str="0";
	}
	else
	{
		//输出最大值的部分
		printf("匹配到的最可能的mac码：\n");
		printf("%02X:%02X:%02X:%02X:%02X:%02X\n",sel[index].selMumac[0],sel[index].selMumac[1],\
			sel[index].selMumac[2],sel[index].selMumac[3],sel[index].selMumac[4],sel[index].selMumac[5]);
		//可以将全部采集到的信号输出，从而完成排序前几个的功能
		str=macToString(sel[index].selMumac);
	}
	return str;
}

void  Wifi::wifiProcessed2(map<string,vector<int>> &ss,vector<mapUsed> &dst,ofstream &raw)//新的方法 完成的是排序后的输出
{
	for (auto const& name:ss)
	{
		mapUsed temp;
		temp.macName=name.first;
		raw<<name.first<<":"<<endl;
		temp.num=name.second.size();
		for (auto const&ttt:name.second)
		{
			raw<<ttt<<" ";
		}
		raw<<endl;
		int sumTemp=accumulate(name.second.begin(),name.second.end(),0);
		temp.avgRssi=sumTemp/int(name.second.size());
		dst.push_back(temp);
	}
	sort(dst.begin(),dst.end(),[](mapUsed &r1,mapUsed &r2){return r1.avgRssi>r2.avgRssi;}); //最后按大小排序
	raw.close();
}

//辅助函数

string Wifi::charTo02XStr(unsigned char input)//将char类型转换成02X字符串型
{
	int high,low;
	char out[2]={0};

	high=(input&240)>>4;//高四位
	low=input&15;		//低四位
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
	string output(out,2);//第二个参数用来控制string的长度，不然会出现乱码
	return output;
}

string Wifi::macToString(unsigned char Mymac[6])//完成将char类型转换成字符串
{
	string output,temp;
	output=charTo02XStr(Mymac[0]);
	for(int i=1;i<6;i++)
	{
		output=output+"_"+charTo02XStr(Mymac[i]);
	}
	return output;
}

void Wifi::getSpecialMac(unsigned char Mymac[6])//转换字符串的形式的函数
{
	//unsigned char Mymac[6]={0};//b0,e2,35,2b,da,e0
	cout<<"输入要特定识别的字符串格式：（请以冒号格式输入，且为小写）"<<endl;
	scanf("%02x:%02x:%02x:%02x:%02x:%02x",&Mymac[0],&Mymac[1],&Mymac[2],&Mymac[3],&Mymac[4],&Mymac[5]);
	cout<<"输出转换后形式："<<endl;
	printf("%hhu,%hhu,%hhu,%hhu,%hhu,%hhu\n",unsigned char(Mymac[0]),Mymac[1],Mymac[2],Mymac[3],Mymac[4],Mymac[5]);
}

void Wifi::getSpecialRssi(char MyRssi)//转换字符串的形式的函数
{
	cout<<"输入最小的dB："<<endl;
	scanf("%d",MyRssi);
	cout<<"输出转换后形式："<<endl;
	printf("%d\n",MyRssi);
}

char Wifi::MaxRssi(char rssi1,char rssi2)//返回较大的RSSI值，且该值不能能等于0
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
