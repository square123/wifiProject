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

//构建小表格  时间 推荐的MAC地址 前五位 以及存储的路径
void proMySQL::creSmallTable(const char *tableName)
{
	string sqlstr;  
	sqlstr = "CREATE TABLE IF NOT EXISTS ";  
	//表的名称
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "(";  
	sqlstr +=  
		"进入时间  varchar(14)  null,";  //时间
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
		"mac存储路径 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Mac地址文件路径'";  //MAC文件路径
	sqlstr += ");";  
	if (0 != mysql_query(&mydata, sqlstr.c_str())) 
	{  
		cout << "mysql_query() create table failed" << endl;  
		mysql_close(&mydata);  
	} 
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
		"进入时间  varchar(14)  null,";  //进入时间
	sqlstr +=  
		"离开时间  varchar(14)  null,";  //离开时间
	sqlstr +=  
		"身份证号 VARCHAR(20) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL PRIMARY KEY COMMENT '身份证号',"; 
	sqlstr +=  
		"姓名 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Name Of User',";
	sqlstr +=  
		"性别 VARCHAR(2) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '性别',";
	sqlstr +=  
		"民族 VARCHAR(2) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '民族',";
	sqlstr +=  
		"出生日期 VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '出生时间',"; 
	sqlstr +=  
		"住址 VARCHAR(30) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '住址',"; 
	sqlstr +=  
		"身份证照片 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '照片',";
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
		"mac存储路径 VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Mac地址文件路径'";  //MAC文件路径
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

void proMySQL::insertSmaTabData(const char *tableName,string &sTime,vector<string> &mac,const char *macSavePath)
{
	string sqlstr="INSERT INTO ";   //还需要测试下
	sqlstr+=tableName;
	sqlstr+=" VALUES(";
	sqlstr+=sTime;
	for(int i=0;i<5;i++)
	{
		sqlstr+=", '";
		if (i<mac.size())
		{
			sqlstr+=mac[i];
		}else
		{
			sqlstr+="";
		}
		sqlstr+="' ";
	}
	sqlstr+=", '";
	sqlstr+=macSavePath;
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