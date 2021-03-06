/*
Filename:		Camera.h
Author:			Nikolaus Karpinsky
Date Created:	09/20/10
Last Edited:	09/20/10

Revision Log:
09/20/10 - Nik Karpinsky - Original creation.
*/

#ifndef _LENS_CAMERA_H_
#define _LENS_CAMERA_H_

#include <string>
#include <vector>

#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"

namespace lens
{
  class ICamera
  {
	

  public:
	virtual bool	  open(void)      = 0;
	virtual bool	  close(void)	  = 0;
	virtual int		  getWidth(void)  = 0;
	virtual int		  getHeight(void) = 0;
	virtual IplImage* getFrame(void)  = 0;
  };
}

#endif	// _LENS_CAMERA_H_
