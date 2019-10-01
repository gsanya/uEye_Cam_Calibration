#include "LoadImage.hpp"


void initializeCameraInterface(HIDS* hCam_internal) {
	// Open cam and see if it was succesfull
	INT nRet = is_InitCamera(hCam_internal, NULL);
	if (nRet == IS_SUCCESS) {
		std::cout << "Camera initialized!" << std::endl;
	}

	// Setting the pixel clock to retrieve data
	UINT nPixelClockDefault = 21;
	nRet = is_PixelClock(*hCam_internal, IS_PIXELCLOCK_CMD_SET, (void*)&nPixelClockDefault, sizeof(nPixelClockDefault));

	if (nRet == IS_SUCCESS) {
		std::cout << "Camera pixel clock succesfully set!" << std::endl;
	}
	else if (nRet == IS_NOT_SUPPORTED) {
		std::cout << "Camera pixel clock setting is not supported!" << std::endl;
	}

	// Set the color mode of the camera
	//INT colorMode = IS_CM_MONO8;
	INT colorMode = IS_CM_RGB8_PACKED;
	nRet = is_SetColorMode(*hCam_internal, colorMode);

	if (nRet == IS_SUCCESS) {
		std::cout << "Camera color mode succesfully set!" << std::endl;
	}

	// Store image in camera memory --> option to chose data capture method
	// Then access that memory to retrieve the data
	INT displayMode = IS_SET_DM_DIB;
	nRet = is_SetDisplayMode(*hCam_internal, displayMode);

	if (nRet == IS_SUCCESS) {
		std::cout << "Display mode succesfully set!" << std::endl;
	}

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
	is_FreezeVideo(*hCam, IS_WAIT);

	VOID* pMem_b;
	int retInt = is_GetImageMem(*hCam, &pMem_b);
	if (retInt != IS_SUCCESS) {
		std::cout << "Image data could not be read from memory!" << std::endl;
	}
	memcpy(mat.ptr(), pMem_b, mat.cols * mat.rows);
	is_FreeImageMem(*hCam, pMem, memID);
}
