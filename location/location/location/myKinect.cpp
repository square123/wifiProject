#include "myKinect.h"

Kinect::Kinect()  
{
	pColorFrame = nullptr;
	pDepthFrame = nullptr;
	colorHalfSizeMat.create(ColorHeight/2, ColorWidth/2, CV_8UC4);
	colorMat.create( ColorHeight, ColorWidth, CV_8UC4);
	depthMat16.create(DepthHeight, DepthWidth, CV_16UC1);
	depthMat8.create(DepthHeight, DepthWidth, CV_8UC1 );
	//showOrbit�����ʼ��
	double maxDis=cos(-(rotateAngle)/180*PI)*4.5;
	double maxWid=2*maxDis*tan(35.0/180*PI);
	double minDis=cos(-(rotateAngle)/180*PI)*0.5;//KinectHeight*tan((60.0+(rotateAngle))/180*PI);
	double minWid=2*minDis*tan(35.0/180*PI);
	sOX=int(ceil(maxWid)*100),sOY=int(ceil(maxDis)*100);
	showOrbit=Mat::zeros(sOY,sOX,CV_8UC3);//����Ϊȫ��
	//����ɫ����
	line(showOrbit,Point(sOX/2-(int(maxWid*100)/2),sOY-int(maxDis*100)),Point(sOX/2+(int(maxWid*100)/2),sOY-int(maxDis*100)),Scalar(255,255,255),2,8);
	line(showOrbit,Point(sOX/2+(int(maxWid*100)/2),sOY-int(maxDis*100)),Point(sOX/2+(int(minWid*100)/2),sOY-int(minDis*100)),Scalar(255,255,255),2,8);
	line(showOrbit,Point(sOX/2+(int(minWid*100)/2),sOY-int(minDis*100)),Point(sOX/2-(int(minWid*100)/2),sOY-int(minDis*100)),Scalar(255,255,255),2,8);
	line(showOrbit,Point(sOX/2-(int(minWid*100)/2),sOY-int(minDis*100)),Point(sOX/2-(int(maxWid*100)/2),sOY-int(maxDis*100)),Scalar(255,255,255),2,8);
	floodFill(showOrbit,Point(sOX/2,sOY/2),Scalar(255,255,255));
	saveTmpIntClr=0;
	saveTmpIntDep=0;
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
	SafeRelease( pBodySource );
	SafeRelease( pBodyReader );
	if ( pSensor )
	{
		pSensor->Close();
	}
	SafeRelease( pSensor );
	cv::destroyAllWindows();
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

void Kinect::dataProcess(double x,double y)
{
	double r3=sqrt((x-0.42)*(x-0.42)+(y+1.39)*(y+1.39));
	double r2=sqrt((x-1.13)*(x-1.13)+(y-2.04)*(y-2.04));
	double r1=sqrt((x+2.05)*(x+2.05)+(y-1.7)*(y-1.7));

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
				BOOLEAN bTracked = false;    //��ʼ������׷�ٵ����塱��ֵΪ��
				hResult = pBody[count]->get_IsTracked( &bTracked );  //ȷ������׷�ٵ����塣
				if( SUCCEEDED( hResult ) && bTracked ){
					Joint joint[JointType::JointType_Count];   //ȡ������Joint(�ؽ�)��JointType��һ��ö�����ͣ���ͬλ�õĹؽڵ㶼�ǲ�ͬ�ı�ű�ʾ�ġ�count��һ����ֵ25��
					hResult = pBody[ count ]->GetJoints( JointType::JointType_Count, joint );  //ȡ������Joint(�ؽ�)��
					//cout<<"����Ϊ"<<count<<"�����꣺ "<<"X="<<joint[ JointType_SpineMid].Position.X<<" "<<"Y="<<joint[ JointType_SpineMid].Position.Y<<" "<<"Z="<<joint[ JointType_SpineMid].Position.Z<<endl;
					//����任,ֻ����x�������нǶ�
					double xx,yy,zz;
					xx=joint[ JointType_SpineMid].Position.X;
					yy=(cos(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Y+sin(rotateAngle/180*PI)*joint[JointType_SpineMid].Position.Z);
					zz=((-1)*sin(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Y+cos(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Z);
					cout<<timeFix<<" "<<"x="<<-xx<<"z="<<(zz-(sOY/200))<<endl;
					double r4=sqrt((-xx+0.29851)*(-xx+0.29851)+((zz-(sOY/200))+1.788921)*((zz-(sOY/200))+1.788921));
					double r3=sqrt((-xx-0.708444)*(-xx-0.708444)+((zz-(sOY/200))+1.761442)*((zz-(sOY/200))+1.761442));
					double r2=sqrt((-xx-1.42421)*(-xx-1.42421)+((zz-(sOY/200))-2.18921)*((zz-(sOY/200))-2.18921));
					double r1=sqrt((-xx+2.1276)*(-xx+2.1276)+((zz-(sOY/200))-2.12288)*((zz-(sOY/200))-2.12288));
					outfile<<timeFix<<","<<count<<","<<r1<<","<<r2<<","<<r3<<","<<r4<<endl;
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