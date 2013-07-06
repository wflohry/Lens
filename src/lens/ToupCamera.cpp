#include "ToupCamera.h"
#include <iostream>
#include "stdafx.h"

using namespace lens;


ToupCamera::ToupCamera(const wchar_t *serialNumber) : width(0), height(0), resIndex(1), mFrameReady(false){
	if (serialNumber){
		//TODO: create deletion function
		m_camera = make_shared<HToupCam> (Toupcam_Open(serialNumber));
	} else {
		//Get list of cameras and see if there is an available camera
		ToupcamInst ti[TOUPCAM_MAX];
		unsigned cnt = Toupcam_Enum(ti);
		if (cnt && camera >= 0 && camera < cnt){
			m_camera = make_shared<HToupCam>(Toupcam_Open(ti[camera].id));
			auto x = ti[camera].id;
			x++;
			open();
		}
	}
}

bool ToupCamera::open(){
	if (!m_camera) return false;

	//Resolution
	LONG longWidth, longHeight;
	HRESULT resResult = Toupcam_get_Resolution(*m_camera, resIndex, &longWidth, &longHeight);
	width = longWidth;
	height = longHeight;

	//Exposure
	Toupcam_put_ExpoAGain(*m_camera, 300);
	Toupcam_put_AutoExpoEnable(*m_camera, false);
	Toupcam_put_ExpoTime(*m_camera, 50000);

	//Processing
	Toupcam_put_ProcessMode(*m_camera, TOUPCAM_PROCESSMODE_FAST);
	Toupcam_put_RealTime(*m_camera, false);
	Toupcam_put_Speed(*m_camera, 0);

	//White Balance
	Toupcam_put_TempTint(*m_camera, 6503, 1000);
	Toupcam_put_Contrast(*m_camera, 0);
	Toupcam_put_Hue(*m_camera, 0);
	Toupcam_put_Saturation(*m_camera, 128);
	Toupcam_put_Brightness(*m_camera, 0);
	Toupcam_put_Gamma(*m_camera, 100);

	//Start up
	HRESULT startResult = Toupcam_Start(*m_camera, &ToupCamera::staticCallback, this);

	return SUCCEEDED(startResult);
}

void ToupCamera::addFrame(const void* pData, const BITMAPINFOHEADER* pHeader, BOOL bStill, void* pCallbackCtx){
	char *imagePtr = (char*) pData;
	ULONG width = pHeader->biWidth;
	ULONG height = pHeader->biHeight;
	mTripleBuffer.write = cv::Mat(height, width, CV_8UC3, imagePtr);
	mTripleBuffer.mutex.lock();
	mTripleBuffer.swapWrite();
	mFrameReady = true;
	mTripleBuffer.mutex.unlock();
}

cv::Mat ToupCamera::getFrameMat(void){
	if (!mFrameReady) return cv::Mat(0,0,0);
	mTripleBuffer.mutex.lock();
	mFrameReady = false;
	mTripleBuffer.swapRead();
	mTripleBuffer.mutex.unlock();

	return mTripleBuffer.write;
}

bool ToupCamera::snap(void)
{
	HRESULT snapped = Toupcam_Snap(*m_camera, resIndex);
	return SUCCEEDED(snapped);
}