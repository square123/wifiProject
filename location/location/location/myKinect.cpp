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
	//showOrbit矩阵初始化
	double maxDis=cos(-(rotateAngle)/180*PI)*4.5;
	double maxWid=2*maxDis*tan(35.0/180*PI);
	double minDis=cos(-(rotateAngle)/180*PI)*0.5;//KinectHeight*tan((60.0+(rotateAngle))/180*PI);
	double minWid=2*minDis*tan(35.0/180*PI);
	sOX=int(ceil(maxWid)*100),sOY=int(ceil(maxDis)*100);
	showOrbit=Mat::zeros(sOY,sOX,CV_8UC3);//背景为全黑
	CoordinateMapperMat.create(DepthHeight,DepthWidth,CV_8UC4);
	CoordinateMapperMat_TM.create(DepthHeight,DepthWidth,CV_8UC1);

	//填充白色区域
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

	for (int i=0;i<6;i++) //先将数据置空，即让数据为30这样能使程序更精简一些
	{
		storeTemp[i].magicNumber=30;
		storeTemp[i].strIndex=-1; //索引是-1代表没有更新
	}
	//memset(&del,0,sizeof(del));
	deee.open("deee.txt",ios::app);//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	histElement=getStructuringElement(MORPH_ELLIPSE,Size(5,5));
	memset(trackStateHold,-1,sizeof(trackStateHold)); //-1都表示数据没有跟踪到，1都代表跟踪到了，就是用一个数据来记忆之前发生的状态，从而让输出滞后一些
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
	time_t syscTimee=time(NULL);	//这里只执行一次，后续就不需要再操作了
	processIndex=syscTimee+4;//因为是要滞后处理，所以要将处理的标志位滞后，具体的滞后时间参数可以修改
}

HRESULT Kinect::InitColor()//必须先InitKinect()
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

HRESULT Kinect::InitDepth()//必须先InitKinect()
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

void Kinect::depthProcess()//16位显示会报错 不用该函数处理
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
	SafeRelease( pDepthFrame );  //这个frame一定要释放，不然无法更新
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
				USHORT depth = *pBuffer;  //16   以两个字节为单位进行读取
				ptrDepth16[j] = depth;        //直接存储显示
				ptrDepth8[j] = depth%256; //转为8位存储显示
				pBuffer++;               //16   以两个字节为单位进行读取
			}
		}
	}
	SafeRelease( pDepthFrame );  //这个frame一定要释放，不然无法更新
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
	SafeRelease( pColorFrame );  //这个frame一定要释放，不然无法更新
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
				imwrite(savePathTemp,a);//存储图像
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
				imwrite(savePathTemp,a);//存储图像
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
		imwrite(savePathTemp,depthDeTemp);//存储去噪的深度图像
		//imwrite(savePathTemp,depthMat8);//存储去噪的深度图像
		saveTmpIntDep++;   
		savePathTemp =savePath;
		sprintf_s(saveTmpStrClr,sizeof(saveTmpStrClr),"%03d",saveTmpIntClr); 
		savePathTemp = savePathTemp+"color" +"_"+ saveTmpStrClr + ".png";
		imwrite(savePathTemp,colorHalfSizeMat);//存储彩色图像
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
	time_t systime=time(NULL);//加入系统时间
	char timeFix[16];
	strftime(timeFix,sizeof(timeFix),"%Y%m%d%H%M%S",localtime(&systime));//时间修复
	hResult = pBodyReader->AcquireLatestFrame( &pBodyFrame );
	if( SUCCEEDED( hResult ) )
	{
		IBody* pBody[BODY_COUNT] = { 0 }; //默认的是 6 个骨骼 ，初始化所有的骨骼信息
		hResult = pBodyFrame->GetAndRefreshBodyData( BODY_COUNT, pBody );//更新骨骼数据，
		if( SUCCEEDED( hResult ) ){
			for( int count = 0; count < BODY_COUNT; count++ ){  //count数从0到5，6个人找骨骼来显示
				int histFlag=0;//这个是作为连续记录的标识。
				BOOLEAN bTracked = false;    //初始化“能追踪到人体”的值为否。
				hResult = pBody[count]->get_IsTracked( &bTracked );  //确认能追踪到人体。
				//用于连续记录的操作
				if (trackStateHold[count]==(bTracked==false?-1:1))
				{
					histFlag=1;
				}else
				{
					trackStateHold[count]=(bTracked==false?-1:1);
				}
				if( SUCCEEDED( hResult ) && bTracked && histFlag ){
					//应该在这个里面加入一个状态位的保持，将结果停顿一下，如果还和上次的状态相同就保持（可以新建立一个数组，在数组中保存之前的状态，如果状态和原来相同，则停顿一下不输出，下次再输出结果）
					Joint joint[JointType::JointType_Count];   //取得人体Joint(关节)。JointType是一个枚举类型，不同位置的关节点都是不同的标号表示的。count是一个数值25。
					hResult = pBody[ count ]->GetJoints( JointType::JointType_Count, joint );  //取得人体Joint(关节)。
					//cout<<"索引为"<<count<<"的坐标： "<<"X="<<joint[ JointType_SpineMid].Position.X<<" "<<"Y="<<joint[ JointType_SpineMid].Position.Y<<" "<<"Z="<<joint[ JointType_SpineMid].Position.Z<<endl;
					//坐标变换,只会在x方向上有角度
					double xx,yy,zz;
					xx=joint[ JointType_SpineMid].Position.X;
					yy=(cos(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Y+sin(rotateAngle/180*PI)*joint[JointType_SpineMid].Position.Z);
					zz=((-1)*sin(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Y+cos(rotateAngle/180*PI)*joint[ JointType_SpineMid].Position.Z);
					//cout<<timeFix<<" "<<"x="<<-xx<<"z="<<(zz-(sOY/200))<<endl;
					double r4=sqrt((-xx-r4x)*(-xx-r4x)+((zz-(sOY/200))-r4z)*((zz-(sOY/200))-r4z));
					double r3=sqrt((-xx-r3x)*(-xx-r3x)+((zz-(sOY/200))-r3z)*((zz-(sOY/200))-r3z));
					double r2=sqrt((-xx-r2x)*(-xx-r2x)+((zz-(sOY/200))-r2z)*((zz-(sOY/200))-r2z));
					double r1=sqrt((-xx-r1x)*(-xx-r1x)+((zz-(sOY/200))-r1z)*((zz-(sOY/200))-r1z));
					//坐标输出部分
						//数据处理部分
					cout<<reNewIndex(count)<<endl;
					kinectDataRawProecess(timeFix,count,r1,r2,r3,r4);
					if (systime>=processIndex)//进行插空操作 最终达到循环操作,改成大于等于是因为数据有可能存在跳秒
					{
						kinectDataProProecess();
						processIndex++;
					}
					//坐标显示部分
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
		for( int count = 0; count < BODY_COUNT; count++ ){   //循环安全释放6个名为body的指针
			SafeRelease( pBody[count] );
		}
	}
	SafeRelease( pBodyFrame );  //这个frame一定要释放，不然无法更新	
}

void Kinect::kinectDataRawProecess(char dataTime[14],int BIndex,double r1,double r2,double r3,double r4)//负责把距离信息存储下来
{
	int second=charTimeGetSecond(dataTime);
	memcpy(kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].Timestamp,dataTime,sizeof(char)*14);
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[0]=r1;
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[1]=r2;
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[2]=r3;
	kinectTimeRaw[second][BIndex][kinectTIIndex[second][BIndex]].r[3]=r4;
	kinectTIIndex[second][BIndex]++;
}

void Kinect::kinectDataProProecess()//负责将信息压缩到同一秒
{
	char timeFixed[16];
	time_t processIndexInit=processIndex-4;
	strftime(timeFixed,sizeof(timeFixed),"%Y%m%d%H%M%S",localtime(&processIndexInit));
	int second=charTimeGetSecond(timeFixed);
	for (int i=0;i<BODY_COUNT;i++)//每个bodycount
	{
		double r0Sum=0.0,r2Sum=0.0,r3Sum=0.0,r1Sum=0.0;//之前没有发现的错误，幸好之前是单人，错误没有加大，在多人下错误会非常大
		if (kinectTIIndex[second][i]!=0)//如果空不操作
		{
			for (int j=0;j<kinectTIIndex[second][i];j++)//计算变量
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
			//文件输出操作
			outfile<<timeFixed<<","<<i<<","<<r0ave<<","<<r1ave<<","<<r2ave<<","<<r3ave<<endl;
		}
	}
	//清空操作
	memset(kinectTimeRaw[second],0,sizeof(KinectDataRaw)*BODY_COUNT*maxStore);
	memset(kinectTIIndex[second],0,sizeof(int)*BODY_COUNT);
}

int Kinect::charTimeGetSecond(char ttt[14])//获得得到数据的后两位
{
	int result;
	char second[2];
	memcpy(second,ttt+12,sizeof(char)*2);
	result=atoi(second);
	return result;
}

void Kinect::CoordinateMapperProcess()
{
	//获取变换尺寸后的图像
	if ( SUCCEEDED(hResult) )
	{
		vector<ColorSpacePoint> colorSpacePoints( DepthHeight*DepthWidth );
		hResult = pCoordinateMapper->MapDepthFrameToColorSpace(DepthWidth*DepthHeight,reinterpret_cast<UINT16*>(depthDeTemp.data),DepthWidth*DepthHeight,&colorSpacePoints[0] );
		if ( SUCCEEDED(hResult) )
		{
			CoordinateMapperMat = Scalar( 0, 0, 0, 0);//需要重新刷新下图片
			CoordinateMapperMat_TM = 0;//需要重新刷新下图片
			for ( int y = 0; y < DepthHeight; y++)
			{
				for ( int x = 0; x < DepthWidth; x++)
				{
					unsigned int index = y * DepthWidth + x;
					ColorSpacePoint p = colorSpacePoints[index];
					int colorX = static_cast<int>( std::floor( p.X + 0.5 ) );  //std::floor向下取整，返回值是浮点数，但是前面static_cast<int>对其强制类型转换为整形来返回
					int colorY = static_cast<int>( std::floor( p.Y + 0.5 ) );
					if( ( colorX >= 0 ) && ( colorX < ColorWidth ) && ( colorY >= 0 ) && ( colorY < ColorHeight )){
						CoordinateMapperMat.at<cv::Vec4b>( y, x ) = colorMat.at<cv::Vec4b>( colorY, colorX );  //将深度图坐标映射后的对应的彩色信息图赋给匹配图
						CoordinateMapperMat_TM.at<UCHAR>( y, x ) = 255; //保存掩模，因为转换后的彩色图形和深度比有些数据是没有的，剔除黑色，方便计算
					}
				}
			}
		}
	}
	
}

void Kinect::bodyColorHistGet(int index,HistBind &dst) //得到了直方图的函数  //先测验下得到阈值
{
	//提取直方图用的数据
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
	erode(RMat,RMat,histElement);//去噪处理
	erode(GMat,GMat,histElement);
	erode(BMat,BMat,histElement);
	dilate(RMat,RMat,histElement);
	dilate(GMat,GMat,histElement);
	dilate(BMat,BMat,histElement);
	calcHist(&CoordinateMapperMat,1,channelsR,RMat,dst.redHist,1,histSize,ranges,true,false);
	calcHist(&CoordinateMapperMat,1,channelsG,BMat,dst.greenHist,1,histSize,ranges,true,false);
	calcHist(&CoordinateMapperMat,1,channelsB,GMat,dst.blueHist,1,histSize,ranges,true,false);
}

//比较三通道直方图的函数
double Kinect::histBindCompare(HistBind &src1,HistBind &src2)
{
	if (src2.blueHist.empty()||src1.blueHist.empty())
	{
		return 0.0;
	}
	//要有一个全零的判断 (如果为零，则直方图对应的数值会变成1，所以不行)
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


int Kinect::reNewIndex(int index) //这个索引更新不一定对，不过值得一试  不过说不定计算还挺快的。感觉在获取索引值时，第一帧是不会获取直方图的数据的。因为设置一个延迟，延迟的机制就设置在外面
{
	
	int resultIndex=index;
	HistBind tempBind;
	bodyColorHistGet(index,tempBind); //先得到基本的数据
	double maxNumber=0.0;
	//复杂的方法是不好的，用交换的思想试试    当索引是30为空，已使用用0表示，其他的如果没有用到即不为30和0的数，每次都加一，当加到30时索引变为30（即为空）
	//每次先找对应索引的直方图，如果效果很差，寻找其他的，如果找到最好的，且大于阈值。则交换两组数据
	maxNumber=histBindCompare(tempBind,storeTemp[index].hist);
	cout<<"原本对应索引下的直方图系数："<<maxNumber<<endl;
	//先比较对应索引的值
	if (maxNumber>0.5) //阈值可以调
	{
		//如果直方图对应相同就不再进行索引的更换
		storeTemp[index].hist=tempBind;  //更新数据
		storeTemp[index].magicNumber=0;	//更新标志位
		resultIndex=storeTemp[index].strIndex;//索引替换
	}
	else  //如果不匹配就完成交换（效果更好），然后就是考虑如何将一些已经过时的直方图记忆删除掉
	{
		//找到值最大的索引
		int maxTemp=index;
		for (int i=0;i<6;i++)
		{
			if ((i!=index)&&storeTemp[i].magicNumber!=30)//当对应索引不正确时，遍历非空的数据
			{
				//新建一个文件存下来
				deee<<"直方图1:"<<tempBind.redHist<<endl;
				deee<<"直方图2:"<<storeTemp[i].hist.redHist<<endl;
				double dell=histBindCompare(tempBind,storeTemp[i].hist);
				//比较了就加1，不用管其他的。因为如果后续能用的到的话就会又重新置为0的
				storeTemp[i].magicNumber++;
				cout<<i<<"比较后的直方图系数："<<dell<<endl;
				if(dell>maxNumber) //就是找更合适的
				{
					maxNumber=dell;
					maxTemp=i;
				}
			}
		}
		if (maxNumber>0.5) //这个标准应该要统一，如果确定找到了索引，就将旧的信息和现在进行交换
		{
			storeTemp[maxTemp].hist=storeTemp[index].hist;  //因为要更新数据，所以所谓的交换就是将原来的数据放到选好的这个坑中
			++storeTemp[index].magicNumber;
			storeTemp[maxTemp].magicNumber=storeTemp[index].magicNumber;	//更新标志位
			resultIndex=storeTemp[maxTemp].strIndex;//输出索引的更新
			//storeTemp[maxTemp].strIndex=storeTemp[index].strIndex+storeTemp[maxTemp].strIndex;	//索引交换
			//storeTemp[index].strIndex=storeTemp[maxTemp].strIndex-storeTemp[index].strIndex;
			//storeTemp[maxTemp].strIndex=storeTemp[maxTemp].strIndex-storeTemp[index].strIndex;
			swap(storeTemp[maxTemp].strIndex,storeTemp[index].strIndex);
			storeTemp[index].hist=tempBind;
			storeTemp[index].magicNumber=0;	
			
		}
		else   //没有满足的话说明历史信息中确实没有这个，就找一个最大的魔数值进行交换，仍然维持原来的索引
		{
			//找最大的魔数值
			int maxMagNum=0;
			int maxMagNumIndex=index;
			set<int> quchong;//存储已经有的索引 索引标号为0-5
			for (int i=0;i<6;i++)
			{
				//if (i!=index)//当对应索引不正确时，遍历非空的数据
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
			//有可能只有一个索引就比较尴尬了
			//if (quchong.size()==0)
			//{
			//	quchong.insert(storeTemp[index].strIndex);
			//}
			//完成交换
			storeTemp[maxMagNumIndex].hist=storeTemp[index].hist;  //因为要更新数据，所以所谓的交换就是将原来的数据放到选好的这个坑中
			++storeTemp[index].magicNumber;
			storeTemp[maxMagNumIndex].magicNumber=storeTemp[index].magicNumber;	//更新标志位
			storeTemp[maxMagNumIndex].strIndex=storeTemp[index].strIndex;
			//因为是之前没有的，则重新赋值索引
			for(int l=0;l<5;l++)
			{
				if (quchong.find(l)==quchong.end())
				{
					//索引更新为之前没有出现过的索引
					storeTemp[index].strIndex=l;//因为有可能出现一个人消失一段时间后又重新出现，这样索引会发生冲突，比如新的人物是3，然而之前消失的人物也是3，这样再出现的会出错
					break;
				}
			}
			storeTemp[index].hist=tempBind;
			storeTemp[index].magicNumber=0;	
			resultIndex=storeTemp[index].strIndex;
		}
	}
	//最后再更新下魔数，如果很久都没有数据了，就把大于30的固定一下。
	for (int i=0;i<6;i++)
	{
		if (storeTemp[i].magicNumber!=0)
		{
			storeTemp[i].magicNumber++;
		}
		if (storeTemp[i].magicNumber>30)     //这个应该指的是30帧的数据，可以根据实际情况调整。
		{
			storeTemp[i].magicNumber=30;
			storeTemp[i].strIndex=-1;//将索引也重新置零
		}
	}
	//检验错误的（最后删掉）
	for (int k=0;k<6;k++)
	{
		cout<<k<<" "<<"存储的索引："<<storeTemp[k].strIndex<<" "<<"魔数："<<storeTemp[k].magicNumber<<endl;
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

