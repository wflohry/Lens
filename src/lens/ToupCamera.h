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

using namespace std;

namespace lens
{
	class ToupCamera : public ICamera
	{
	private:
		static void					StaticDataCallback(const void* pData, const BITMAPINFOHEADER* pHeader, BOOL bStill, void* pCtx);
		void						addFrame(const void* pData, const BITMAPINFOHEADER* pHeader, BOOL bStill);
		shared_ptr<HToupCam>		m_camera;
		cv::Mat			m_convertedImage;
		wchar_t						id[64];
		long						frameCounter;
		int							width, height;
		bool						async;
		ULONG						resolutionIndex;
	public:
		ToupCamera(const wchar_t *serialNumber = nullptr);

		bool						open(void);
		bool						close(void);
		int							getWidth(void);
		int							getHeight(void);
		IplImage*					getFrame(void);
		cv::Mat						getFrameMat(void);
		bool						captureAsync(void);
		
		/**
		* Sets the serial number of the camera to use. This function must be called
		* before connecting and powering up the camera: i.e. open() as it is used
		* to specify which camera should be loaded
		*/
		void		setSerialNumber(const wchar_t *serialNumber);
	};
}

#endif