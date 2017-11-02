#include "proMySQL.h" 

//存数据的时候应该单独写个函数，不用很复杂

proMySQL::proMySQL() //默认的数据库test
{
	//初始化数据库  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//初始化数据结构  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//在连接数据库之前，设置额外的连接选项  
	//可以设置的选项很多，这里设置字符集，否则无法处理中文  
	if (0 != mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk"))
		cout << "mysql_options() failed" << endl;  
	if (NULL== mysql_real_connect(&mydata, "localhost", "root", "1234", "test",3307, NULL, 0))   //这里的地址，用户名，密码，端口可以根据自己本地的情况更改     3307针对是64位
		cout << "mysql_real_connect() failed" << endl;  
}

proMySQL::proMySQL(const char *host,const char *user,const char *passwd,const char *db) //非默认的数据库
{
	//初始化数据库  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//初始化数据结构  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//在连接数据库之前，设置额外的连接选项  
	//可以设置的选项很多，这里设置字符集，否则无法处理中文  
	if (0 != mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk"))
		cout << "mysql_options() failed" << endl;  
	if (NULL== mysql_real_connect(&mydata, host, user, passwd, db,3306, NULL, 0))       
		cout << "mysql_real_connect() failed" << endl;  
}

proMySQL::~proMySQL()
{
	mysql_close(&mydata);  
	mysql_server_end();  
}

//构建大表格 包含身份证数据 已经所有数据 包括 身份证 性别 民族 出生 住址 身份证 身份证图片路径 多点碰撞标识符 5个MAC地址(拍好后的) Mac地址存入的路径（日志文件 txt） 外貌信息路径 文件日志
void proMySQL::creBigTable(const char *tableName)
{
	string sqlstr;  
	sqlstr = "CREATE TABLE IF NOT EXISTS ";  
	//表的名称
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "("; 
	sqlstr +=  
		"进入时间  varchar(14)  null PRIMARY KEY COMMENT '进入时间',";  //进入时间 这个是主键，在柜台处应该加入柜台的特殊标识，这样就不会出现问题 如果后续需要有所谓的分析不在本模块范畴，本模块主要负责的是信息的采集和入库
	sqlstr +=  
		"离开时间  varchar(14)  null COMMENT '离开时间',";  //离开时间 //找姜华要时间
	sqlstr +=  
		"身份证号 VARCHAR(20) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '身份证号',"; 
	sqlstr +=  
		"姓名 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Name Of User',";
	sqlstr +=  
		"性别 VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '性别',";
	sqlstr +=  
		"民族 VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '民族编号',";
	sqlstr +=  
		"出生日期 VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '出生时间',"; 
	sqlstr +=  
		"住址 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '住址',"; 
	sqlstr +=  
		"身份证照片 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '证件照片',";//照片存储路径
	sqlstr +=
		"多点碰撞标识 int(5)  NULL COMMENT '多点碰撞标识',";//多点碰撞标识
	//这里应该还需要加入一个多点碰撞的标志位，之前的数据不需要考虑只负责记录仍然保存，后面每次在数据库中查找可能的标志位，从过去的集合中经过分析才得到数据，里面涉及到查询的过程，因此还需要把之前的log进行一个固定格式,以方便后续合并和查询
	sqlstr +=  
		"第一Mac地址 varchar(20) null COMMENT '第一Mac地址',";  //Mac1
	sqlstr +=  
		"第二Mac地址 varchar(20) null COMMENT '第二Mac地址',";  //Mac2
	sqlstr +=  
		"第三Mac地址 varchar(20) null COMMENT '第三Mac地址',";  //Mac3
	sqlstr +=  
		"第四Mac地址 varchar(20) null COMMENT '第四Mac地址',";  //Mac4
	sqlstr +=  
		"第五Mac地址 varchar(20) null COMMENT '第五Mac地址',";  //Mac5
	sqlstr +=  
		"mac存储路径 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Mac地址文件路径',";  //MAC文件路径
	sqlstr +=  
		"外貌存储路径 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '外貌信息文件路径'";  //外貌信息路径
	sqlstr += ");";  
	if (0 != mysql_query(&mydata, sqlstr.c_str())) 
	{  
		cout << "mysql_query() create table failed" << endl;  
		mysql_close(&mydata);  
	} 
}

void proMySQL::dropTable(const char *tableName)
{
	string sqlstr;
	sqlstr = "DROP TABLE ";  
	sqlstr+=tableName;
	sqlstr+=";";
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {  
		cout << "mysql_query() drop table failed" << endl;  
		mysql_close(&mydata);  
	} 
}

void proMySQL::insertBigTabData(const char *tableName,string &sTime,string &eTime,const char *macSavePath,const char *perform,char *idData)
{
	//查询工作 查询的是MAC地址的路径，因为已经存到文件中了，所以直接读取就行,即使身份证不对，在存的时候也一定会有时间的
	int duoDian=0;//这个算更新碰撞的次数，如果没有默认为0 所以需要进行查询
	vector<string> mac;//存储的是最后拍好序的mac地址排列 最后应该有个（）的过程

	//合并工作
	duoDianPengZhuang(mac,duoDian,macSavePath);	
	
	//插入工作
	string sqlstr="INSERT INTO ";   
	sqlstr+=tableName;
	sqlstr+=" VALUES(";
	sqlstr+=sTime;//进入
	sqlstr+=",";
	sqlstr+=eTime;//离开

	//身份证 姓名 性别 民族 出生日期 住址 照片路径
	sqlstr+=", '";
	sqlstr+=string(idData+148).size()!=0?string(idData+148):"";
	sqlstr+="'";

	sqlstr+=", '";
	sqlstr+=string(idData).size()!=0?string(idData):"";
	sqlstr+="'";

	sqlstr+=", '";
	sqlstr+=string(idData+32).size()!=0?string(idData+32):"";
	sqlstr+="'";

	sqlstr+=", '";
	sqlstr+=string(idData+38).size()!=0?string(idData+38):"";
	sqlstr+="'";

	sqlstr+=", '";
	sqlstr+=string(idData+58).size()!=0?string(idData+58):"";
	sqlstr+="'";

	sqlstr+=", '";
	sqlstr+=string(idData+76).size()!=0?string(idData+76):"";
	sqlstr+="'";

	sqlstr+=", '";
	//sqlstr+=string(idData+292).size()!=0?transPath(string(idData+292)):""; //这里需要加入一个函数处理下string chuli(string src);反正不长，慢就慢点 原始存储路径 
	sqlstr+=string(idData+292).size()!=0?("F://处理后//身份数据//"+string(idData+148)+".bmp"):""; //转移后的路径
	sqlstr+="'";

	//添加多点碰撞索引
	sqlstr+=", '";
	sqlstr+=intToString(duoDian-1); 
	sqlstr+="'";

	for(int i=0;i<5;i++)//这里应该是多点碰撞后的的MAC地址
	{
		sqlstr+=", '";
		sqlstr+=i<mac.size()?mac[i]:"";
		sqlstr+="' ";
	}

	sqlstr+=", '";//mac地址路径
	sqlstr+=macSavePath;
	sqlstr+="'";

	sqlstr+=", '";//外貌信息路径
	sqlstr+=perform;
	sqlstr+="'";

	sqlstr+=");";
	if (0 != mysql_query(&mydata, sqlstr.c_str())) {   
		cout << "mysql_query() insert data failed" << endl;  
		mysql_close(&mydata);  
	}  
}

string proMySQL::timeToString(char timeData[14])//time转string 函数
{
	char timeDataEd[15];
	memset(timeDataEd,0,sizeof(char)*15);
	memcpy(timeDataEd,timeData,sizeof(char)*14);
	stringstream stream;
	stream<<timeDataEd;
	return stream.str();
}

string proMySQL::transPath(string &src) //路径转换函数
{
	std::string::size_type pos = 0;
	
	while ((pos=src.find('\\',pos))!=std::string::npos)
	{
		src.replace(pos,1,"//");
		pos+=2;
	}
	return src;
}

int proMySQL::stringToInt(string x)
{
	int temp;
	stringstream stream;
	stream<<x;
	stream>>temp;
	return temp;
}

string proMySQL::intToString(int x)//int 转 string 函数
{
	stringstream stream;
	stream<<x;
	return stream.str();
}


void proMySQL::duoDianPengZhuang(vector<string> &mac,int &num,const char *macSavePath)
{
	//要做一个查询工作，其实不用再涉及数据库的查询，直接在文件中就可以做到 查询文件的大小然后再更新duoDian的数据
	//要使用Map<string,double>来完成文件的更新
	//先求所有分数的和，然后将所有数据除以个数，然后再通过一个sort排序
	//文件外的查询工作可以通过数据库来实现
	num=-1;
	//打开文件
	ifstream Macfile(macSavePath);
	string fileStr;
	map<string,double> macList;
	while (!Macfile.eof())//检查一下 以后
	{
		num++;
	/*	cout<<num<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!次数！！！！！！！！！！！！！！！！"<<endl;*/
		getline(Macfile,fileStr);
		istringstream tmpCin(fileStr);
		string timeString;
		int tmpNum=0;
		tmpCin>>timeString>>tmpNum;
		//确定格式
		string tempMac,tempRSSI;
		double tempScore=0.0;
		for (int i=0;i<tmpNum;i++)
		{
			getline(Macfile,fileStr);
			istringstream tmpCin2(fileStr);
			tmpCin2>>tempMac>>tempRSSI>>tempRSSI;
			if (num==0)//第一次只是赋值
			{
				macList[tempMac]=tempScore;
			}else
			{
				macList[tempMac]=macList[tempMac]+tempScore;
			}
		}
	}
	vector<mapDuodian> tempMap;
	for (auto &i:macList)
	{
		i.second=i.second/(num+1);
		mapDuodian temp;
		temp.mac=i.first;
		temp.score=i.second;
		tempMap.push_back(temp);
	}
	sort(tempMap.begin(),tempMap.end(),[](mapDuodian s1,mapDuodian s2){return s1.score>s2.score;});
	for (auto i:tempMap)
	{
		mac.push_back(i.mac);
	}
	//关闭文件
	Macfile.close();
}
