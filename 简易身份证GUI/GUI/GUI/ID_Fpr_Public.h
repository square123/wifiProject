extern "C"{
	int __stdcall FP_GetVersion(unsigned char code[4]);
	int __stdcall FP_Begin();
	int __stdcall FP_FeatureExtract(
		unsigned char cScannerType,
		unsigned char cFingerCode,
		unsigned char * pFingerImgBuf,
		unsigned char * pFeatureData
		);
	int __stdcall FP_FeatureMatch(
		unsigned char * pFeatureData1,
		unsigned char* pFeatureData2,
		float *	pfSimilarity
		);
	int __stdcall FP_ImageMatch(
		unsigned char * pFingerImgBuf,
		unsigned char * pFeatureData,
		float *	pfSimilarity
		);
	int __stdcall FP_Compress(
		unsigned char cScannerType, 
		unsigned char cEnrolResult,
		unsigned char cFingerCode,
		unsigned char * pFingerImgBuf,
		int nCompressRatio, 
		unsigned char *pCompressedImgBuf,
		unsigned charstrBuf[256]
	);
	int __stdcall FP_Decompress(
		unsigned char * pCompressedImgBuf,
		unsigned char *	pFingerImgBuf,
		unsigned char strBuf[256]
	);
	int __stdcall FP_GetQualityScore(unsigned char * pFingerImgBuf,unsigned char * pnScore);
	int __stdcall FP_GenFeatureFromEmpty1(
		unsigned char cScannerType,
		unsigned char cFingerCode,
		unsigned char * pFeatureData
		);
	int __stdcall FP_GenFeatureFromEmpty2(unsigned char cFingerCode,unsigned char * pFeatureData);
	int __stdcall FP_End();
}