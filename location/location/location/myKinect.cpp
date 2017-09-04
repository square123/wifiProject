#include "myKinect.h"

#define r1x (-2.29144)
#define r1z (2.12263)
#define r2x (1.25144)
#define r2z (2.32463)
#define r3x (0.518092)
#define r3z (-1.182477)
#define r4x (-0.00761917)
#define r4z (-1.50423)
Kinect::Kinect()  
{
	pColorFrame = nullptr;
	pDepthFrame = nullptr;
	pBodyIndexFrame=nullptr;
	pBodyFrame = nullptr;
	colorHalfSizeMat.create(ColorHeight/2, ColorWidth/2, CV_8UC4);
	colorMat.create( ColorHeight, ColorWidth, CV_8UC4);
	depthMat16.create(DepthHeight, DepthWidth, CV_16UC1);
	depthMat8.create(DepthHeight, DepthWidth, CV_8UC1 );
	depthDeTemp.create(DepthHeight, DepthWidth, CV_16UC1);
	bodyIndexMat.create(DepthHeight, DepthWidth, CV_8UC3);
	bodyIndexMatInt.create(DepthHeight, DepthWidth, CV_8UC1);
	//showOrbit�����ʼ��
	double maxDis=cos(-(rotateAngle)/180*PI)*4.5;
	double maxWid=2*maxDis*tan(35.0/180*PI);
	double minDis=cos(-(rotateAngle)/180*PI)*0.5;//KinectHeight*tan((60.0+(rotateAngle))/180*PI);
	double minWid=2*minDis*tan(35.0/180*PI);
	sOX=int(ceil(maxWid)*100),sOY=int(ceil(maxDis)*100);
	showOrbit=Mat::zeros(sOY,sOX,CV_8UC3);//����Ϊȫ��
	CoordinateMapperMat.create(DepthHeight,DepthWidth,CV_8UC4);
	CoordinateMapperMat_TM.create(DepthHeight,DepthWidth,CV_8UC1);

	//����ɫ����
	line(showOrbit,Point(sOX/2-(int(maxWid*100)/2),sOY-int(maxDis*100)),Point(sOX/2+(int(maxWid*100)/2),sOY-int(maxDis*100)),Scalar(255,255,255),2,8);
	line(showOrbit,Point(sOX/2+(int(maxWid*100)/2),sOY-int(maxDis*100)),Point(sOX/2+(int(minWid*100)/2),sOY-int(minDis*100)),Scalar(255,255,255),2,8);
	line(showOrbit,Point(sOX/2+(int(minWid*100)/2),sOY-int(minDis*100)),Point(sOX/2-(int(minWid*100)/2),sOY-int(minDis*100)),Scalar(255,255,255),2,8);
	line(showOrbit,Point(sOX/2-(int(minWid*100)/2),sOY-int(minDis*100)),Point(sOX/2-(int(maxWid*100)/2),sOY-int(maxDis*100)),Scalar(255,255,255),2,8);
	floodFill(showOrbit,Point(sOX/2,sOY/2),Scalar(255,255,255));
	saveTmpIntClr=0;
	saveTmpIntDep=0;
	processIndex=0;
	memset(kinectTimeRaw,0,sizeof(kinectTimeRaw));
	memset(kinectTIIndex,0,sizeof(kinectTIIndex));

	for (int i=0;i<6;i++) //�Ƚ������ÿգ���������Ϊ30������ʹ���������һЩ
	{
		storeTemp[i].magicNumber=30;
		storeTemp[i].strIndex=-1; //������-1����û�и���
	}
	//memset(&del,0,sizeof(del));
	deee.open("deee.txt",ios::app);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	histElement=getStructuringElement(MORPH_ELLIPSE,Size(5,5));
	memset(trackStateHold,-1,sizeof(trackStateHold)); //-1����ʾ����û�и��ٵ���1��������ٵ��ˣ�������һ������������֮ǰ������״̬���Ӷ�������ͺ�һЩ
}

Kinect::~Kinect()
{
	outfile.close();
	SafeRelease( pDepthSource );
	SafeRelease( pDepthReader );
	SafeRelease( pDepthDescription );
	SafeRelease( pColorSource );
	SafeRelease( pColorReader );
	SafeRelease( pColorDescription );
	SafeRelease( pCoordinateMapper );
	SafeRelease( pBodyIndexSource );
	SafeRelease( pBodyIndexReader );
	SafeRelease( pBodySource );
	SafeRelease( pBodyReader );
	if ( pSensor )
	{
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();
	deee.close();//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

}

HRESULT Kinect::InitKinect()
{
	hResult = GetDefaultKinectSensor( &pSensor );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:GetDefaultKinectSensor" <<endl;
		return -1;
	}
	hResult = pSensor->Open();
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::Open()" <<endl;
		return -1;
	}
	time_t syscTimee=time(NULL);	//����ִֻ��һ�Σ������Ͳ���Ҫ�ٲ�����
	processIndex=syscTimee+4;//��Ϊ��Ҫ�ͺ�������Ҫ������ı�־λ�ͺ󣬾�����ͺ�ʱ����������޸�
}

HRESULT Kinect::InitColor()//������InitKinect()
{
	hResult = pSensor->get_ColorFrameSource( &pColorSource );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_ColorFrameSource()"<<endl;
		return -1;
	}
	hResult = pColorSource->OpenReader( &pColorReader );     
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
		return -1; 
	}

}

HRESULT Kinect::InitDepth()//������InitKinect()
{
	hResult = pSensor->get_DepthFrameSource( &pDepthSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_DepthFrameSource()"<<endl;
		return -1;
	}
	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
		return -1;
	}
}

HRESULT Kinect::InitCoorMap()
{
	hResult = pSensor->get_CoordinateMapper( &pCoordinateMapper );     
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_CoordinateMapper()"<<endl;
		return -1;
	}
}

HRESULT Kinect::Initbody()
{

	hResult = pSensor->get_BodyFrameSource( &pBodySource );
	if( FAILED( hResult ) ){
		std::cerr << "Error : IKinectSensor::get_BodyFrameSource()" << std::endl;
		return -1;
	}
	hResult = pBodySource->OpenReader( &pBodyReader ); 
	if( FAILED( hResult ) ){
		std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
		return -1;
	}
}

void Kinect::depthProcess()//16λ��ʾ�ᱨ�� ���øú�������
{
	unsigned int depthBufferSize = DepthWidth * DepthHeight * sizeof( unsigned short );
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&depthBufferSize, reinterpret_cast<UINT16**>(&depthMat16.data));
		if ( SUCCEEDED(hResult) )
		{
			depthMat16.convertTo(depthMat8, CV_8U, -255.0f/8000.0f, 255.0f );
		}

	}
	SafeRelease( pDepthFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
}

void Kinect::depthProcess2()
{
	UINT16 *pBuffer = NULL;
	UINT nBufferSize = 0; 
	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame );
	if ( SUCCEEDED(hResult) )
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);
		for(int i=0;i<DepthHeight;i++)
		{
			ushort* ptrDepth16 = depthMat16.ptr<ushort>(i);  // 16
			uchar* ptrDepth8 = depthMat8.ptr<uchar>(i);  //8
			for(int j=0;j<DepthWidth;j++)
			{
				USHORT depth = *pBuffer;  //16   �������ֽ�Ϊ��λ���ж�ȡ
				ptrDepth16[j] = depth;        //ֱ�Ӵ洢��ʾ
				ptrDepth8[j] = depth%256; //תΪ8λ�洢��ʾ
				pBuffer++;               //16   �������ֽ�Ϊ��λ���ж�ȡ
			}
		}
	}
	SafeRelease( pDepthFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
}

void Kinect::colorProcess()
{
	unsigned int colorBufferSize = ColorWidth * ColorHeight * 4 * sizeof( unsigned char );
	hResult = pColorReader->AcquireLatestFrame( &pColorFrame );      
	if ( SUCCEEDED(hResult) )
	{
		hResult = pColorFrame->CopyConvertedFrameDataToArray( colorBufferSize,
			reinterpret_cast<BYTE*> (colorMat.data), ColorImageFormat::ColorImageFormat_Bgra );
		if ( SUCCEEDED(hResult) )
		{
			resize( colorMat,colorHalfSizeMat , Size(), 0.5, 0.5 );
		}
	}
	SafeRelease( pColorFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����
}

void Kinect::kincetSave(Mat a,string savePath,int opt)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	char optTemp[2];
	sprintf_s(optTemp,sizeof(optTemp),"%d",opt);
	string savePathTemp=savePath;
	switch (opt)
	{
	case saveDepth:
		{
			if( GetKeyState( VK_NUMPAD1 ) < 0 ){
				sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%03d",saveTmpIntDep); 
				savePathTemp = savePathTemp +  optTemp +"_"+ saveTmpStrDep + ".png";
				imwrite(savePathTemp,a);//�洢ͼ��
				saveTmpIntDep++;   
				savePathTemp =savePath;
			}
			break;
		}
	case saveColor:
		{
			if( GetKeyState( VK_NUMPAD2 ) < 0 ){
				sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
				savePathTemp = savePathTemp + optTemp +"_"+ saveTmpStrClr + ".png";
				imwrite(savePathTemp,a);//�洢ͼ��
				saveTmpIntClr++;   
				savePathTemp =savePath;
			}
			break;
		}
	}
}

void Kinect::kinectSaveAll(string savePath)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(0);
	string savePathTemp=savePath;
	if( GetKeyState( VK_RETURN) < 0 ){
		sprintf_s(saveTmpStrDep,sizeof(saveTmpStrDep),"%03d",saveTmpIntDep); 
		savePathTemp = savePathTemp +"depth" +"_"+ saveTmpStrDep + ".png";
		imwrite(savePathTemp,depthDeTemp);//�洢ȥ������ͼ��
		//imwrite(savePathTemp,depthMat8);//�洢ȥ������ͼ��
		saveTmpIntDep++;   
		savePathTemp =savePath;
		sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
		savePathTemp = savePathTemp+"color" +"_"+ saveTmpStrClr + ".png";
		imwrite(savePathTemp,colorHalfSizeMat);//�洢��ɫͼ��
		saveTmpIntClr++;   
		savePathTemp =savePath;
	}
}

void Kinect::bodyLocation()
{
	cv::Vec3b color[BODY_COUNT];      //#define BODY_COUNT=6
	color[0] = cv::Vec3b( 255,   0,   0 );
	color[1] = cv::Vec3b(   0, 255,   0 );
	color[2] = cv::Vec3b(   0,   0, 255 );
	color[3] = cv::Vec3b( 255, 255,   0 );
	color[4] = cv::Vec3b( 255,   0, 255 );
	color[5] = cv::Vec3b(   0, 255, 255 );
	pBodyFrame = nullptr;
	time_t systime=time(NULL);//����ϵͳʱ��
	char timeFix[16];
	strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&systime));//ʱ���޸�
	hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
	if( SUCCEEDED( hResult ) )
	{
		IBody* pBody[BODY_COUNT] = { 0 }; //Ĭ�ϵ��� 6 ������ ����ʼ�����еĹ�����Ϣ
		hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );//���¹������ݣ�
		if( SUCCEEDED( hResult ) ){
			for( int count = 0; count < BODY_COUNT; count++ ){  //count����0��5��6�����ҹ�������ʾ
				int histFlag=0;//�������Ϊ������¼�ı�ʶ��
				BOOLEAN bTracked = false;    //��ʼ������׷�ٵ����塱��ֵΪ��
				hResult = pBody[count]->get_IsTracked( &bTracked );  //ȷ����׷�ٵ����塣
				//����������¼�Ĳ���
				if (trackStateHold[count]==(bTracked==false?-1:1))
				{
					histFlag=1;
				}else
				{
					trackStateHold[count]=(bTracked==false?-1:1);
				}
				if( SUCCEEDED( hResult ) && bTracked && histFlag ){
					//Ӧ��������������һ��״̬λ�ı��֣������ͣ��һ�£���������ϴε�״̬��ͬ�ͱ��֣������½���һ�����飬�������б���֮ǰ��״̬�����״̬��ԭ����ͬ����ͣ��һ�²�������´�����������
					Joint joint[JointType::JointType_Count];   //ȡ������Joint(�ؽ�)��JointType��һ��ö�����ͣ���ͬλ�õĹؽڵ㶼�ǲ�ͬ�ı�ű�ʾ�ġ�count��һ����ֵ25��
					hResult = pBody[ count ]->GetJoints( JointType::JointType_Count, joint );  //ȡ������Joint(�ؽ�)��
					//cout<<"����Ϊ"<<count<<"�����꣺ "<<"X="<<joint[ JointType_SpineMid].Position.X<<" "<<"Y="<<joint[ JointType_SpineMid].Position.Y<<" "<<"Z="<<joint[ JointType_SpineMid].Position.Z<<endl;
					//����任,ֻ����x�������нǶ�
					double xx,yy,zz;
					xx=joint[ JointType_SpineMid].Position.X;
					yy=(cos(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Y+sin(rotateAngle/180*PI)*joint[JointType_SpineMid].Position.Z);
					zz=((-1)*sin(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Y+cos(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Z);
					//cout<<timeFix<<" "<<"x="<<-xx<<"z="<<(zz-(sOY/200))<<endl;
					double r4=sqrt((-xx-r4x)*(-xx-r4x)+((zz-(sOY/200))-r4z)*((zz-(sOY/200))-r4z));
					double r3=sqrt((-xx-r3x)*(-xx-r3x)+((zz-(sOY/200))-r3z)*((zz-(sOY/200))-r3z));
					double r2=sqrt((-xx-r2x)*(-xx-r2x)+((zz-(sOY/200))-r2z)*((zz-(sOY/200))-r2z));
					double r1=sqrt((-xx-r1x)*(-xx-r1x)+((zz-(sOY/200))-r1z)*((zz-(sOY/200))-r1z));
					//�����������
						//���ݴ�����
					cout<<reNewIndex(count)<<endl;
					kinectDataRawProecess(timeFix,count,r1,r2,r3,r4);
					if (systime>=processIndex)//���в�ղ��� ���մﵽѭ������,�ĳɴ��ڵ�������Ϊ�����п��ܴ�������
					{
						kinectDataProProecess();
						processIndex++;
					}
					//������ʾ����
					float a,b;
					a=xx*100;
					b=zz*100;
					Point center;
					center.x=sOX/2-int(a);
					center.y=sOY-int(b);
					circle(showOrbit,center,1,static_cast< cv::Scalar >( color[count] ),-1,CV_AA);
				}
			}
		}
		for( int count = 0; count < BODY_COUNT; count++ ){   //ѭ����ȫ�ͷ�6����Ϊbody��ָ��
			SafeRelease( pBody[count] );
		}
	}
	SafeRelease( pBodyFrame );  //���frameһ��Ҫ�ͷţ���Ȼ�޷�����	
}

void Kinect::kinectDataRawProecess(char dataTime[14],int BIndex,double r1,double r2,double r3,double r4)//����Ѿ�����Ϣ�洢����
{
	int second=charTimeGetSecond(dataTime);
	memcpy(kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].Timestamp,dataTime,sizeof(char)*14);
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[0]=r1;
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[1]=r2;
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[2]=r3;
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[3]=r4;
	kinectTIIndex[second][BIndex]++;
}

void Kinect::kinectDataProProecess()//������Ϣѹ����ͬһ��
{
	char timeFixed[16];
	time_t processIndexInit=processIndex-4;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processIndexInit));
	int second=charTimeGetSecond(timeFixed);
	for (int i=0;i<BODY_COUNT;i++)//ÿ��bodycount
	{
		double r0Sum=0.0,r2Sum=0.0,r3Sum=0.0,r1Sum=0.0;//֮ǰû�з��ֵĴ����Һ�֮ǰ�ǵ��ˣ�����û�мӴ��ڶ����´����ǳ���
		if (kinectTIIndex[second][i]!=0)//����ղ�����
		{
			for (int j=0;j<kinectTIIndex[second][i];j++)//�������
			{
				r0Sum+=kinectTimeRaw[second][i][j].r[0];
				r1Sum+=kinectTimeRaw[second][i][j].r[1];
				r2Sum+=kinectTimeRaw[second][i][j].r[2];
				r3Sum+=kinectTimeRaw[second][i][j].r[3];
			}
			double r0ave=r0Sum/kinectTIIndex[second][i];
			double r1ave=r1Sum/kinectTIIndex[second][i];
			double r2ave=r2Sum/kinectTIIndex[second][i];
			double r3ave=r3Sum/kinectTIIndex[second][i];
			//�ļ��������
			outfile<<timeFixed<<","<<i<<","<<r0ave<<","<<r1ave<<","<<r2ave<<","<<r3ave<<endl;
		}
	}
	//��ղ���
	memset(kinectTimeRaw[second],0,sizeof(KinectDataRaw)*BODY_COUNT*maxStore);
	memset(kinectTIIndex[second],0,sizeof(int)*BODY_COUNT);
}

int Kinect::charTimeGetSecond(char ttt[14])//��õõ����ݵĺ���λ
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}

void Kinect::CoordinateMapperProcess()
{
	//��ȡ�任�ߴ���ͼ��
	if ( SUCCEEDED(hResult) )
	{
		vector<ColorSpacePoint> colorSpacePoints( DepthHeight*DepthWidth );
		hResult = pCoordinateMapper->MapDepthFrameToColorSpace(DepthWidth*DepthHeight,reinterpret_cast<UINT16*>(depthDeTemp.data),DepthWidth*DepthHeight,&colorSpacePoints[0] );
		if ( SUCCEEDED(hResult) )
		{
			CoordinateMapperMat = Scalar( 0, 0, 0, 0);//��Ҫ����ˢ����ͼƬ
			CoordinateMapperMat_TM = 0;//��Ҫ����ˢ����ͼƬ
			for ( int y = 0; y < DepthHeight; y++)
			{
				for ( int x = 0; x < DepthWidth; x++)
				{
					unsigned int index = y * DepthWidth + x;
					ColorSpacePoint p = colorSpacePoints[index];
					int colorX = static_cast<int>( std::floor( p.X + 0.5 ) );  //std::floor����ȡ��������ֵ�Ǹ�����������ǰ��static_cast<int>����ǿ������ת��Ϊ����������
					int colorY = static_cast<int>( std::floor( p.Y + 0.5 ) );
					if( ( colorX >= 0 ) && ( colorX < ColorWidth ) && ( colorY >= 0 ) && ( colorY < ColorHeight )){
						CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorMat.at<cv::Vec4b>( colorY, colorX );  //�����ͼ����ӳ���Ķ�Ӧ�Ĳ�ɫ��Ϣͼ����ƥ��ͼ
						CoordinateMapperMat_TM.at<UCHAR>( y, x ) = 255; //������ģ����Ϊת����Ĳ�ɫͼ�κ���ȱ���Щ������û�еģ��޳���ɫ���������
					}
				}
			}
		}
	}
	
}

void Kinect::bodyColorHistGet(int index,HistBind &dst) //�õ���ֱ��ͼ�ĺ���  //�Ȳ����µõ���ֵ
{
	//��ȡֱ��ͼ�õ�����
	int bins=256;
	int histSize[]={bins};
	float range[]={0,256};
	const float* ranges[]={range};
	int channelsR[]={0};
	int channelsG[]={1};
	int channelsB[]={2};
	Mat RMat=Mat(bodyIndexMatInt==bodyInt[index])&CoordinateMapperMat_TM;
	Mat	BMat=Mat(bodyIndexMatInt==bodyInt[index])&CoordinateMapperMat_TM;
	Mat	GMat=Mat(bodyIndexMatInt==bodyInt[index])&CoordinateMapperMat_TM;
	erode(RMat,RMat,histElement);//ȥ�봦��
	erode(GMat,GMat,histElement);
	erode(BMat,BMat,histElement);
	dilate(RMat,RMat,histElement);
	dilate(GMat,GMat,histElement);
	dilate(BMat,BMat,histElement);
	calcHist(&CoordinateMapperMat,1,channelsR,RMat,dst.redHist,1,histSize,ranges,true,false);
	calcHist(&CoordinateMapperMat,1,channelsG,BMat,dst.greenHist,1,histSize,ranges,true,false);
	calcHist(&CoordinateMapperMat,1,channelsB,GMat,dst.blueHist,1,histSize,ranges,true,false);
}

//�Ƚ���ͨ��ֱ��ͼ�ĺ���
double Kinect::histBindCompare(HistBind &src1,HistBind &src2)
{
	if (src2.blueHist.empty()||src1.blueHist.empty())
	{
		return 0.0;
	}
	//Ҫ��һ��ȫ����ж� (���Ϊ�㣬��ֱ��ͼ��Ӧ����ֵ����1�����Բ���)
	if (countNonZero(src1.blueHist)==0&&countNonZero(src1.redHist)==0&&countNonZero(src1.greenHist)==0)
	{
		return 0.0;
	}
	if (countNonZero(src2.blueHist)==0&&countNonZero(src2.redHist)==0&&countNonZero(src2.greenHist)==0)
	{
		return 0.0;
	}
	normalize(src1.blueHist,src1.blueHist,0,1,NORM_MINMAX,-1,Mat());
	normalize(src2.blueHist,src2.blueHist,0,1,NORM_MINMAX,-1,Mat());
	normalize(src1.redHist,src1.redHist,0,1,NORM_MINMAX,-1,Mat());
	normalize(src2.redHist,src2.redHist,0,1,NORM_MINMAX,-1,Mat());
	normalize(src1.greenHist,src1.greenHist,0,1,NORM_MINMAX,-1,Mat());
	normalize(src2.greenHist,src2.greenHist,0,1,NORM_MINMAX,-1,Mat());
	return 0.3333*compareHist(src1.blueHist,src2.blueHist,CV_COMP_CORREL)+0.3333*compareHist(src1.greenHist,src2.greenHist,CV_COMP_CORREL)+0.3333*compareHist(src1.redHist,src2.redHist,CV_COMP_CORREL);
}


int Kinect::reNewIndex(int index) //����������²�һ���ԣ�����ֵ��һ��  ����˵�������㻹ͦ��ġ��о��ڻ�ȡ����ֵʱ����һ֡�ǲ����ȡֱ��ͼ�����ݵġ���Ϊ����һ���ӳ٣��ӳٵĻ��ƾ�����������
{
	
	int resultIndex=index;
	HistBind tempBind;
	bodyColorHistGet(index,tempBind); //�ȵõ�����������
	double maxNumber=0.0;
	//���ӵķ����ǲ��õģ��ý�����˼������    ��������30Ϊ�գ���ʹ����0��ʾ�����������û���õ�����Ϊ30��0������ÿ�ζ���һ�����ӵ�30ʱ������Ϊ30����Ϊ�գ�
	//ÿ�����Ҷ�Ӧ������ֱ��ͼ�����Ч���ܲѰ�������ģ�����ҵ���õģ��Ҵ�����ֵ���򽻻���������
	maxNumber=histBindCompare(tempBind,storeTemp[index].hist);
	cout<<"ԭ����Ӧ�����µ�ֱ��ͼϵ����"<<maxNumber<<endl;
	//�ȱȽ϶�Ӧ������ֵ
	if (maxNumber>0.5) //��ֵ���Ե�
	{
		//���ֱ��ͼ��Ӧ��ͬ�Ͳ��ٽ��������ĸ���
		storeTemp[index].hist=tempBind;  //��������
		storeTemp[index].magicNumber=0;	//���±�־λ
		resultIndex=storeTemp[index].strIndex;//�����滻
	}
	else  //�����ƥ�����ɽ�����Ч�����ã���Ȼ����ǿ�����ν�һЩ�Ѿ���ʱ��ֱ��ͼ����ɾ����
	{
		//�ҵ�ֵ��������
		int maxTemp=index;
		for (int i=0;i<6;i++)
		{
			if ((i!=index)&&storeTemp[i].magicNumber!=30)//����Ӧ��������ȷʱ�������ǿյ�����
			{
				//�½�һ���ļ�������
				deee<<"ֱ��ͼ1:"<<tempBind.redHist<<endl;
				deee<<"ֱ��ͼ2:"<<storeTemp[i].hist.redHist<<endl;
				double dell=histBindCompare(tempBind,storeTemp[i].hist);
				//�Ƚ��˾ͼ�1�����ù������ġ���Ϊ����������õĵ��Ļ��ͻ���������Ϊ0��
				storeTemp[i].magicNumber++;
				cout<<i<<"�ȽϺ��ֱ��ͼϵ����"<<dell<<endl;
				if(dell>maxNumber) //�����Ҹ����ʵ�
				{
					maxNumber=dell;
					maxTemp=i;
				}
			}
		}
		if (maxNumber>0.5) //�����׼Ӧ��Ҫͳһ�����ȷ���ҵ����������ͽ��ɵ���Ϣ�����ڽ��н���
		{
			storeTemp[maxTemp].hist=storeTemp[index].hist;  //��ΪҪ�������ݣ�������ν�Ľ������ǽ�ԭ�������ݷŵ�ѡ�õ��������
			++storeTemp[index].magicNumber;
			storeTemp[maxTemp].magicNumber=storeTemp[index].magicNumber;	//���±�־λ
			resultIndex=storeTemp[maxTemp].strIndex;//��������ĸ���
			//storeTemp[maxTemp].strIndex=storeTemp[index].strIndex+storeTemp[maxTemp].strIndex;	//��������
			//storeTemp[index].strIndex=storeTemp[maxTemp].strIndex-storeTemp[index].strIndex;
			//storeTemp[maxTemp].strIndex=storeTemp[maxTemp].strIndex-storeTemp[index].strIndex;
			swap(storeTemp[maxTemp].strIndex,storeTemp[index].strIndex);
			storeTemp[index].hist=tempBind;
			storeTemp[index].magicNumber=0;	
			
		}
		else   //û������Ļ�˵����ʷ��Ϣ��ȷʵû�����������һ������ħ��ֵ���н�������Ȼά��ԭ��������
		{
			//������ħ��ֵ
			int maxMagNum=0;
			int maxMagNumIndex=index;
			set<int> quchong;//�洢�Ѿ��е����� �������Ϊ0-5
			for (int i=0;i<6;i++)
			{
				//if (i!=index)//����Ӧ��������ȷʱ�������ǿյ�����
				//{
					if (storeTemp[i].magicNumber>maxMagNum)
					{
						maxMagNum=storeTemp[i].magicNumber;
						maxMagNumIndex=i;
					}
					if (storeTemp[i].strIndex!=-1)
					{
						quchong.insert(storeTemp[i].strIndex);
					}
				//}
			}
			//�п���ֻ��һ�������ͱȽ�������
			//if (quchong.size()==0)
			//{
			//	quchong.insert(storeTemp[index].strIndex);
			//}
			//��ɽ���
			storeTemp[maxMagNumIndex].hist=storeTemp[index].hist;  //��ΪҪ�������ݣ�������ν�Ľ������ǽ�ԭ�������ݷŵ�ѡ�õ��������
			++storeTemp[index].magicNumber;
			storeTemp[maxMagNumIndex].magicNumber=storeTemp[index].magicNumber;	//���±�־λ
			storeTemp[maxMagNumIndex].strIndex=storeTemp[index].strIndex;
			//��Ϊ��֮ǰû�еģ������¸�ֵ����
			for(int l=0;l<5;l++)
			{
				if (quchong.find(l)==quchong.end())
				{
					//��������Ϊ֮ǰû�г��ֹ�������
					storeTemp[index].strIndex=l;//��Ϊ�п��ܳ���һ������ʧһ��ʱ��������³��֣����������ᷢ����ͻ�������µ�������3��Ȼ��֮ǰ��ʧ������Ҳ��3�������ٳ��ֵĻ����
					break;
				}
			}
			storeTemp[index].hist=tempBind;
			storeTemp[index].magicNumber=0;	
			resultIndex=storeTemp[index].strIndex;
		}
	}
	//����ٸ�����ħ��������ܾö�û�������ˣ��ͰѴ���30�Ĺ̶�һ�¡�
	for (int i=0;i<6;i++)
	{
		if (storeTemp[i].magicNumber!=0)
		{
			storeTemp[i].magicNumber++;
		}
		if (storeTemp[i].magicNumber>30)     //���Ӧ��ָ����30֡�����ݣ����Ը���ʵ�����������
		{
			storeTemp[i].magicNumber=30;
			storeTemp[i].strIndex=-1;//������Ҳ��������
		}
	}
	//�������ģ����ɾ����
	for (int k=0;k<6;k++)
	{
		cout<<k<<" "<<"�洢��������"<<storeTemp[k].strIndex<<" "<<"ħ����"<<storeTemp[k].magicNumber<<endl;
	}
	return resultIndex;
}

HRESULT Kinect::InitBodyIndex()
{
	bodyInt[0]=15;
	bodyInt[1]=55;
	bodyInt[2]=95;
	bodyInt[3]=135;
	bodyInt[4]=175;
	bodyInt[5]=215;
	hResult = pSensor->get_BodyIndexFrameSource( &pBodyIndexSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_BodyIndexFrameSource"<<endl;
		return -1;
	}
	hResult = pBodyIndexSource->OpenReader( &pBodyIndexReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IBodyIndexFrameSource::OpenReader()"<<endl;
		return -1;
	}
}

void Kinect::bodyIndexProcess()
{
	cv::Vec3b color[6]; 
	color[0] = cv::Vec3b( 255, 0, 0 );
	color[1] = cv::Vec3b( 0, 255, 0 );
	color[2] = cv::Vec3b( 0, 0, 255 );
	color[3] = cv::Vec3b( 255, 255, 0 );
	color[4] = cv::Vec3b( 255, 0, 255 );
	color[5] = cv::Vec3b( 0, 255, 255 );
	hResult = pBodyIndexReader->AcquireLatestFrame( &pBodyIndexFrame ); 
	if( SUCCEEDED( hResult ) ){
		unsigned int bufferSize = 0;
		unsigned char* buffer = nullptr;
		hResult = pBodyIndexFrame->AccessUnderlyingBuffer( &bufferSize, &buffer );
		if( SUCCEEDED( hResult ) ){
			for( int y = 0; y < DepthHeight; y++ ){
				for( int x = 0; x < DepthWidth; x++ ){
					unsigned int index = y * DepthWidth + x;
					if( buffer[index] != 0xff ){
						bodyIndexMat.at<cv::Vec3b>( y, x ) = color[buffer[index]];
						bodyIndexMatInt.at<char>(y,x)=bodyInt[buffer[index]];
					}
					else{
						bodyIndexMat.at<cv::Vec3b>( y, x ) = cv::Vec3b( 0, 0, 0 ); 
						bodyIndexMatInt.at<char>(y,x)=0;
					}
				}
			}
		}
	}
	SafeRelease( pBodyIndexFrame );
}

