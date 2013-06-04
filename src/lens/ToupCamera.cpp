#include "ToupCamera.h"
#include <iostream>

using namespace lens;




ToupCamera::ToupCamera(const wchar_t *serialNumber) : m_camera(), frameCounter(0), async(false) {
	m_convertedImage =cv::Mat(0,0,0);
	resolutionIndex = 1;

	if (serialNumber){
		//TODO: create deletion function
		m_camera = make_shared<HToupCam> (Toupcam_Open(serialNumber));
	} else {
		//Line 56 in toupcamdemocpp
		//List of toupcams
		ToupcamInst ti[TOUPCAM_MAX];
		unsigned cnt = Toupcam_Enum(ti);
		if (cnt > 0)
		{
			//Open first ToupCam
			//TODO: create deletion function
			m_camera = make_shared<HToupCam>(Toupcam_Open(ti[0].id));
		}
	}

	if (m_camera)
	{
		HRESULT sizeResult, autoExpo, expoTime, processModeResult;
		sizeResult = Toupcam_put_eSize(*m_camera, resolutionIndex);
		HRESULT a = Toupcam_get_ResolutionNumber(*m_camera);
		autoExpo = Toupcam_put_AutoExpoEnable(*m_camera, false);
		expoTime = Toupcam_put_ExpoTime(*m_camera, 5000000);
		processModeResult = Toupcam_put_ProcessMode(*m_camera, TOUPCAM_PROCESSMODE_FULL);
		open();
	}

	

}

void ToupCamera::StaticDataCallback(const void* pData, const BITMAPINFOHEADER* pHeader, BOOL bStill, void* pCtx){
	//Line 42 in toupcamdemocpp
	//create an instance pointer
	std::cout << "captured." << std::endl;
	ToupCamera *pThis = (ToupCamera*) pCtx;

	char *imagePtr = (char*) pData;
	if (pThis && pHeader && pData){
		pThis->addFrame(pData, pHeader, bStill);
	}
}

void ToupCamera::addFrame(const void* pData, const BITMAPINFOHEADER* pHeader, BOOL bStill){
	if (!pData || !pHeader) return;

	ULONG Time = 0;
	Toupcam_get_ExpoTime(*m_camera, &Time);

	char *imagePtr = (char*) pData;
	width = pHeader->biWidth;
	height = pHeader->biHeight;
	cv::Size	m_cvSize(pHeader->biWidth, pHeader->biHeight);
	WORD m_bitCount = pHeader->biBitCount;
	std::cout << "captured" << std::endl;
	int nChannels;

	//In most cases you could replace this with 
	//nChannels = (m_bitCount / 8)
	switch (m_bitCount){
		case 8:		nChannels = 8;
		case 16:	nChannels = 16;
		case 24:	nChannels = 24;
	}

	m_convertedImage = cv::Mat(height, width, CV_8UC3, (char*) pData);
	int x = 0;
	x++;
}

bool ToupCamera::open(void){
	if (m_camera){
		HRESULT startResult = Toupcam_Start(*m_camera, StaticDataCallback, this);
		return SUCCEEDED(startResult);
	} else {
		return false;
	}
}

bool ToupCamera::close(void){
	if (m_camera){
		Toupcam_Close(*m_camera.get());
		m_camera.reset();
	}
	return true;
}

int ToupCamera::getWidth(void){
	return width;
}

int ToupCamera::getHeight(void){
	return height;
}

IplImage* ToupCamera::getFrame(void){
	return nullptr;
}

cv::Mat ToupCamera::getFrameMat(void){
	if (!*m_camera || !m_convertedImage.rows || !m_convertedImage.data){ return cv::Mat(0,0,0); }
	return m_convertedImage;
}

bool ToupCamera::captureAsync(void){
	if (!m_camera) return false;
	async = true;
	HRESULT a = Toupcam_Snap(*m_camera, resolutionIndex);
	return SUCCEEDED(a);
}