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
		"����ʱ��  varchar(14)  null PRIMARY KEY COMMENT '����ʱ��',";  //����ʱ�� ������������ڹ�̨��Ӧ�ü����̨�������ʶ�������Ͳ���������� ���������Ҫ����ν�ķ������ڱ�ģ�鷶�룬��ģ����Ҫ���������Ϣ�Ĳɼ������
	sqlstr +=  
		"�뿪ʱ��  varchar(14)  null COMMENT '�뿪ʱ��',";  //�뿪ʱ�� //�ҽ���Ҫʱ��
	sqlstr +=  
		"���֤�� VARCHAR(20) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '���֤��',"; 
	sqlstr +=  
		"���� VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Name Of User',";
	sqlstr +=  
		"�Ա� VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '�Ա�',";
	sqlstr +=  
		"���� VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '������',";
	sqlstr +=  
		"�������� VARCHAR(10) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '����ʱ��',"; 
	sqlstr +=  
		"סַ VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'סַ',"; 
	sqlstr +=  
		"���֤��Ƭ VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT '֤����Ƭ',";//��Ƭ�洢·��
	sqlstr +=
		"�����ײ��ʶ int(5)  NULL COMMENT '�����ײ��ʶ',";//�����ײ��ʶ
	//����Ӧ�û���Ҫ����һ�������ײ�ı�־λ��֮ǰ�����ݲ���Ҫ����ֻ�����¼��Ȼ���棬����ÿ�������ݿ��в��ҿ��ܵı�־λ���ӹ�ȥ�ļ����о��������ŵõ����ݣ������漰����ѯ�Ĺ��̣���˻���Ҫ��֮ǰ��log����һ���̶���ʽ,�Է�������ϲ��Ͳ�ѯ
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
		"mac�洢·�� VARCHAR(100) CHARACTER SET gb2312 COLLATE gb2312_chinese_ci NULL COMMENT 'Mac��ַ�ļ�·��',";  //MAC�ļ�·��
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

void proMySQL::insertBigTabData(const char *tableName,string &sTime,string &eTime,const char *macSavePath,const char *perform,char *idData)
{
	//��ѯ���� ��ѯ����MAC��ַ��·������Ϊ�Ѿ��浽�ļ����ˣ�����ֱ�Ӷ�ȡ����,��ʹ���֤���ԣ��ڴ��ʱ��Ҳһ������ʱ���
	int duoDian=0;//����������ײ�Ĵ��������û��Ĭ��Ϊ0 ������Ҫ���в�ѯ
	vector<string> mac;//�洢��������ĺ����mac��ַ���� ���Ӧ���и������Ĺ���

	//�ϲ�����
	duoDianPengZhuang(mac,duoDian,macSavePath);	
	
	//���빤��
	string sqlstr="INSERT INTO ";   
	sqlstr+=tableName;
	sqlstr+=" VALUES(";
	sqlstr+=sTime;//����
	sqlstr+=",";
	sqlstr+=eTime;//�뿪

	//���֤ ���� �Ա� ���� �������� סַ ��Ƭ·��
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
	//sqlstr+=string(idData+292).size()!=0?transPath(string(idData+292)):""; //������Ҫ����һ������������string chuli(string src);������������������ ԭʼ�洢·�� 
	sqlstr+=string(idData+292).size()!=0?("F://�����//�������//"+string(idData+148)+".bmp"):""; //ת�ƺ��·��
	sqlstr+="'";

	//��Ӷ����ײ����
	sqlstr+=", '";
	sqlstr+=intToString(duoDian-1); 
	sqlstr+="'";

	for(int i=0;i<5;i++)//����Ӧ���Ƕ����ײ��ĵ�MAC��ַ
	{
		sqlstr+=", '";
		sqlstr+=i<mac.size()?mac[i]:"";
		sqlstr+="' ";
	}

	sqlstr+=", '";//mac��ַ·��
	sqlstr+=macSavePath;
	sqlstr+="'";

	sqlstr+=", '";//��ò��Ϣ·��
	sqlstr+=perform;
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

string proMySQL::transPath(string &src) //·��ת������
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

string proMySQL::intToString(int x)//int ת string ����
{
	stringstream stream;
	stream<<x;
	return stream.str();
}


void proMySQL::duoDianPengZhuang(vector<string> &mac,int &num,const char *macSavePath)
{
	//Ҫ��һ����ѯ��������ʵ�������漰���ݿ�Ĳ�ѯ��ֱ�����ļ��оͿ������� ��ѯ�ļ��Ĵ�СȻ���ٸ���duoDian������
	//Ҫʹ��Map<string,double>������ļ��ĸ���
	//�������з����ĺͣ�Ȼ���������ݳ��Ը�����Ȼ����ͨ��һ��sort����
	//�ļ���Ĳ�ѯ��������ͨ�����ݿ���ʵ��
	num=-1;
	//���ļ�
	ifstream Macfile(macSavePath);
	string fileStr;
	map<string,double> macList;
	while (!Macfile.eof())//���һ�� �Ժ�
	{
		num++;
	/*	cout<<num<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!������������������������������������"<<endl;*/
		getline(Macfile,fileStr);
		istringstream tmpCin(fileStr);
		string timeString;
		int tmpNum=0;
		tmpCin>>timeString>>tmpNum;
		//ȷ����ʽ
		string tempMac,tempRSSI;
		double tempScore=0.0;
		for (int i=0;i<tmpNum;i++)
		{
			getline(Macfile,fileStr);
			istringstream tmpCin2(fileStr);
			tmpCin2>>tempMac>>tempRSSI>>tempRSSI;
			if (num==0)//��һ��ֻ�Ǹ�ֵ
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
	//�ر��ļ�
	Macfile.close();
}
