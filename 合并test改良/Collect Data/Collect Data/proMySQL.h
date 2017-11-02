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
	MYSQL mydata; //���ݿ�Ĳ���

	string timeToString(char timeData[14]);//timeתstring ����

public:
	proMySQL();
	proMySQL(const char *host,const char *user,const char *passwd,const char *db);
	~proMySQL();
	struct mapDuodian
	{
		string mac;
		double score;
	};
	//�������
	//�������֤���� �Ѿ��������� ���� ���֤ �Ա� ���� ���� סַ ���֤ ���֤ͼƬ·�� �����ײ��ʶ�� 5��MAC��ַ(�ĺú��) Mac��ַ�����·������־�ļ� txt�� ��ò��Ϣ·�� �ļ���־
	void creBigTable(const char *tableName); //��ı����Ϊ������������Ŀ
	//ɾ�����
	void dropTable(const char *tableName);

	//�������� �����£�������ôʵ�֣� ͨ���ڴ�ķ�ʽʵ�֣� �Ƚ����ж��������ݷ����ڴ��У�Ȼ�����뿪�������ڴ����
	void insertBigTabData(const char *tableName,string &sTimeEnter,string &sTimeLeave,const char *macSavePath,const char *perform,char *idData); //����Ӧ�ð������֤��������֤����Ϣ��ʽ���Լ������ݸ�ʽ������·��

	//��Ĭ����Ŀ�����ﳹ���뿪�󣬲�������е����ݣ�

	//Mac��ַ�Ĵ洢��ֻ�Ǵ洢����Ϊ�Ѿ�����˷�����  �������ײ�������ƣ�������ʵ��һ���Ƚ����׵Ļ��ƿ����Ժ���ʵ�ֱȽϺ��ʵ��㷨 (������㷨�Ժ��ٿ���)

	//�����ײ�Ļ��� ���
private:
	//ת����ʽ��С����
	string transPath(string &src);//������\��ת��Ϊ//
	//�����ĺ��� ���Ƕ����ײ��С����
	int stringToInt(string x);
	string intToString(int x);//int ת string ����
	void duoDianPengZhuang(vector<string> &mac,int &num,const char *macSavePath);//�����MAC��ַ������Ķ����ײ�������ļ�·��

};



