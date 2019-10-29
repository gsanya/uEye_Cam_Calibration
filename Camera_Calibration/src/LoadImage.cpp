#include "LoadImage.hpp"


void initializeCameraInterface(HIDS* hCam) {
	// Open cam and see if it was succesfull
	INT nRet = is_InitCamera(hCam, NULL);
	if (nRet == IS_SUCCESS) {
		std::cout << "Camera initialized!" << std::endl;
	}


	

	// Setting the pixel clock to retrieve data
	
	UINT nRange[3];
	is_PixelClock(*hCam, IS_PIXELCLOCK_CMD_GET_RANGE, (void*)nRange, sizeof(nRange));
	std::cout << "pixel clock range: " << nRange[0] << "-" << nRange[1] << "\t increment: " << nRange[2]<<std::endl;
	UINT nPixelClock = nRange[0];
	nRet = is_PixelClock(*hCam, IS_PIXELCLOCK_CMD_SET, (void*)&nPixelClock, sizeof(nPixelClock));
	UINT nPixelClockValue;
	is_PixelClock(*hCam, IS_PIXELCLOCK_CMD_GET, (void*)&nPixelClockValue, sizeof(nPixelClockValue));
	std::cout << "pixel range: " << nPixelClockValue << std::endl;
	
	if (nRet == IS_SUCCESS) {
		std::cout << "Camera pixel clock succesfully set!" << std::endl;
	}
	else if (nRet == IS_NOT_SUPPORTED) {
		std::cout << "Camera pixel clock setting is not supported!" << std::endl;
	}
	/*
	double FPS=0;
	double newFPS=0;
	is_SetFrameRate(*hCam, FPS, &newFPS);
	*/
	double min = 0, max = 0, increment = 0;
	is_GetFrameTimeRange(*hCam, &min, &max, &increment);
	std::cout << "FrameTimeRange: " << min << "-" << max << "\t increment: " << increment;


	double pdExposureRange[3];
	is_Exposure(*hCam, IS_EXPOSURE_CMD_GET_EXPOSURE_RANGE, (void*)pdExposureRange, 24); 
	std::cout << "exposure range: " << pdExposureRange[0] << "-" << pdExposureRange[1] << "\t increment: " << pdExposureRange[2] << std::endl;
	is_Exposure(*hCam, IS_EXPOSURE_CMD_GET_EXPOSURE, (void*)pdExposureRange, 8);
	std::cout << "exposure: " << pdExposureRange[0] << " [ms]"<<std::endl;

	if (nRet == IS_SUCCESS) {
		std::cout << "Camera exposure succesfully set!" << std::endl;
	}
	else if (nRet == IS_NOT_SUPPORTED) {
		std::cout << "Camera pixel clock setting is not supported!" << std::endl;
	}

	// Set the color mode of the camera
	//INT colorMode = IS_CM_MONO8;
	INT colorMode = IS_CM_BGR8_PACKED;
	nRet = is_SetColorMode(*hCam, colorMode);

	if (nRet == IS_SUCCESS) {
		std::cout << "Camera color mode succesfully set!" << std::endl;
	}

	// Store image in camera memory --> option to chose data capture method
	// Then access that memory to retrieve the data
	INT displayMode = IS_SET_DM_DIB;
	nRet = is_SetDisplayMode(*hCam, displayMode);

	if (nRet == IS_SUCCESS) {
		std::cout << "Display mode succesfully set!" << std::endl;
	}

	INT triggerMode = IS_SET_TRIGGER_OFF;
	nRet = is_SetExternalTrigger(*hCam, triggerMode);

	if (nRet == IS_SUCCESS) {
		std::cout << "Trigger mode succesfully set!" << std::endl;
	}
	triggerMode = IS_GET_EXTERNALTRIGGER;
	int trigMode = is_SetExternalTrigger(*hCam, triggerMode);
	std::cout << "Trigger mode: " << trigMode << std::endl;

}

// Capture a frame and push it in a OpenCV mat element
void getFrame(HIDS* hCam, int width, int height, cv::Mat& mat) {
	// Allocate memory for image
	char* pMem = NULL;
	int memID = 0;
	is_AllocImageMem(*hCam, width, height, 8*3, &pMem, &memID);

	// Activate the image memory for storing the frame captured
	// Grabbing the image
	// Getting the data of the frame and push it in a Mat element
	is_SetImageMem(*hCam, pMem, memID);
	//IS_WAIT: waits till the image is in the memory
	int nRet=is_FreezeVideo(*hCam, IS_WAIT);
	
	

	VOID* pMem_b;
	int retInt = is_GetImageMem(*hCam, &pMem_b);
	if (retInt != IS_SUCCESS) {
		std::cout << "Image data could not be read from memory!" << std::endl;
	}
	memcpy(mat.ptr(), pMem_b, mat.cols * mat.rows*3);
	is_FreeImageMem(*hCam, pMem, memID);
}

void coutError(int Error)
{
	switch (Error)
	{
	case IS_CANT_COMMUNICATE_WITH_DRIVER:
		std::cout << "IS_CANT_COMMUNICATE_WITH_DRIVER";
		break;
	case IS_BAD_STRUCTURE_SIZE:
		std::cout << "IS_BAD_STRUCTURE_SIZE";
		break;
	case IS_CAPTURE_RUNNING:
		std::cout << "IS_CAPTURE_RUNNING";
		break;
	case IS_INVALID_BUFFER_SIZE:
		std::cout << "IS_INVALID_BUFFER_SIZE" ;
		break;
	case IS_INVALID_CAMERA_TYPE:
		std::cout << "IS_INVALID_CAMERA_TYPE" ;
		break;
	case IS_INVALID_EXPOSURE_TIME:
		std::cout << "IS_INVALID_EXPOSURE_TIME" ;
		break;
	case IS_INVALID_CAMERA_HANDLE:
		std::cout << "IS_INVALID_CAMERA_HANDLE" ;
		break;
	case IS_INVALID_MEMORY_POINTER:
		std::cout << "IS_INVALID_MEMORY_POINTER" ;
		break;
	case IS_INVALID_PARAMETER:
		std::cout << "IS_INVALID_PARAMETER" ;
		break;
	case IS_IO_REQUEST_FAILED:
		std::cout << "IS_IO_REQUEST_FAILED" ;
		break;
	case IS_NO_ACTIVE_IMG_MEM:
		std::cout << "IS_NO_ACTIVE_IMG_MEM" ;
		break;
	case IS_NO_USB20:
		std::cout << "IS_NO_USB20" ;
		break;
	case IS_NO_SUCCESS:
		std::cout << "IS_NO_SUCCESS" ;
		break;
	case IS_NOT_CALIBRATED:
		std::cout << "IS_NOT_CALIBRATED" ;
		break;
	case IS_NOT_SUPPORTED:
		std::cout << "IS_NOT_SUPPORTED" ;
		break;
	case IS_OUT_OF_MEMORY:
		std::cout << "IS_OUT_OF_MEMORY" ;
		break;
	case IS_TIMED_OUT:
		std::cout << "IS_TIMED_OUT" ;
		break;
	case IS_SUCCESS:
		std::cout << "IS_SUCCESS" ;
		break;
	case IS_TRANSFER_ERROR:
		std::cout << "IS_TRANSFER_ERROR" ;
		break;
	default:
		break;
	}
}