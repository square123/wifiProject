extern "C"{
	/****************************************************************************                                       
	* 函数意义		: 打开与设备的连接											*
	* 返回值			: 成功与否(1=成功;非1代表错误码)								*				*
	*****************************************************************************/
	int		__stdcall	LIVESCAN_Init();
	/****************************************************************************                                       
	* 函数意义		: 关闭与设备的连接											*
	* 返回值			: 成功与否(1=成功;非1代表错误码)								*				*
	*****************************************************************************/
	int		__stdcall	LIVESCAN_Close();

	int		__stdcall	LIVESCAN_GetChannelCount();
	/****************************************************************************                                       
	* 函数意义		: 设置当前亮度											    *
	* 返回值			: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 nBright  : 亮度值                                                     *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_SetBright(int nChannel, int nBright);
	/****************************************************************************                                       
	* 函数意义		: 设置对比度                							        *
	* 返回值			: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 nContrast: 对比度值                                                   *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_SetContrast(	int nChannel, int nContrast );
	/****************************************************************************                                       
	* 函数意义		: 获取当前亮度                							    *
	* 返回值			: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 pnBright : 得到当前的亮度值                                            *
	*****************************************************************************/
	int __stdcall LIVESCAN_GetBright(int nChannel,int *pnBright);
	/****************************************************************************                                       
	* 函数意义		: 获取当前对比度                							    *
	* 返回值			: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 pnBright : 得到当前的对比度值                                          *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetContrast(	int nChannel, int *pnContrast );
	/****************************************************************************                                       
	* 函数意义		: 获得图像的最大高度和宽度                				    *
	* 返回值			: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 pnWidth  : 得到图像的最大宽度                                          *
	* 参数 pnHeight : 得到图像的最大高度                                          *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetMaxImageSize(	int nChannel, int *pnWidth, int *pnHeight  );

	/****************************************************************************                                       
	* 函数意义		: 获得当前的窗口位置                      				    *
	* 返回值		: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 pnOriginX: 得到当前X坐标值                                             *
	* 参数 pnOriginY: 得到当前Y坐标值                                             *
	* 参数 pnWidth  : 得到图像宽度                                                *
	* 参数 pnHeight : 得到图像高度                                                *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetCaptWindow(	int nChannel, int *pnOriginX, int *pnOriginY, int *pnWidth, int *pnHeight);

	/****************************************************************************                                       
	* 函数意义		: 设置当前的窗口位置                      				    *
	* 返回值			: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 pnOriginX: X坐标值                                                    *
	* 参数 pnOriginY: Y坐标值                                                    *
	* 参数 pnWidth  : 图像宽度                                                   *
	* 参数 pnHeight : 图像高度                                                   *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_SetCaptWindow(	int nChannel, int nOriginX, int nOriginY  , int nWidth, int nHeight  );

	//调用设置对话框
	int		__stdcall	LIVESCAN_Setup();

	int		__stdcall	LIVESCAN_BeginCapture(	int nChannel );

	int		__stdcall	LIVESCAN_GetFPRawData(int nChannel,unsigned char *pRawData);

	/****************************************************************************                                       
	* 函数意义		: 获取图像数据并显示出来                     				    *
	* 返回值			: 成功与否(1=成功;非1代表错误码)	                            *
	* 参数 nChannel : 通道号                                                     *
	* 参数 pRawData : 存放图像数据                                               *
	* 参数 pnWidth  : 需显示图像的宽度                                            *
	* 参数 pnHeight : 需显示图像的高度                                            *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetFPBmpData(int nChannel, unsigned char *pBmpData);

	int		__stdcall	LIVESCAN_EndCapture(int nChannel );

	int		__stdcall	LIVESCAN_IsSupportSetup();

	int		__stdcall	LIVESCAN_GetVersion();

	int		__stdcall	LIVESCAN_GetDesc(char pszDesc[1024]);

	int		__stdcall	LIVESCAN_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	int		__stdcall	LIVESCAN_SetBufferEmpty(unsigned char *pImageData, long imageLength);

}