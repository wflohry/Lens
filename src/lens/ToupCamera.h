/*
Filename:	ToupCamera.h
Author:		William Lohry
Date Created:	2013-06-01

*/

#ifndef _TOUP_CAMERA_H__
#define _TOUP_CAMERA_H__

#include <memory>
#include "ICamera.h"
#include "toupcam.h"
#include <functional>
#include <mutex>
#include <atomic>

using namespace std;

class cvTripleBuffer {
	public:
		std::mutex mutex;
		cv::Mat write;
		cv::Mat read;
		void swapWrite(){cv::Mat temp = write; write = middle; middle = temp;};
		void swapRead(){cv::Mat temp = read; read = middle; middle = temp;};
	private:
		cv::Mat middle;
};

namespace lens
{
	class ToupCamera : public ICamera
	{
	private:
		void					addFrame(const void* pData, const BITMAPINFOHEADER* pHeader, BOOL bStill, void* pCallbackCtx = nullptr);
		static void				staticCallback(const void* pData, const BITMAPINFOHEADER* pHeader, BOOL bStill, void* pCallbackCtx){((ToupCamera*) pCallbackCtx)->addFrame(pData, pHeader, bStill);};
		int width, height;
		shared_ptr<HToupCam>	m_camera;
		ULONG					resIndex; // resolution index, 0 is highest
		bool					mFrameReady;
		cvTripleBuffer			mTripleBuffer;
	public:
		//Initialize camera with pointer to serial
		ToupCamera(std::string serialNumber);
		ToupCamera(int cameraID);
		ToupCamera(){};
		~ToupCamera();
		bool				open(void);
		bool				close(void) {return false;};
		int					getWidth(void){return width;};
		int					getHeight(void){return height;};
		IplImage*			getFrame(void){return nullptr;};
		bool				snap(void);
		const bool			frameReady(void){return mFrameReady;};
		cv::Mat				getFrameMat(void);
		const std::string	getSerialNumber(void);

		void				exposureAutoSet(bool enable) {Toupcam_put_AutoExpoEnable(*m_camera, enable);};
		void				exposureSet(unsigned long exposureMicroseconds);
		void				exposureGainSet(unsigned short gain){Toupcam_put_ExpoAGain(*m_camera, gain);};
		const unsigned short exposureGainGet() {unsigned short gain; Toupcam_get_ExpoAGain(*m_camera, &gain); return gain;};
		const unsigned long	exposureGet(void) {unsigned long time; Toupcam_get_ExpoTime(*m_camera, &time); return time;};
	};

}

#endif