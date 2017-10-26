#include "proMySQL.h" 

//�����ݵ�ʱ��Ӧ�õ���д�����������úܸ���

proMySQL::proMySQL() //Ĭ�ϵ����ݿ�test
{
	//��ʼ�����ݿ�  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//��ʼ�����ݽṹ  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//���������ݿ�֮ǰ�����ö��������ѡ��  
	//�������õ�ѡ��ܶ࣬���������ַ����������޷���������  
	if (0 != mysql_options(&mydata, MYSQL_SET_CHARSET_NAME, "gbk"))
		cout << "mysql_options() failed" << endl;  
	if (NULL== mysql_real_connect(&mydata, "localhost", "root", "1234", "test",3307, NULL, 0))   //����ĵ�ַ���û��������룬�˿ڿ��Ը����Լ����ص��������     3307�����64λ
		cout << "mysql_real_connect() failed" << endl;  
}

proMySQL::proMySQL(const char *host,const char *user,const char *passwd,const char *db) //��Ĭ�ϵ����ݿ�
{
	//��ʼ�����ݿ�  
	if (0 != mysql_library_init(0, NULL, NULL))  
		cout << "mysql_library_init() failed" << endl;  
	//��ʼ�����ݽṹ  
	if (NULL == mysql_init(&mydata))
		cout << "mysql_init() failed" << endl;  
	//���������ݿ�֮ǰ�����ö��������ѡ��  
	//�������õ�ѡ��ܶ࣬���������ַ����������޷���������  
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

//����С���  ʱ�� �Ƽ���MAC��ַ ǰ��λ �Լ��洢��·��
void proMySQL::creSmallTable(const char *tableName)
{
	string sqlstr;  
	sqlstr = "CREATE TABLE IF NOT EXISTS ";  
	//�������
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "(";  
	sqlstr +=  
		"����ʱ��  varchar(14)  null,";  //ʱ��
	sqlstr +=  
		"��һMac��ַ varchar(20) null COMMENT '��һMac��ַ',";  //Mac1
	sqlstr +=  
		"�ڶ�Mac��ַ varchar(20) null COMMENT '�ڶ�Mac��ַ',";  //Mac2
	sqlstr +=  
		"����Mac��ַ varchar(20) null COMMENT '����Mac��ַ',";  //Mac3
	sqlstr +=  
		"����Mac��ַ varchar(20) null COMMENT '����Mac��ַ',";  //Mac4
	sqlstr +=  
		"����Mac��ַ varchar(20) null COMMENT '����Mac��ַ',";  //Mac5
	sqlstr +=  
		"mac�洢·�� VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Mac��ַ�ļ�·��'";  //MAC�ļ�·��
	sqlstr += ");";  
	if (0 != mysql_query(&mydata, sqlstr.c_str())) 
	{  
		cout << "mysql_query() create table failed" << endl;  
		mysql_close(&mydata);  
	} 
}

//�������� �������֤���� �Ѿ��������� ���� ���֤ �Ա� ���� ���� סַ ���֤ ���֤ͼƬ·�� �����ײ��ʶ�� 5��MAC��ַ(�ĺú��) Mac��ַ�����·������־�ļ� txt�� ��ò��Ϣ·�� �ļ���־
void proMySQL::creBigTable(const char *tableName)
{
	string sqlstr;  
	sqlstr = "CREATE TABLE IF NOT EXISTS ";  
	//�������
	sqlstr +=tableName;
	sqlstr +=" ";
	sqlstr += "("; 
	sqlstr +=  
		"����ʱ��  varchar(14)  null,";  //����ʱ��
	sqlstr +=  
		"�뿪ʱ��  varchar(14)  null,";  //�뿪ʱ��
	sqlstr +=  
		"���֤�� VARCHAR(20) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL PRIMARY KEY COMMENT '���֤��',"; 
	sqlstr +=  
		"���� VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Name Of User',";
	sqlstr +=  
		"�Ա� VARCHAR(2) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '�Ա�',";
	sqlstr +=  
		"���� VARCHAR(2) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '����',";
	sqlstr +=  
		"�������� VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '����ʱ��',"; 
	sqlstr +=  
		"סַ VARCHAR(30) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'סַ',"; 
	sqlstr +=  
		"���֤��Ƭ VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '��Ƭ',";
	sqlstr +=  
		"��һMac��ַ varchar(20) null COMMENT '��һMac��ַ',";  //Mac1
	sqlstr +=  
		"�ڶ�Mac��ַ varchar(20) null COMMENT '�ڶ�Mac��ַ',";  //Mac2
	sqlstr +=  
		"����Mac��ַ varchar(20) null COMMENT '����Mac��ַ',";  //Mac3
	sqlstr +=  
		"����Mac��ַ varchar(20) null COMMENT '����Mac��ַ',";  //Mac4
	sqlstr +=  
		"����Mac��ַ varchar(20) null COMMENT '����Mac��ַ',";  //Mac5
	sqlstr +=  
		"mac�洢·�� VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Mac��ַ�ļ�·��'";  //MAC�ļ�·��
	sqlstr +=  
		"��ò�洢·�� VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '��ò��Ϣ�ļ�·��'";  //��ò��Ϣ·��
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
	string sqlstr="INSERT INTO ";   //����Ҫ������
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

string proMySQL::timeToString(char timeData[14])//timeתstring ����
{
	char timeDataEd[15];
	memset(timeDataEd,0,sizeof(char)*15);
	memcpy(timeDataEd,timeData,sizeof(char)*14);
	stringstream stream;
	stream<<timeDataEd;
	return stream.str();
}