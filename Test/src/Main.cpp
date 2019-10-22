#pragma once

#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include <ueye.h>

#include "LoadImage.hpp"




int main()
{
	// ---------------------------------------------------------------------------------------------------------------
	// START BY CONFIGURING THE INTERFACE WITH THE UEYE CAMERA
	// ---------------------------------------------------------------------------------------------------------------

	// Camera initialisation
	// Index 0 means taking the first camera available
	HIDS hCam = 1;
	initializeCameraInterface(&hCam);

	// ---------------------------------------------------------------------------------------------------------------
	// CAPTURE DATA AND PROCESS
	// ---------------------------------------------------------------------------------------------------------------

	while (true) {
		// Create an image and grab a frame
		cv::Mat current_image(2048, 2048, CV_8UC3);
		getFrame(&hCam, 2048, 2048, current_image);
		cv::resize(current_image, current_image, cv::Size(), 1.0f / 4.0f, 1.0f / 4.0f, cv::INTER_LANCZOS4);
		// PERFORM YOUR OPENCV PROCESSING HERE!
		// Visualise the data


		cv::imshow("test_interface", current_image);

		// Check if we need to stop processing
		if ((int)cv::waitKey(10) >= 0) {
			break;
		}
	}

	// Release the camera again
	is_ExitCamera(hCam);

	return 0;
}