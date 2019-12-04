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

	int i = 0;
	while (i<11) {
		// Create an image and grab a frame
		cv::Mat current_image(2048, 2048, CV_8UC3);
		getFrame(&hCam, 2048, 2048, current_image);
		cv::Mat imtoshow;
		cv::resize(current_image, imtoshow, cv::Size(), 1.0f / 2.0f, 1.0f / 2.0f, cv::INTER_LANCZOS4);
		// PERFORM YOUR OPENCV PROCESSING HERE!
		// Visualise the data


		cv::imshow("test_interface", imtoshow);

		// Check if we need to stop processing
		if ((int)cv::waitKey(10) >= 0) {
			break;
		}
		

		std::stringstream ss;
		if (i < 9)
			ss << "D:/Users/Sanya/Pictures/uEye/0" << i + 1 << ".png";
		else
			ss << "D:/Users/Sanya/Pictures/uEye/" << i + 1 << ".png";
		
		int key=cv::waitKey(0);
		if (key == 13) {
			cv::imwrite(ss.str(), current_image);
			i++;
		}
		

		//i++;
	}


	while (true)
	{
		// Create an image and grab a frame
		cv::Mat current_image(2048, 2048, CV_8UC3);
		getFrame(&hCam, 2048, 2048, current_image);
		cv::Mat imtoshow;
		cv::resize(current_image, imtoshow, cv::Size(), 1.0f / 2.0f, 1.0f / 2.0f, cv::INTER_LANCZOS4);
		// PERFORM YOUR OPENCV PROCESSING HERE!
		// Visualise the data


		cv::imshow("test_interface", imtoshow);

		// Check if we need to stop processing
		if ((int)cv::waitKey(10) >= 0) {
			break;
		}
	}
	// Release the camera again
	is_ExitCamera(hCam);

	return 0;
}