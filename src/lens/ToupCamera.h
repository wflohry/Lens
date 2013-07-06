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

using namespace std;

class cvTripleBuffer {
	public:
		std::mutex mutex;
		cv::Mat write;
		cv::Mat read;
		void swapWrite(){cv::Mat temp = write; write = middle; middle = temp;};
		void swapRead(){cv::Mat temp = read; read = middle; middle = read;};
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
		ToupCamera(const wchar_t *serialNumber = nullptr);
		bool		open(void);
		bool		close(void) {return false;};
		int			getWidth(void){return width;};
		int			getHeight(void){return height;};
		IplImage*	getFrame(void){return nullptr;};
		bool		snap(void);
		const bool	frameReady(void){return mFrameReady;};
		cv::Mat		getFrameMat(void);
	};

}

#endif