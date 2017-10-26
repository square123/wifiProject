extern "C"{
	/****************************************************************************                                       
	* ��������		: �����豸������											*
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)								*				*
	*****************************************************************************/
	int		__stdcall	LIVESCAN_Init();
	/****************************************************************************                                       
	* ��������		: �ر����豸������											*
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)								*				*
	*****************************************************************************/
	int		__stdcall	LIVESCAN_Close();

	int		__stdcall	LIVESCAN_GetChannelCount();
	/****************************************************************************                                       
	* ��������		: ���õ�ǰ����											    *
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� nBright  : ����ֵ                                                     *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_SetBright(int nChannel, int nBright);
	/****************************************************************************                                       
	* ��������		: ���öԱȶ�                							        *
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� nContrast: �Աȶ�ֵ                                                   *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_SetContrast(	int nChannel, int nContrast );
	/****************************************************************************                                       
	* ��������		: ��ȡ��ǰ����                							    *
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� pnBright : �õ���ǰ������ֵ                                            *
	*****************************************************************************/
	int __stdcall LIVESCAN_GetBright(int nChannel,int *pnBright);
	/****************************************************************************                                       
	* ��������		: ��ȡ��ǰ�Աȶ�                							    *
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� pnBright : �õ���ǰ�ĶԱȶ�ֵ                                          *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetContrast(	int nChannel, int *pnContrast );
	/****************************************************************************                                       
	* ��������		: ���ͼ������߶ȺͿ��                				    *
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� pnWidth  : �õ�ͼ��������                                          *
	* ���� pnHeight : �õ�ͼ������߶�                                          *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetMaxImageSize(	int nChannel, int *pnWidth, int *pnHeight  );

	/****************************************************************************                                       
	* ��������		: ��õ�ǰ�Ĵ���λ��                      				    *
	* ����ֵ		: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� pnOriginX: �õ���ǰX����ֵ                                             *
	* ���� pnOriginY: �õ���ǰY����ֵ                                             *
	* ���� pnWidth  : �õ�ͼ����                                                *
	* ���� pnHeight : �õ�ͼ��߶�                                                *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetCaptWindow(	int nChannel, int *pnOriginX, int *pnOriginY, int *pnWidth, int *pnHeight);

	/****************************************************************************                                       
	* ��������		: ���õ�ǰ�Ĵ���λ��                      				    *
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� pnOriginX: X����ֵ                                                    *
	* ���� pnOriginY: Y����ֵ                                                    *
	* ���� pnWidth  : ͼ����                                                   *
	* ���� pnHeight : ͼ��߶�                                                   *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_SetCaptWindow(	int nChannel, int nOriginX, int nOriginY  , int nWidth, int nHeight  );

	//�������öԻ���
	int		__stdcall	LIVESCAN_Setup();

	int		__stdcall	LIVESCAN_BeginCapture(	int nChannel );

	int		__stdcall	LIVESCAN_GetFPRawData(int nChannel,unsigned char *pRawData);

	/****************************************************************************                                       
	* ��������		: ��ȡͼ�����ݲ���ʾ����                     				    *
	* ����ֵ			: �ɹ����(1=�ɹ�;��1���������)	                            *
	* ���� nChannel : ͨ����                                                     *
	* ���� pRawData : ���ͼ������                                               *
	* ���� pnWidth  : ����ʾͼ��Ŀ��                                            *
	* ���� pnHeight : ����ʾͼ��ĸ߶�                                            *
	*****************************************************************************/
	int		__stdcall	LIVESCAN_GetFPBmpData(int nChannel, unsigned char *pBmpData);

	int		__stdcall	LIVESCAN_EndCapture(int nChannel );

	int		__stdcall	LIVESCAN_IsSupportSetup();

	int		__stdcall	LIVESCAN_GetVersion();

	int		__stdcall	LIVESCAN_GetDesc(char pszDesc[1024]);

	int		__stdcall	LIVESCAN_GetErrorInfo(int nErrorNo, char pszErrorInfo[256]);

	int		__stdcall	LIVESCAN_SetBufferEmpty(unsigned char *pImageData, long imageLength);

}