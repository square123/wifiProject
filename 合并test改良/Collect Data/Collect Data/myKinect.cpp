#include"myKinect.h"

Kinect::Kinect()
{
	pBodyIndexFrame=nullptr;
	pDepthFrame = nullptr;
	pColorFrame = nullptr;
	pBodyFrame = nullptr;

	pDepthBuffer = NULL;
	pBodyIndexBuffer = NULL;
	img.create(ColorHeight,ColorWidth,CV_8UC4);
	bodyMat.create( ColorHeight / 2, ColorWidth / 2, CV_8UC4 );	
	color.create( ColorHeight / 2, ColorWidth / 2, CV_8UC4 );
	depthMat16.create(DepthHeight, DepthWidth, CV_16UC1);
	depthMat8.create(DepthHeight, DepthWidth, CV_8UC1 );       //8λ���ͼ��ʾ
	bodyIndexMat.create(DepthHeight, DepthWidth, CV_8UC3);
	bodyIndexMatInt.create(DepthHeight, DepthWidth, CV_8UC1);

	NumOnce = 0;
	Num=0;
	Temp=6;

	for (int i=0;i<6;i++)
	{
		DepthShow[i]=10;
		DepthSort[i]=10;
	}

	T_enter=0;
	T_leave=0;
	
	T_enter_flag=0;
}

Kinect::~Kinect()
{
	SafeRelease( pDepthSource );
	SafeRelease( pDepthReader );
	SafeRelease( pDepthDescription );
	SafeRelease( pColorSource );
	SafeRelease( pColorReader );
	SafeRelease( pColorDescription );
	SafeRelease( pBodyIndexSource );
	SafeRelease( pBodyIndexReader );
	SafeRelease( pInfraredSource );
	SafeRelease( pInfraredReader );
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

HRESULT Kinect::InitColor()
{
	hResult = pSensor->get_ColorFrameSource( &pColorSource );
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_ColorFrameSource()"<<endl;
		return -1;
	}
	/*hResult = pColorSource->OpenReader( &pColorReader );     
	if ( FAILED( hResult ) )
	{
	cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
	return -1; 
	}
	*/
}

HRESULT Kinect::InitDepth()
{
	hResult = pSensor->get_DepthFrameSource( &pDepthSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_DepthFrameSource()"<<endl;
		return -1;
	}
	//hResult = pDepthSource->OpenReader( &pDepthReader );           
	//if ( FAILED( hResult ) )
	//{
	//	cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
	//	return -1;
	//}
}

HRESULT Kinect::InitBodyIndex()
{
	hResult = pSensor->get_BodyIndexFrameSource( &pBodyIndexSource );      
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IKinectSensor::get_BodyIndexFrameSource"<<endl;
		return -1;
	}
	/*hResult = pBodyIndexSource->OpenReader( &pBodyIndexReader );           
	if ( FAILED( hResult ) )
	{
	cerr <<"Error:IBodyIndexFrameSource::OpenReader()"<<endl;
	return -1;
	}*/
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

HRESULT Kinect::InitCoorMap()
{
	hResult = pSensor->get_CoordinateMapper( &pCoordinateMapper );     
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IKinectSensor::get_CoordinateMapper()"<<endl;
		return -1;
	}
}

void Kinect::bodyIndexProcess()
{

	hResult = pBodyIndexSource->OpenReader( &pBodyIndexReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IBodyIndexFrameSource::OpenReader()"<<endl;
	}
	//bodyIndexProcess
	//IBodyIndexFrame* pBodyIndexFrame = nullptr;   kinect��ʼ��ʱ�Ѿ�д��
	//unsigned int nBodyIndexBufferSize = 0;
	//unsigned char *pBodyIndexBuffer = NULL;

	cv::Vec3b color[6]; 
	color[0] = cv::Vec3b( 255, 0, 0 );
	color[1] = cv::Vec3b( 0, 255, 0 );
	color[2] = cv::Vec3b( 0, 0, 255 );
	color[3] = cv::Vec3b( 255, 255, 0 );
	color[4] = cv::Vec3b( 255, 0, 255 );
	color[5] = cv::Vec3b( 0, 255, 255 );
	UINT8 bodyInt[6]={15,55,95,135,175,215};//��������6����

	hResult = pBodyIndexReader->AcquireLatestFrame(&pBodyIndexFrame);//Frame�������洢���ݵ��࣬ÿһ�ζ���Reader���������ݴ���Frame��
	if (SUCCEEDED(hResult))
	{
		unsigned int nBodyIndexBufferSize = 0;
		//unsigned char* pBodyIndexBuffer = nullptr;
		hResult = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);  //��Ҫ���ǰ�Frame�е�����ת�浽һ��������  pBodyIndexBuffer����һ��424*512��С��16λunsigned int���飬�����洢�������
		if( SUCCEEDED( hResult ) ){
			for( int y = 0; y < DepthHeight; y++ ){
				for( int x = 0; x < DepthWidth; x++ ){
					unsigned int index = y * DepthWidth + x;
					if( pBodyIndexBuffer[index] != 0xff ){
						bodyIndexMat.at<cv::Vec3b>( y, x ) = color[pBodyIndexBuffer[index]];
						bodyIndexMatInt.at<char>(y,x)=bodyInt[pBodyIndexBuffer[index]];
					}
					else{
						bodyIndexMat.at<cv::Vec3b>( y, x ) = cv::Vec3b( 0, 0, 0 ); 
						bodyIndexMatInt.at<char>(y,x)=0;
					}
				}
			}
		}
	}
	SafeRelease(pBodyIndexFrame); 
}

void Kinect::depthProcess()
{

	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
	}
	UINT nDepthBufferSize = 0;
	//UINT16 *pDepthBuffer = NULL;

	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);//��ȡ����������֡
	if (SUCCEEDED(hResult))
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);  //�õ�һ�����֡���ݵĵ�
		for(int i=0;i<DepthHeight;i++)
		{
			ushort* ptrDepth16 = depthMat16.ptr<ushort>(i);  // 16
			uchar* ptrDepth8 = depthMat8.ptr<uchar>(i);  //8

			for(int j=0;j<DepthWidth;j++)
			{
				USHORT depth = *pDepthBuffer;  //16   �������ֽ�Ϊ��λ���ж�ȡ
				ptrDepth16[j] = depth;        //ֱ�Ӵ洢��ʾ
				ptrDepth8[j] = depth%256; //תΪ8λ�洢��ʾ
				pDepthBuffer++;               //16   �������ֽ�Ϊ��λ���ж�ȡ
			}
		}
	}
	SafeRelease( pDepthFrame );  
}

void Kinect::colorProcess()
{

	hResult = pColorSource->OpenReader( &pColorReader );     
	if ( FAILED( hResult ) )
	{
		cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
	}


	UINT nColorBufferSize = 0;
	RGBQUAD *pColorBuffer = NULL;    //RGBQUAD��һ���ṹ�壬�䱣��һ�����ص��RGBֵ         
	hResult = pColorReader->AcquireLatestFrame(&pColorFrame);  //��ȡ�����Ĳ�ɫ֡
	if (SUCCEEDED(hResult))
	{
		hResult = pColorFrame->CopyConvertedFrameDataToArray(ColorHeight*ColorWidth * 4, reinterpret_cast<BYTE*>(img.data), ColorImageFormat::ColorImageFormat_Bgra);  //��ɫ֡����ת���Ҫ��ĸ�ʽ��
		if (SUCCEEDED(hResult))
		{
			hResult = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));  //�õ���ɫ֡���ݵĵ�
			cv::resize( img, color, cv::Size(), 0.5, 0.5 );
			//imshow("1",bodyMat);
		}
	}

	SafeRelease( pColorFrame );
}

void Kinect::bodyProcess2()
{  //1

	//��ÿ��ʹ�ù������ǰ�Ȱѹ�������Ϣ��0
	memset(state,0,sizeof(state));
	memset(coord_x,0,sizeof(coord_x));
	memset(coord_y,0,sizeof(coord_y));

	hResult = pBodyIndexSource->OpenReader( &pBodyIndexReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IBodyIndexFrameSource::OpenReader()"<<endl;
	}

	IBodyIndexFrame* pBodyIndexFrame = nullptr;
	unsigned int nBodyIndexBufferSize = 0;
	unsigned char *pBodyIndexBuffer = NULL;

	hResult = pBodyIndexReader->AcquireLatestFrame(&pBodyIndexFrame);//Frame�������洢���ݵ��࣬ÿһ�ζ���Reader���������ݴ���Frame��
	if (SUCCEEDED(hResult))
	{
		hResult = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);  //��Ҫ���ǰ�Frame�е�����ת�浽һ��������  pBodyIndexBuffer����һ��424*512��С��16λunsigned int���飬�����洢�������
	}


	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
	}

	//�������
	if (SUCCEEDED(hResult))
	{     //2
		//IDepthFrame* pDepthFrame = nullptr;
		UINT nDepthBufferSize = 0;
		UINT16 *pDepthBuffer = NULL;

		hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);//��ȡ����������֡
		if (SUCCEEDED(hResult))
		{
			hResult = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);  //�õ�һ�����֡���ݵĵ�
		}


		hResult = pColorSource->OpenReader( &pColorReader );     
		if ( FAILED( hResult ) )
		{
			cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
		}
		//��ɫ����
		if (SUCCEEDED(hResult))
		{  //3
			//IColorFrame  *pColorFrame = nullptr;
			UINT nColorBufferSize = 0;
			RGBQUAD *pColorBuffer = NULL;    //RGBQUAD��һ���ṹ�壬�䱣��һ�����ص��RGBֵ         
			hResult = pColorReader->AcquireLatestFrame(&pColorFrame);  //��ȡ�����Ĳ�ɫ֡
			if (SUCCEEDED(hResult))
			{
				hResult = pColorFrame->CopyConvertedFrameDataToArray(ColorHeight*ColorWidth * 4, reinterpret_cast<BYTE*>(img.data), ColorImageFormat::ColorImageFormat_Bgra);  //��ɫ֡����ת���Ҫ��ĸ�ʽ��
				if (SUCCEEDED(hResult))
				{
					hResult = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));  //�õ���ɫ֡���ݵĵ�
				}
			}

			//������Ϣ���ɫͼӳ��
			//IBodyFrame* pBodyFrame = nullptr;
			hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
			if( SUCCEEDED( hResult ) )
			{    //4
				IBody* pBody[BODY_COUNT] = { 0 }; //Ĭ�ϵ��� 6 ������ ����ʼ�����еĹ�����Ϣ
				hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );//���¹������ݣ�
				if( SUCCEEDED( hResult ) )
				{   //5  

					static float tmpDepth[6]={0,0,0,0,0,0}; //��¼�ϴε����ֵ�����ڴ˴κ��ϴεıȽ��Ƿ��б仯
					for( int count = 0; count < BODY_COUNT; count++ )
					{  //count����0��5��6�����ҹ�������ʾ
						BOOLEAN bTracked = false;    //��ʼ������׷�ٵ����塱��ֵΪ��
						hResult = pBody[count]->get_IsTracked( &bTracked );  //ȷ������׷�ٵ����塣
						if( SUCCEEDED( hResult ) && bTracked )
						{
							Joint joint[JointType::JointType_Count];   //ȡ������Joint(�ؽ�)��JointType��һ��ö�����ͣ���ͬλ�õĹؽڵ㶼�ǲ�ͬ�ı�ű�ʾ�ġ�count��һ����ֵ25��
							hResult = pBody[ count ]->GetJoints( JointType::JointType_Count, joint );  //ȡ������Joint(�ؽ�)��


				
							//����ʵʱ��ʾ������
							DepthShow[count]=joint[JointType_Neck].Position.Z;

							if (DepthShow[count]!=10)
							{
								cout<<"������ "<<count<<" �����ֵΪ�� "<<DepthShow[count]<<endl;
								DepthShow[count]=10;  //��ʾ������������Ϊ��ʼֵ

							}

							//���������һ�����ݣ�����������������������ж�
							DepthSort[count]=joint[JointType_Neck].Position.Z;
							

							if( SUCCEEDED( hResult ) )
							{
								CvPoint skeletonPoint[BODY_COUNT][JointType_Count] = { cvPoint(0,0) };							
								// Joint
								for( int type = 0; type < JointType::JointType_Count; type++ )
								{  //����25���ؽڵ�
									ColorSpacePoint colorSpacePoint = { 0 };   //ColorSpacePoint�ṹ��������һ��colorSpacePointʵ�������� Represents a 2D point in color space, expressed in pixels.������һ��2d������ɫ�ռ���,�����ر�ʾ��
									pCoordinateMapper->MapCameraPointToColorSpace( joint[type].Position, &colorSpacePoint );//����ͷ��ռ䵽��ɫ�ռ��ת�����ѹؽڵ������ת������ɫ�ռ��ϣ�������ʾ
								}
							}				
						}
					}				
				

					vector<DepthSpacePoint>depthspacepoints(ColorWidth*ColorHeight); 
					if (SUCCEEDED(hResult))
					{
						pCoordinateMapper->MapColorFrameToDepthSpace(DepthWidth * DepthHeight, (UINT16*)pDepthBuffer, ColorWidth * ColorHeight, &depthspacepoints[0]);  //����ɫ�ռ䵽��ȿռ������ӳ�䣬�����任Ҳ��  depthspacepoints[0]�ǲ���������׵�ַ��Ӧ����
#pragma omp parallel for  
						for (int i = 0; i < ColorHeight; i++)
							for (int j = 0; j < ColorWidth; j++)
							{
								int k = i*ColorWidth + j;//ѭ������ÿһ������
								DepthSpacePoint p = depthspacepoints[k];  //��ƥ������ͼ�����ÿһ�����ء�  DepthSpacePoint��һ���ṹ�� p�ǽṹ�����͵ı���

								int depthX = (int)(p.X + 0.5f); //������ṹ�����ͱ���p���ýṹ���еĳ�Ա����������p��λk����,��Ϊk����Ϊ��һ�����εı�����������һ��Ĺ��ܾ��ǰѻ�ȡ����ÿһ���ض������ṹ�����͵ı���
								int depthY = (int)(p.Y + 0.5f);

								if ((depthX >= 0 && depthX < DepthWidth) && (depthY >= 0 && depthY < DepthHeight))
								{
									BYTE player = pBodyIndexBuffer[depthX + (depthY * DepthWidth)];  //BYTE��unsigned char,�ѻ�ȡ����ÿһ�����ض�����player
									if (player == 0xff)  //��ȿռ�if���ж�ÿһ�������ǲ����������壬0~5�ֱ��ʾ��ʶ���6���ˣ�0xff��ʾ�����ز������ˣ���˵�������ڵ�7���ˣ���kinectʶ���ˣ����ԾͲ��Ѹ����ص���������ˣ�
									{ 
										img.at <Vec4b>(i,j) = Vec4b(255,255,255,255); //��ɫ�ռ䴦��0��0��0��ʾ��ɫ��255��ʾalpha͸����,����ԽСԽ͸��
									}//���ﲢûдһ���ж��ҵ�������ô�죬Ĭ�Ͼ��ǲ����д�����ô����ֱ����ʾ��
								}
								else  //���else�Ǻ����ϸ�ifƥ��ģ���ʾ����������ռ䣨�������壩֮��ĵط�Ҳ��Ҫ��Ϊ��ɫ
									img.at <Vec4b>(i, j) = Vec4b(255,255,255,255);
							}//��ɫ�ռ����˫��ѭ���������
					}  // pCoordinateMapper

					cv::resize( img, bodyMat, cv::Size(), 0.5, 0.5 );
					//imshow("BODY", bodyMat);

					//���������ȡ
					pBodyFrame->GetAndRefreshBodyData(_countof(pBody), pBody); 
					for (int i = 0; i < BODY_COUNT; ++i)  
					{   //6
						IBody* pBody1 = pBody[i];
						if (pBody1)   //���������˵���ţ�����������ж��ǲ��ǿ�ָ�롣
						{    //7
							BOOLEAN bTracked = false;   //�Ƿ��ѱ�����
							hResult = pBody1->get_IsTracked(&bTracked);

							if (SUCCEEDED(hResult) && bTracked)  //����������µ����岢�����ܱ����ٵ�
							{  //8

								Joint joints[JointType_Count];//�洢�ؽڵ��� Joint��һ���ṹ�����  JointType_Count=25

								//�洢�������ϵ�еĹؽڵ�λ��
								//DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];
								ColorSpacePoint *colorSpacePosition = new ColorSpacePoint[_countof(joints)];

								//��ùؽڵ���
								hResult = pBody1->GetJoints(_countof(joints), joints);
								if (SUCCEEDED(hResult))
								{   //9
									for (int j = 0; j < _countof(joints); ++j)
									{
										//���ؽڵ���������������ϵ��-1~1��ת���������ϵ��424*512��
										//	pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
										pCoordinateMapper->MapCameraPointToColorSpace(joints[j].Position, &colorSpacePosition[j]);
									}   

									for (int j=0;j<25;j++)
									{
										state[i][j]=joints[j].TrackingState;
									}
									

									//���еĹ����ڵ�������
									for (int j=0;j<25;j++)
									{
										coord_x[i][j]=colorSpacePosition[j].X;
										coord_y[i][j]=colorSpacePosition[j].Y;
									}
								}  //9
							}   //8
						}  //7

					}  //6
				}   //5

                //ѭ����ȫ�ͷ�6����Ϊbody��ָ��
				for( int count = 0; count < BODY_COUNT; count++ )
				{   
					SafeRelease( pBody[count] );
				}

				SafeRelease( pBodyFrame );   
			}  //4

			SafeRelease(pColorFrame);
		}  //��ɫ���� 3

		SafeRelease(pDepthFrame);
	} //������� 2

	//====================���������ȡend==============================================
	SafeRelease(pBodyIndexFrame);

} //1

void Kinect::segmentProcess2(int Miner)
{
	cout<<"������ "<<Miner<<" ���ĵ� "<<NumOnce<<" ֡ͼ��"<<endl;
	
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	switch (Miner)
	{
	case 0:{dst = "F://�������//0//";break;}
	case 1:{dst = "F://�������//1//";break;}
	case 2:{dst = "F://�������//2//";break;}
	case 3:{dst = "F://�������//3//";break;}
	case 4:{dst = "F://�������//4//";break;}
	case 5:{dst = "F://�������//5//";break;}
	default: break;
	}

	for (int i=0;i<11;i++)
	{
		switch (i)
		{
		case 0: SegmentAllParts(0,0,0,0,state[Miner][1],0,i);break;
		case 1: SegmentAllParts(coord_x[Miner][2],coord_y[Miner][2],coord_x[Miner][3],coord_y[Miner][3],state[Miner][2],state[Miner][3],i);      break;
		case 2: SegmentAllParts(coord_x[Miner][2],coord_y[Miner][2],coord_x[Miner][0],coord_y[Miner][0],state[Miner][2],state[Miner][0],i);      break;
		case 3: SegmentAllParts(coord_x[Miner][8],coord_y[Miner][8],coord_x[Miner][9],coord_y[Miner][9],state[Miner][8],state[Miner][9],i);      break;
		case 4: SegmentAllParts(coord_x[Miner][9],coord_y[Miner][9],coord_x[Miner][11],coord_y[Miner][11],state[Miner][9],state[Miner][11],i);   break;
		case 5: SegmentAllParts(coord_x[Miner][4],coord_y[Miner][4],coord_x[Miner][5],coord_y[Miner][5],state[Miner][4],state[Miner][5],i);      break;
		case 6: SegmentAllParts(coord_x[Miner][5],coord_y[Miner][5],coord_x[Miner][7],coord_y[Miner][7],state[Miner][5],state[Miner][7],i);      break;
		case 7: SegmentAllParts(coord_x[Miner][16],coord_y[Miner][16],coord_x[Miner][17],coord_y[Miner][17],state[Miner][16],state[Miner][17],i);break;
		case 8: SegmentAllParts(coord_x[Miner][17],coord_y[Miner][17],coord_x[Miner][18],coord_y[Miner][18],state[Miner][17],state[Miner][18],i);break;
		case 9: SegmentAllParts(coord_x[Miner][12],coord_y[Miner][12],coord_x[Miner][13],coord_y[Miner][13],state[Miner][12],state[Miner][13],i);break;
		case 10:SegmentAllParts(coord_x[Miner][13],coord_y[Miner][13],coord_x[Miner][14],coord_y[Miner][14],state[Miner][13],state[Miner][14],i);break;

		default:
			break;
		}
	}
		NumOnce++;  //ĳһ�������������ۼ�
}

void Kinect::SegmentAllParts(float first_x,float first_y,float second_x,float second_y,int first_state,int second_state,int SelectExecute)
{

	if (SelectExecute==0)   //ִ��ȫ��Ĵ���
	{
		dst_temp=dst;

		sprintf_s( chari, "%05d", NumOnce);  
		dst_temp += chari;
		dst_temp += "_0";//ȫ��
		dst_temp += ".jpg";

		if(first_state==2)
		{
			cv::imwrite( dst_temp, bodyMat, compression_params);
		}

	}
	else
	{
		int arr_x[4];
		int arr_y[4];
		if (SelectExecute==1)
		{
			int radius=fabs(first_y-second_y);//Բ�İ뾶

			arr_x[0]=second_x;arr_x[1]=second_x+radius;arr_x[2]=second_x,arr_x[3]=second_x-radius;  //���е�x�������һ������
			arr_y[0]=second_y-radius;arr_y[1]=second_y;arr_y[2]=first_y;arr_y[3]=second_y;         //���е�y�������һ������
			
		}
		else
		{
			double result;
			result = atan2 (first_y-second_y,first_x-second_x) * 180 / CV_PI;
			float len=sqrt(pow((first_x-second_x),2)+pow((first_y-second_y),2));  //�����������ڵ������Ϊ���εĳ���
			RotatedRect rRect=RotatedRect(Point2f((first_x+second_x)/2,(first_y+second_y)/2),Size2f(len,len/2),result); //����һ����ת����    
			Point2f vertices[4];    
			rRect.points(vertices);

			arr_x[0]=vertices[0].x;arr_x[1]=vertices[1].x;arr_x[2]=vertices[2].x;arr_x[3]=vertices[3].x; //���е�x�������һ������
			arr_y[0]=vertices[0].y;arr_y[1]=vertices[1].y;arr_y[2]=vertices[2].y;arr_y[3]=vertices[3].y; //���е�y�������һ������
		}

		sort(arr_x,arr_x+4);  //��������
		sort(arr_y,arr_y+4);

		int length=arr_x[3]-arr_x[0]+1;
		int width=arr_y[3]-arr_y[0]+1;

		Mat ImgPart(width,length,CV_8UC4,Scalar(255,255,255));
		if((arr_y[0]>0)&&(arr_y[0]<1080)&&(arr_y[3]>0)&&(arr_y[3]<1080)&&(arr_x[0]>0)&&(arr_x[0]<1920)&&(arr_x[3]>0)&&(arr_x[3]<1920))
		{
			for(int i=arr_y[0];i<arr_y[3];i++)  //��
				for(int j=arr_x[0];j<arr_x[3];j++)//��
				{
					if(img.at<Vec4b>(i,j)!=Vec4b(255,255,255,255))
						ImgPart.at<Vec4b>(i-arr_y[0],j-arr_x[0])=img.at<Vec4b>(i,j);	
				}
		}						

		dst_temp=dst;
		sprintf_s( chari, "%05d", NumOnce);  
		dst_temp += chari;
		switch (SelectExecute)
		{
		case 1:  dst_temp += "_1"; break;
		case 2:  dst_temp += "_2"; break;
		case 3:  dst_temp += "_3"; break;
		case 4:  dst_temp += "_4"; break;
		case 5:  dst_temp += "_5"; break;
		case 6:  dst_temp += "_6"; break;
		case 7:  dst_temp += "_7"; break;
		case 8:  dst_temp += "_8"; break;
		case 9:  dst_temp += "_9"; break;
		case 10: dst_temp += "_10";break;

		default:
			break;
		}

		dst_temp += ".jpg";

		if(first_state==2&&second_state==2)
		{
			cv::imwrite( dst_temp, ImgPart, compression_params);	
		}

	}

}


//ͨ���ж�һ���������ĵ���֡��NumOnce���ж���û���ĵ���
void Kinect::select2(int indexer)
{
	cout<<"������ "<<indexer<<" ���ĵ�: "<<NumOnce<<" ֡ͼ��   "<<endl<<endl;
	
	float s[100];   //����һ�����飬���ڶ�ÿһ��ͼ��İ�ɫ����ռ��ֵ��������,��100�ѽ��㹻����һ����λӦ�ò�����100֡

	Directory dir;  
	string path;  
	switch (indexer)
	{
	case 0: { path = "F:\\�������\\0\\";  break; }    
	case 1: { path = "F:\\�������\\1\\";  break; }   
	case 2: { path = "F:\\�������\\2\\";  break; }    
	case 3: { path = "F:\\�������\\3\\";  break; }   
	case 4: { path = "F:\\�������\\4\\";  break; }   
	case 5: { path = "F:\\�������\\5\\";  break; }  
	default: break;
	}

	string exten;
	int size;

	if (NumOnce==0)
	{
		cout<<"û�ɼ����κ�ͼ��"<<endl;
		//Num--;
	}
	else
	{
		cout<<"   ��֡ͼ��ɸѡ..."<<endl;

		for (int parts=0;parts<=10;parts++)
		{
			switch (parts)
			{
			case 0: exten = "*_0.jpg";break;    //ȫ��
			case 1: exten = "*_1.jpg";break;    //ͷ��
			case 2: exten = "*_2.jpg";break;    //����
			case 3: exten = "*_3.jpg";break;    //�ұ���
			case 4: exten = "*_4.jpg";break;    //�ұ���
			case 5: exten = "*_5.jpg";break;    //�����
			case 6: exten = "*_6.jpg";break;    //�����
			case 7: exten = "*_7.jpg";break;    //������
			case 8: exten = "*_8.jpg";break;    //������
			case 9: exten = "*_9.jpg";break;    //������
			case 10:exten = "*_10.jpg";break;   //������
			default: break;
			}

			vector<string> filenames = dir.GetListFiles(path, exten, false); 
			size=0;
			size = filenames.size();

			if(size!=0)
			{
				for (int p = 0; p < size;p++)  
				{  
					string fileName = filenames[p];  
					string fileFullName = path + fileName;  

					const char * filename=fileFullName.c_str();  //��string���͵�·��ת����char��

					IplImage *src=cvLoadImage (filename, 0);   //ֱ��ת���ɻҶ�ͼ

					int n=0;  //ͳ�ư�ɫ���صĸ���  
					for(int i=0;i<src->height;i++)
						for(int j=0;j<src->width;j++)
						{
							if(((uchar *)(src->imageData + i*src->widthStep))[j]==255)  //����Ҷ�ͼ�а�ɫ���صĸ���
								n++; 
						}
						float per=(float)n/(src->height*src->width);  //�����ɫ����������ͼ�е�ռ��
						s[p]=per;	  //�Ѷัͼ��ռ��ֵд��������

						cvReleaseImage(&src);

				}
				//==========�ҳ���С��ռ��ֵ======================
				float min=s[0]; //�����һ������Сֵ
				int index=0;   //���ڼ�¼�����ҵ�����Сֵ���±�

				for(int i=0;i<size;i++)
				{
					if(s[i]<min)
					{
						min=s[i];
						index=i;
					}
				}

				string fileName = filenames[index];  
				string fileFullName = path + fileName;

				const char * filename=fileFullName.c_str();

				switch (parts)
				{//��д��������������������������������������������������������������
				//case 0:{ char cmd_0[255] = {0}; sprintf(cmd_0, "copy %s G:\\�����\\ͼ������\\%05d_0.jpg", filename,T_enter);system(cmd_0);break;}
				//case 1:{ char cmd_1[255] = {0}; sprintf(cmd_1, "copy %s G:\\�����\\ͼ������\\%05d_1.jpg", filename,T_enter);system(cmd_1);break;}
				//case 2:{ char cmd_2[255] = {0}; sprintf(cmd_2, "copy %s G:\\�����\\ͼ������\\%05d_2.jpg", filename,T_enter);system(cmd_2);break;}
				//case 3:{ char cmd_3[255] = {0}; sprintf(cmd_3, "copy %s G:\\�����\\ͼ������\\%05d_3.jpg", filename,T_enter);system(cmd_3);break;}
				//case 4:{ char cmd_4[255] = {0}; sprintf(cmd_4, "copy %s G:\\�����\\ͼ������\\%05d_4.jpg", filename,T_enter);system(cmd_4);break;}
				//case 5:{ char cmd_5[255] = {0}; sprintf(cmd_5, "copy %s G:\\�����\\ͼ������\\%05d_5.jpg", filename,T_enter);system(cmd_5);break;}
				//case 6:{ char cmd_6[255] = {0}; sprintf(cmd_6, "copy %s G:\\�����\\ͼ������\\%05d_6.jpg", filename,T_enter);system(cmd_6);break;}
				//case 7:{ char cmd_7[255] = {0}; sprintf(cmd_7, "copy %s G:\\�����\\ͼ������\\%05d_7.jpg", filename,T_enter);system(cmd_7);break;}
				//case 8:{ char cmd_8[255] = {0}; sprintf(cmd_8, "copy %s G:\\�����\\ͼ������\\%05d_8.jpg", filename,T_enter);system(cmd_8);break;}
				//case 9: {char cmd_9[255] = {0}; sprintf(cmd_9, "copy %s G:\\�����\\ͼ������\\%05d_9.jpg", filename,T_enter);system(cmd_9);break;}
				//case 10:{ char cmd_10[255] = {0}; sprintf(cmd_10, "copy %s G:\\�����\\ͼ������\\%05d_10.jpg", filename,T_enter);system(cmd_10);}break;

				case 0:{ string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_0.jpg";system(tmp.c_str());break;}
				case 1:{ string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_1.jpg";system(tmp.c_str());break;}
				case 2:{string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_2.jpg";system(tmp.c_str());break;}
				case 3:{ string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_3.jpg";system(tmp.c_str());break;}
				case 4:{ string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_4.jpg";system(tmp.c_str());break;}
				case 5:{ string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_5.jpg";system(tmp.c_str());break;}
				case 6:{ string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_6.jpg";system(tmp.c_str());break;}
				case 7:{string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_7.jpg";system(tmp.c_str());break;}
				case 8:{string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_8.jpg";system(tmp.c_str());break;}
				case 9: {string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_9.jpg";system(tmp.c_str());break;}
				case 10:{ string tmp; tmp="copy "+string(filename)+"  F:\\�����\\ͼ������\\"+timeIntToTimestampString(T_enter)+"_10.jpg";system(tmp.c_str());break;}
				default: break;
				}

				//=========ɾͼ=========================
				for(int i=0;i<size;i++)
				{
					string fileName = filenames[i];  
					string fileFullName = path + fileName;  

					const char * filename=fileFullName.c_str(); 
					if( remove(filename) == 0 ) ; 
					else
						perror("remove");
				}
			}
		}
		Num++;
		cout<<"��ǰ�Ѳɼ� "<<Num<<" ������"<<endl;
		cout<<"*******************************"<<endl<<endl;
	}
	NumOnce=0;  //��һ�������ŵ������ĵ��ĸ�����Ϊ0.
}

//�¿���ĳ���
//detPeopleDepth3������ʹ����Ҫ��������Ȼ�ȡ���ֵ
void Kinect::depthProcess2()
{
	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
	}

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

//�ж�һ���ض���������ֵ����ѡ���ǲ���Ҫִ�������ͼ�Ͳ�λ�ķָ�Ĺ�����
short Kinect::detPeopleDepth3()//ͨ���򵥵���ֵ�˲����ж����
{

	return medianOut(depthMat16,rec_tl,rec_br); //����˲���Ľ��
}

//������ѡһ����ֵ���˲�����
short Kinect::medianOut(Mat&src,Point &tl,Point &br)//��16λ���ͼ������˲�
{
	vector<short> tempVec;
	for(int i=tl.y;i<br.y;i++)//�ȰѾ����з����Ԫ�ش洢����
	{
		short* data= src.ptr<short>(i);
		for(int j=tl.x;j<br.x;j++){
			short a=data[j];
			if (a!=0)
			{
				tempVec.push_back(a);
			}
		}
	}
	sort(tempVec.begin(),tempVec.end()); //������
	if (tempVec.size()==0)
	{
		return 0;
	}
	return tempVec[tempVec.size()/2]; // �����
}

string Kinect::timeIntToTimestampString(time_t input)//time_tת����timestamp   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!��ʱ
{
	tm *pTmp=localtime(&input);
	char pStr[15]={};
	sprintf(pStr,"%d%d%d%d%d%d",pTmp->tm_year+1900,pTmp->tm_mon+1,pTmp->tm_mday+1,pTmp->tm_hour,pTmp->tm_min,pTmp->tm_sec);
	return string(pStr);
}
