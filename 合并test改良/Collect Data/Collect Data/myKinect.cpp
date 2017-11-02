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
	depthMat8.create(DepthHeight, DepthWidth, CV_8UC1 );       //8位深度图显示
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
	//IBodyIndexFrame* pBodyIndexFrame = nullptr;   kinect初始化时已经写了
	//unsigned int nBodyIndexBufferSize = 0;
	//unsigned char *pBodyIndexBuffer = NULL;

	cv::Vec3b color[6]; 
	color[0] = cv::Vec3b( 255, 0, 0 );
	color[1] = cv::Vec3b( 0, 255, 0 );
	color[2] = cv::Vec3b( 0, 0, 255 );
	color[3] = cv::Vec3b( 255, 255, 0 );
	color[4] = cv::Vec3b( 255, 0, 255 );
	color[5] = cv::Vec3b( 0, 255, 255 );
	UINT8 bodyInt[6]={15,55,95,135,175,215};//用于区别6个人

	hResult = pBodyIndexReader->AcquireLatestFrame(&pBodyIndexFrame);//Frame是真正存储数据的类，每一次都让Reader读到的数据传到Frame中
	if (SUCCEEDED(hResult))
	{
		unsigned int nBodyIndexBufferSize = 0;
		//unsigned char* pBodyIndexBuffer = nullptr;
		hResult = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);  //主要就是把Frame中的数据转存到一个数组中  pBodyIndexBuffer就是一个424*512大小的16位unsigned int数组，用来存储深度数据
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

	hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);//获取到最近的深度帧
	if (SUCCEEDED(hResult))
	{
		hResult = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);  //得到一个深度帧数据的点
		for(int i=0;i<DepthHeight;i++)
		{
			ushort* ptrDepth16 = depthMat16.ptr<ushort>(i);  // 16
			uchar* ptrDepth8 = depthMat8.ptr<uchar>(i);  //8

			for(int j=0;j<DepthWidth;j++)
			{
				USHORT depth = *pDepthBuffer;  //16   以两个字节为单位进行读取
				ptrDepth16[j] = depth;        //直接存储显示
				ptrDepth8[j] = depth%256; //转为8位存储显示
				pDepthBuffer++;               //16   以两个字节为单位进行读取
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
	RGBQUAD *pColorBuffer = NULL;    //RGBQUAD是一个结构体，其保存一个像素点的RGB值         
	hResult = pColorReader->AcquireLatestFrame(&pColorFrame);  //获取到最后的彩色帧
	if (SUCCEEDED(hResult))
	{
		hResult = pColorFrame->CopyConvertedFrameDataToArray(ColorHeight*ColorWidth * 4, reinterpret_cast<BYTE*>(img.data), ColorImageFormat::ColorImageFormat_Bgra);  //彩色帧数据转变成要求的格式。
		if (SUCCEEDED(hResult))
		{
			hResult = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));  //得到颜色帧数据的点
			cv::resize( img, color, cv::Size(), 0.5, 0.5 );
			//imshow("1",bodyMat);
		}
	}

	SafeRelease( pColorFrame );
}

void Kinect::bodyProcess2()
{  //1

	//在每次使用骨骼点的前先把骨骼点信息置0
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

	hResult = pBodyIndexReader->AcquireLatestFrame(&pBodyIndexFrame);//Frame是真正存储数据的类，每一次都让Reader读到的数据传到Frame中
	if (SUCCEEDED(hResult))
	{
		hResult = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);  //主要就是把Frame中的数据转存到一个数组中  pBodyIndexBuffer就是一个424*512大小的16位unsigned int数组，用来存储深度数据
	}


	hResult = pDepthSource->OpenReader( &pDepthReader );           
	if ( FAILED( hResult ) )
	{
		cerr <<"Error:IDepthFrameSource::OpenReader()"<<endl;
	}

	//深度数据
	if (SUCCEEDED(hResult))
	{     //2
		//IDepthFrame* pDepthFrame = nullptr;
		UINT nDepthBufferSize = 0;
		UINT16 *pDepthBuffer = NULL;

		hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);//获取到最近的深度帧
		if (SUCCEEDED(hResult))
		{
			hResult = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);  //得到一个深度帧数据的点
		}


		hResult = pColorSource->OpenReader( &pColorReader );     
		if ( FAILED( hResult ) )
		{
			cerr<<"Error:IColorFrameSource::OpenReader()"<<endl;
		}
		//彩色数据
		if (SUCCEEDED(hResult))
		{  //3
			//IColorFrame  *pColorFrame = nullptr;
			UINT nColorBufferSize = 0;
			RGBQUAD *pColorBuffer = NULL;    //RGBQUAD是一个结构体，其保存一个像素点的RGB值         
			hResult = pColorReader->AcquireLatestFrame(&pColorFrame);  //获取到最后的彩色帧
			if (SUCCEEDED(hResult))
			{
				hResult = pColorFrame->CopyConvertedFrameDataToArray(ColorHeight*ColorWidth * 4, reinterpret_cast<BYTE*>(img.data), ColorImageFormat::ColorImageFormat_Bgra);  //彩色帧数据转变成要求的格式。
				if (SUCCEEDED(hResult))
				{
					hResult = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));  //得到颜色帧数据的点
				}
			}

			//骨骼信息向彩色图映射
			//IBodyFrame* pBodyFrame = nullptr;
			hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
			if( SUCCEEDED( hResult ) )
			{    //4
				IBody* pBody[BODY_COUNT] = { 0 }; //默认的是 6 个骨骼 ，初始化所有的骨骼信息
				hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );//更新骨骼数据，
				if( SUCCEEDED( hResult ) )
				{   //5  

					static float tmpDepth[6]={0,0,0,0,0,0}; //记录上次的深度值，用于此次和上次的比较是否有变化
					for( int count = 0; count < BODY_COUNT; count++ )
					{  //count数从0到5，6个人找骨骼来显示
						BOOLEAN bTracked = false;    //初始化“能追踪到人体”的值为否。
						hResult = pBody[count]->get_IsTracked( &bTracked );  //确认能着追踪到人体。
						if( SUCCEEDED( hResult ) && bTracked )
						{
							Joint joint[JointType::JointType_Count];   //取得人体Joint(关节)。JointType是一个枚举类型，不同位置的关节点都是不同的标号表示的。count是一个数值25。
							hResult = pBody[ count ]->GetJoints( JointType::JointType_Count, joint );  //取得人体Joint(关节)。


				
							//用于实时显示的数据
							DepthShow[count]=joint[JointType_Neck].Position.Z;

							if (DepthShow[count]!=10)
							{
								cout<<"索引号 "<<count<<" 的深度值为： "<<DepthShow[count]<<endl;
								DepthShow[count]=10;  //显示结束后立即置为初始值

							}

							//用于排序的一个数据，用于主函数中作最近的人判断
							DepthSort[count]=joint[JointType_Neck].Position.Z;
							

							if( SUCCEEDED( hResult ) )
							{
								CvPoint skeletonPoint[BODY_COUNT][JointType_Count] = { cvPoint(0,0) };							
								// Joint
								for( int type = 0; type < JointType::JointType_Count; type++ )
								{  //遍历25个关节点
									ColorSpacePoint colorSpacePoint = { 0 };   //ColorSpacePoint结构变量，后一个colorSpacePoint实例化对象。 Represents a 2D point in color space, expressed in pixels.代表了一种2d点在颜色空间中,用像素表示。
									pCoordinateMapper->MapCameraPointToColorSpace( joint[type].Position, &colorSpacePoint );//摄像头点空间到颜色空间的转换，把关节点的坐标转换到颜色空间上，便于显示
								}
							}				
						}
					}				
				

					vector<DepthSpacePoint>depthspacepoints(ColorWidth*ColorHeight); 
					if (SUCCEEDED(hResult))
					{
						pCoordinateMapper->MapColorFrameToDepthSpace(DepthWidth * DepthHeight, (UINT16*)pDepthBuffer, ColorWidth * ColorHeight, &depthspacepoints[0]);  //从颜色空间到深度空间的坐标映射，叫做变换也好  depthspacepoints[0]是不是数组的首地址？应该是
#pragma omp parallel for  
						for (int i = 0; i < ColorHeight; i++)
							for (int j = 0; j < ColorWidth; j++)
							{
								int k = i*ColorWidth + j;//循环遍历每一个像素
								DepthSpacePoint p = depthspacepoints[k];  //用匹配的深度图像代表每一个像素。  DepthSpacePoint是一个结构体 p是结构体类型的变量

								int depthX = (int)(p.X + 0.5f); //用这个结构体类型变量p调用结构体中的成员变量，若把p换位k不行,因为k定义为是一个整形的变量，所以上一句的功能就是把获取到的每一像素都传给结构体类型的变量
								int depthY = (int)(p.Y + 0.5f);

								if ((depthX >= 0 && depthX < DepthWidth) && (depthY >= 0 && depthY < DepthHeight))
								{
									BYTE player = pBodyIndexBuffer[depthX + (depthY * DepthWidth)];  //BYTE即unsigned char,把获取到的每一个像素都交给player
									if (player == 0xff)  //深度空间if句判断每一个像素是不是属于人体，0~5分别表示可识别的6个人，0xff表示该像素不属于人（或说可能属于第7个人，但kinect识别不了，所以就不把该像素当做人体的了）
									{ 
										img.at <Vec4b>(i,j) = Vec4b(255,255,255,255); //彩色空间处理：0，0，0表示黑色，255表示alpha透明度,数字越小越透明
									}//这里并没写一个判断找到人体怎么办，默认就是不进行处理，那么就是直接显示咯
								}
								else  //这个else是和上上个if匹配的，表示如果在搜索空间（搜索人体）之外的地方也需要置为黑色
									img.at <Vec4b>(i, j) = Vec4b(255,255,255,255);
							}//彩色空间遍历双层循环在这结束
					}  // pCoordinateMapper

					cv::resize( img, bodyMat, cv::Size(), 0.5, 0.5 );
					//imshow("BODY", bodyMat);

					//骨骼坐标获取
					pBodyFrame->GetAndRefreshBodyData(_countof(pBody), pBody); 
					for (int i = 0; i < BODY_COUNT; ++i)  
					{   //6
						IBody* pBody1 = pBody[i];
						if (pBody1)   //索引到的人的序号，在这里就是判断是不是空指针。
						{    //7
							BOOLEAN bTracked = false;   //是否已被跟踪
							hResult = pBody1->get_IsTracked(&bTracked);

							if (SUCCEEDED(hResult) && bTracked)  //如果索引到新的人体并且已能被跟踪到
							{  //8

								Joint joints[JointType_Count];//存储关节点类 Joint是一个结构体变量  JointType_Count=25

								//存储深度坐标系中的关节点位置
								//DepthSpacePoint *depthSpacePosition = new DepthSpacePoint[_countof(joints)];
								ColorSpacePoint *colorSpacePosition = new ColorSpacePoint[_countof(joints)];

								//获得关节点类
								hResult = pBody1->GetJoints(_countof(joints), joints);
								if (SUCCEEDED(hResult))
								{   //9
									for (int j = 0; j < _countof(joints); ++j)
									{
										//将关节点坐标从摄像机坐标系（-1~1）转到深度坐标系（424*512）
										//	pCoordinateMapper->MapCameraPointToDepthSpace(joints[j].Position, &depthSpacePosition[j]);
										pCoordinateMapper->MapCameraPointToColorSpace(joints[j].Position, &colorSpacePosition[j]);
									}   

									for (int j=0;j<25;j++)
									{
										state[i][j]=joints[j].TrackingState;
									}
									

									//所有的骨骼节点坐标编号
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

                //循环安全释放6个名为body的指针
				for( int count = 0; count < BODY_COUNT; count++ )
				{   
					SafeRelease( pBody[count] );
				}

				SafeRelease( pBodyFrame );   
			}  //4

			SafeRelease(pColorFrame);
		}  //颜色数据 3

		SafeRelease(pDepthFrame);
	} //深度数据 2

	//====================骨骼坐标获取end==============================================
	SafeRelease(pBodyIndexFrame);

} //1

void Kinect::segmentProcess2(int Miner)
{
	cout<<"索引号 "<<Miner<<" 已拍得 "<<NumOnce<<" 帧图像"<<endl;
	
	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	switch (Miner)
	{
	case 0:{dst = "F://数据输出//0//";break;}
	case 1:{dst = "F://数据输出//1//";break;}
	case 2:{dst = "F://数据输出//2//";break;}
	case 3:{dst = "F://数据输出//3//";break;}
	case 4:{dst = "F://数据输出//4//";break;}
	case 5:{dst = "F://数据输出//5//";break;}
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
		NumOnce++;  //某一个人体索引号累加
}

void Kinect::SegmentAllParts(float first_x,float first_y,float second_x,float second_y,int first_state,int second_state,int SelectExecute)
{

	if (SelectExecute==0)   //执行全身的处理
	{
		dst_temp=dst;

		sprintf_s( chari, "%05d", NumOnce);  
		dst_temp += chari;
		dst_temp += "_0";//全身
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
			int radius=fabs(first_y-second_y);//圆的半径

			arr_x[0]=second_x;arr_x[1]=second_x+radius;arr_x[2]=second_x,arr_x[3]=second_x-radius;  //所有的x坐标存在一个数组
			arr_y[0]=second_y-radius;arr_y[1]=second_y;arr_y[2]=first_y;arr_y[3]=second_y;         //所有的y坐标存在一个数组
			
		}
		else
		{
			double result;
			result = atan2 (first_y-second_y,first_x-second_x) * 180 / CV_PI;
			float len=sqrt(pow((first_x-second_x),2)+pow((first_y-second_y),2));  //求两个骨骼节点距离作为矩形的长度
			RotatedRect rRect=RotatedRect(Point2f((first_x+second_x)/2,(first_y+second_y)/2),Size2f(len,len/2),result); //定义一个旋转矩形    
			Point2f vertices[4];    
			rRect.points(vertices);

			arr_x[0]=vertices[0].x;arr_x[1]=vertices[1].x;arr_x[2]=vertices[2].x;arr_x[3]=vertices[3].x; //所有的x坐标存在一个数组
			arr_y[0]=vertices[0].y;arr_y[1]=vertices[1].y;arr_y[2]=vertices[2].y;arr_y[3]=vertices[3].y; //所有的y坐标存在一个数组
		}

		sort(arr_x,arr_x+4);  //升序排序
		sort(arr_y,arr_y+4);

		int length=arr_x[3]-arr_x[0]+1;
		int width=arr_y[3]-arr_y[0]+1;

		Mat ImgPart(width,length,CV_8UC4,Scalar(255,255,255));
		if((arr_y[0]>0)&&(arr_y[0]<1080)&&(arr_y[3]>0)&&(arr_y[3]<1080)&&(arr_x[0]>0)&&(arr_x[0]<1920)&&(arr_x[3]>0)&&(arr_x[3]<1920))
		{
			for(int i=arr_y[0];i<arr_y[3];i++)  //高
				for(int j=arr_x[0];j<arr_x[3];j++)//宽
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


//通过判断一个索引号拍到的帧数NumOnce来判断有没有拍到人
void Kinect::select2(int indexer)
{
	cout<<"索引号 "<<indexer<<" 已拍得: "<<NumOnce<<" 帧图像   "<<endl<<endl;
	
	float s[100];   //创建一个数组，用于对每一副图像的白色区域占比值保存下来,用100已结足够大了一个部位应该不会拍100帧

	Directory dir;  
	string path;  
	switch (indexer)
	{
	case 0: { path = "F:\\数据输出\\0\\";  break; }    
	case 1: { path = "F:\\数据输出\\1\\";  break; }   
	case 2: { path = "F:\\数据输出\\2\\";  break; }    
	case 3: { path = "F:\\数据输出\\3\\";  break; }   
	case 4: { path = "F:\\数据输出\\4\\";  break; }   
	case 5: { path = "F:\\数据输出\\5\\";  break; }  
	default: break;
	}

	string exten;
	int size;

	if (NumOnce==0)
	{
		cout<<"没采集到任何图像"<<endl;
		//Num--;
	}
	else
	{
		cout<<"   多帧图像筛选..."<<endl;

		for (int parts=0;parts<=10;parts++)
		{
			switch (parts)
			{
			case 0: exten = "*_0.jpg";break;    //全身
			case 1: exten = "*_1.jpg";break;    //头部
			case 2: exten = "*_2.jpg";break;    //躯干
			case 3: exten = "*_3.jpg";break;    //右臂上
			case 4: exten = "*_4.jpg";break;    //右臂下
			case 5: exten = "*_5.jpg";break;    //左臂上
			case 6: exten = "*_6.jpg";break;    //左臂下
			case 7: exten = "*_7.jpg";break;    //右腿上
			case 8: exten = "*_8.jpg";break;    //右腿下
			case 9: exten = "*_9.jpg";break;    //左腿上
			case 10:exten = "*_10.jpg";break;   //左腿下
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

					const char * filename=fileFullName.c_str();  //把string类型的路径转换成char形

					IplImage *src=cvLoadImage (filename, 0);   //直接转换成灰度图

					int n=0;  //统计白色像素的个数  
					for(int i=0;i<src->height;i++)
						for(int j=0;j<src->width;j++)
						{
							if(((uchar *)(src->imageData + i*src->widthStep))[j]==255)  //计算灰度图中白色像素的个数
								n++; 
						}
						float per=(float)n/(src->height*src->width);  //计算白色区域在整幅图中的占比
						s[p]=per;	  //把多副图的占比值写入数组中

						cvReleaseImage(&src);

				}
				//==========找出最小的占比值======================
				float min=s[0]; //假设第一个是最小值
				int index=0;   //用于记录数组找到的最小值的下标

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
				{//改写！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
				//case 0:{ char cmd_0[255] = {0}; sprintf(cmd_0, "copy %s G:\\处理后\\图像数据\\%05d_0.jpg", filename,T_enter);system(cmd_0);break;}
				//case 1:{ char cmd_1[255] = {0}; sprintf(cmd_1, "copy %s G:\\处理后\\图像数据\\%05d_1.jpg", filename,T_enter);system(cmd_1);break;}
				//case 2:{ char cmd_2[255] = {0}; sprintf(cmd_2, "copy %s G:\\处理后\\图像数据\\%05d_2.jpg", filename,T_enter);system(cmd_2);break;}
				//case 3:{ char cmd_3[255] = {0}; sprintf(cmd_3, "copy %s G:\\处理后\\图像数据\\%05d_3.jpg", filename,T_enter);system(cmd_3);break;}
				//case 4:{ char cmd_4[255] = {0}; sprintf(cmd_4, "copy %s G:\\处理后\\图像数据\\%05d_4.jpg", filename,T_enter);system(cmd_4);break;}
				//case 5:{ char cmd_5[255] = {0}; sprintf(cmd_5, "copy %s G:\\处理后\\图像数据\\%05d_5.jpg", filename,T_enter);system(cmd_5);break;}
				//case 6:{ char cmd_6[255] = {0}; sprintf(cmd_6, "copy %s G:\\处理后\\图像数据\\%05d_6.jpg", filename,T_enter);system(cmd_6);break;}
				//case 7:{ char cmd_7[255] = {0}; sprintf(cmd_7, "copy %s G:\\处理后\\图像数据\\%05d_7.jpg", filename,T_enter);system(cmd_7);break;}
				//case 8:{ char cmd_8[255] = {0}; sprintf(cmd_8, "copy %s G:\\处理后\\图像数据\\%05d_8.jpg", filename,T_enter);system(cmd_8);break;}
				//case 9: {char cmd_9[255] = {0}; sprintf(cmd_9, "copy %s G:\\处理后\\图像数据\\%05d_9.jpg", filename,T_enter);system(cmd_9);break;}
				//case 10:{ char cmd_10[255] = {0}; sprintf(cmd_10, "copy %s G:\\处理后\\图像数据\\%05d_10.jpg", filename,T_enter);system(cmd_10);}break;

				case 0:{ string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_0.jpg";system(tmp.c_str());break;}
				case 1:{ string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_1.jpg";system(tmp.c_str());break;}
				case 2:{string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_2.jpg";system(tmp.c_str());break;}
				case 3:{ string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_3.jpg";system(tmp.c_str());break;}
				case 4:{ string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_4.jpg";system(tmp.c_str());break;}
				case 5:{ string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_5.jpg";system(tmp.c_str());break;}
				case 6:{ string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_6.jpg";system(tmp.c_str());break;}
				case 7:{string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_7.jpg";system(tmp.c_str());break;}
				case 8:{string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_8.jpg";system(tmp.c_str());break;}
				case 9: {string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_9.jpg";system(tmp.c_str());break;}
				case 10:{ string tmp; tmp="copy "+string(filename)+"  F:\\处理后\\图像数据\\"+timeIntToTimestampString(T_enter)+"_10.jpg";system(tmp.c_str());break;}
				default: break;
				}

				//=========删图=========================
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
		cout<<"当前已采集 "<<Num<<" 个人体"<<endl;
		cout<<"*******************************"<<endl<<endl;
	}
	NumOnce=0;  //把一个索引号的人所拍到的个数置为0.
}

//郝凯锋的程序
//detPeopleDepth3函数的使用需要这个函数先获取深度值
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
				USHORT depth = *pBuffer;  //16   以两个字节为单位进行读取
				ptrDepth16[j] = depth;        //直接存储显示
				ptrDepth8[j] = depth%256; //转为8位存储显示
				pBuffer++;               //16   以两个字节为单位进行读取
			}
		}
	}
	SafeRelease( pDepthFrame );  //这个frame一定要释放，不然无法更新
}

//判断一个特定区域的深度值，来选择是不是要执行人体存图和部位的分割的工作。
short Kinect::detPeopleDepth3()//通过简单的中值滤波来判断情况
{

	return medianOut(depthMat16,rec_tl,rec_br); //输出滤波后的结果
}

//用于挑选一个中值的滤波函数
short Kinect::medianOut(Mat&src,Point &tl,Point &br)//对16位深度图像进行滤波
{
	vector<short> tempVec;
	for(int i=tl.y;i<br.y;i++)//先把矩阵中非零的元素存储起来
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
	sort(tempVec.begin(),tempVec.end()); //先排序
	if (tempVec.size()==0)
	{
		return 0;
	}
	return tempVec[tempVec.size()/2]; // 后输出
}

string Kinect::timeIntToTimestampString(time_t input)//time_t转换成timestamp   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!临时
{
	tm *pTmp=localtime(&input);
	char pStr[15]={};
	sprintf(pStr,"%d%d%d%d%d%d",pTmp->tm_year+1900,pTmp->tm_mon+1,pTmp->tm_mday+1,pTmp->tm_hour,pTmp->tm_min,pTmp->tm_sec);
	return string(pStr);
}
