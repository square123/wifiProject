#pragma once
#include <winsock.h>  
#include <iostream>  
#include <mysql.h> 
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <map>
#include <cmath>
#include <numeric>
using namespace std;  

class proMySQL
{
private:
	MYSQL mydata; //数据库的操作

	string timeToString(char timeData[14]);//time转string 函数

public:
	proMySQL();
	proMySQL(const char *host,const char *user,const char *passwd,const char *db);
	~proMySQL();
	struct mapDuodian
	{
		string mac;
		double score;
	};
	//构建表格
	//包含身份证数据 已经所有数据 包括 身份证 性别 民族 出生 住址 身份证 身份证图片路径 多点碰撞标识符 5个MAC地址(拍好后的) Mac地址存入的路径（日志文件 txt） 外貌信息路径 文件日志
	void creBigTable(const char *tableName); //大的表格是为了整合整个项目
	//删除表格
	void dropTable(const char *tableName);

	//插入数据 分析下（具体怎么实现） 通过内存的方式实现， 先将所有读到的数据放在内存中，然后在离开环境后将内存结束
	void insertBigTabData(const char *tableName,string &sTimeEnter,string &sTimeLeave,const char *macSavePath,const char *perform,char *idData); //传参应该包含身份证读卡器的证件信息格式，自己的数据格式，两个路径

	//（默认是目标人物彻底离开后，才完成所有的数据）

	//Mac地址的存储，只是存储（因为已经完成了分析）  （多点碰撞分析机制）现在先实现一个比较容易的机制可以以后再实现比较合适的算法 (具体的算法以后再考虑)

	//多点碰撞的机制 设计
private:
	//转换格式的小函数
	string transPath(string &src);//用来将\将转换为//
	//分析的函数 就是多点碰撞的小机制
	int stringToInt(string x);
	string intToString(int x);//int 转 string 函数
	void duoDianPengZhuang(vector<string> &mac,int &num,const char *macSavePath);//输出的MAC地址，输出的多点碰撞次数，文件路径

};



