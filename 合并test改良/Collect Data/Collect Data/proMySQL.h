#pragma once
#include <winsock.h>  
#include <iostream>  
#include <mysql.h> 
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <numeric>
using namespace std;  

class proMySQL
{
private:
	MYSQL mydata; //���ݿ�Ĳ���

	string timeToString(char timeData[14]);//timeתstring ����

public:
	proMySQL();
	proMySQL(const char *host,const char *user,const char *passwd,const char *db);
	~proMySQL();

	//�������
	
	//С�ı����Ϊ�˲����Լ��ĳ����� //�Ժ����ɾ����
	void creSmallTable(const char *tableName);//����С���  ֻ�������������� ���ж�Ӧ�洢ͼƬ��·�� �ļ���ʽ
	//�������� �������֤���� �Ѿ��������� ���� ���֤ �Ա� ���� ���� סַ ���֤ ���֤ͼƬ·�� �����ײ��ʶ�� 5��MAC��ַ(�ĺú��) Mac��ַ�����·������־�ļ� txt�� ��ò��Ϣ·�� �ļ���־
	void creBigTable(const char *tableName); //��ı����Ϊ������������Ŀ
	//ɾ�����
	void dropTable(const char *tableName);

	//�������� �����£�������ôʵ�֣� ͨ���ڴ�ķ�ʽʵ�֣� �Ƚ����ж��������ݷ����ڴ��У�Ȼ�����뿪�������ڴ����
	void insertSmaTabData(const char *tableName,string &sTime,vector<string> &mac,const char *macSavePath);
	void insertBigTabData(const char *tableName,string &sTime,vector<string> &mac,const char *macSavePath,const char *perform,char *idData); //����Ӧ�ð������֤��������֤����Ϣ��ʽ���Լ������ݸ�ʽ������·��

	//��Ĭ����Ŀ�����ﳹ���뿪�󣬲�������е����ݣ�

	//Mac��ַ�Ĵ洢��ֻ�Ǵ洢����Ϊ�Ѿ�����˷�����  �������ײ�������ƣ�������ʵ��һ���Ƚ����׵Ļ��ƿ����Ժ���ʵ�ֱȽϺ��ʵ��㷨 (������㷨�Ժ��ٿ���)






};



