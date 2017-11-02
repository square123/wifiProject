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
	}
}

void Wifi::reSelMacRssi()//将记录的结构体数组重新置零
{
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

void  Wifi::wifiProcessed2(map<string,vector<int>> &ss,vector<mapUsed> &dst,ofstream &raw,ofstream &rawPro,string enterTime)//新的方法 完成的是排序后的输出 //输入数据  输出数据  保存原始数据的操作，还有输入时间
{
	//这里要以身份证号作为文件命名，在每个文件中加入日志选项，还有时间输入
	raw<<enterTime<<" "<<ss.size()<<endl; //时间 Mac个数

	//原始数据输出+处理
	for (auto const& name:ss)
	{
		mapUsed temp;
		temp.macName=name.first;
		raw<<name.first<<" "<<name.second.size()<<" ";//Mac名称 个数 RSSI值
		temp.num=name.second.size();
		for (auto const & ttt:name.second) 
		{
			raw<<ttt<<" ";
		}
		raw<<endl;
		int sumTemp=accumulate(name.second.begin(),name.second.end(),0);
		temp.avgRssi=sumTemp/int(name.second.size());
		temp.score=temp.num/(temp.avgRssi==0?1000.0:abs(temp.avgRssi));//考虑有可能出现为0的情况，就把这个替换剔除掉
		//cout<<temp.score<<"--------------------------------------!!!!!!!!!!!!!!!!"<<endl;
		dst.push_back(temp);
	}

	//处理后的数据
	rawPro<<enterTime<<" "<<ss.size()<<endl; //时间 Mac个数
	sort(dst.begin(),dst.end(),[](mapUsed r1,mapUsed r2){return r1.score>r2.score;}); //最后按大小排序 //最后的排序方式是可以调整的 因为在分数中已经考虑到0的操作，因此在后续的输出时不需再考虑到0的情况
	for (int it=0;it<dst.size();it++) //将排序好的信息输出
	{
			rawPro<<dst[it].macName<<" "<<dst[it].avgRssi<<" "<<dst[it].num<<" "<<dst[it].score<<endl;
	}
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
