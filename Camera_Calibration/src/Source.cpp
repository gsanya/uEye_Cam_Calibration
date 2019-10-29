#pragma once

#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include <ueye.h>

#include "LoadImage.hpp"
#include "Properties.hpp"

void ImshowResize(cv::Mat image, float resizefact, std::string windowname);
void Mean(cv::Mat Means, std::vector<cv::Mat> rvecs);
void StandardDeviation(cv::Mat SDs, std::vector<cv::Mat> vecs, cv::Mat Means);


int main()
{
	// ---------------------------------------------------------------------------------------------------------------
	// START BY CONFIGURING THE INTERFACE WITH THE UEYE CAMERA
	// ---------------------------------------------------------------------------------------------------------------


	// Camera initialisation
	// Index 1 means taking the USB camera
	HIDS hCam = 1;
	initializeCameraInterface(&hCam);




	// ---------------------------------------------------------------------------------------------------------------
	// INIT FOR CALIBRATION
	// ---------------------------------------------------------------------------------------------------------------

	int numSquares = numCornersHor * numCornersVer;
	cv::Size board_sz = cv::Size(numCornersHor, numCornersVer);

	std::vector<std::vector<cv::Point3f>> object_points;
	std::vector<std::vector<cv::Point2f>> image_points;

	std::vector<cv::Point2f> corners;

	cv::Mat current_image(2048, 2048, CV_8UC3);
	cv::Mat gray_image;

	//obj is the global coordinate of the corners
	std::vector<cv::Point3f> obj;
	for (int j = 0; j < numSquares; j++)
		obj.push_back(cv::Point3f(squareSizeInmm*(j / numCornersHor), squareSizeInmm*(j%numCornersHor), 0.0f));

	//windows to show the results
	cv::namedWindow("RGB captured image");
	cv::namedWindow("BnW with corners");
	cv::waitKey(1);

	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;
	std::vector<cv::Mat> intrinsicvec;
	std::vector<cv::Mat> distcoeffvec;

	int key = 0;
	// ---------------------------------------------------------------------------------------------------------------
	// CAPTURE DATA AND PROCESS
	// ---------------------------------------------------------------------------------------------------------------
	for(int ii=0; ii<numRuns;ii++)
	{
		int successes = 0;
		
		while (successes < numBoards)
		{
			//grab a frame
			getFrame(&hCam, 2048, 2048, current_image);

			cv::cvtColor(current_image, gray_image, CV_BGR2GRAY);
			ImshowResize(current_image, resizeFactor, "RGB captured image");

			bool found = cv::findChessboardCorners(gray_image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
			if (found)
			{
				cv::cornerSubPix(gray_image, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
				cv::drawChessboardCorners(gray_image, board_sz, corners, found);
				ImshowResize(gray_image, resizeFactor, "BnW with corners");
				//we wait till a key is pressed If it is enter we will save the points, and increment successes
				//if not we jump to next pic
				if (requireUserInput)
				{
					key = cv::waitKey(0);
				}
				if (key == 13 || !requireUserInput)
				{
					image_points.push_back(corners);
					object_points.push_back(obj);

					std::cout << successes + 1 << "/" << numBoards << "            \r";
					successes++;
				}
			}
			else
			{
				std::cout << "not found     \r";
			}
			//if escapes is pressed we quit
			if (key == 27)
				return -1;
		}

		cv::Mat intrinsic = cv::Mat(3, 3, CV_64FC1);
		cv::Mat distCoeffs = cv::Mat(1,5, CV_64FC1);
		std::vector<cv::Mat> rvecs;
		std::vector<cv::Mat> tvecs;

		intrinsic.ptr<double>(0)[0] = 1;
		intrinsic.ptr<double>(1)[1] = 1;

		cv::calibrateCamera(object_points, image_points, current_image.size(), intrinsic, distCoeffs, rvecs, tvecs);
		intrinsicvec.push_back(intrinsic);
		distcoeffvec.push_back(distCoeffs);
		std::cout << "intrinsics:\n"<<intrinsic<<"\ndistortion coeffs:\n"<<distCoeffs<<std::endl;

		object_points.clear();
		image_points.clear();
	}

	cv::Mat Intrinsic_means = cv::Mat(3, 3, CV_64FC1);
	cv::Mat Intrinsic_SDs = cv::Mat(3, 3, CV_64FC1);
	cv::Mat Distcoeffs_means = cv::Mat(1, 5, CV_64FC1);
	cv::Mat Distcoeffs_SDs = cv::Mat(1, 5, CV_64FC1);

	cv::Mat Rot_means = cv::Mat(3, 1, CV_64FC1,cv::Scalar(0));
	cv::Mat Rot_SDs = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));

	cv::Mat Trans_means = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));
	cv::Mat Trans_SDs = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));

	Mean(Intrinsic_means, intrinsicvec);
	StandardDeviation(Intrinsic_SDs, intrinsicvec, Intrinsic_means);

	Mean(Distcoeffs_means, distcoeffvec);
	StandardDeviation(Distcoeffs_SDs, distcoeffvec, Distcoeffs_means);

	std::cout << "Mean of intrinsics:\n" << Intrinsic_means << std::endl;
	std::cout << "SD of intrinsics:\n" << Intrinsic_SDs << std::endl;
	std::cout << "Mean of distcoeffs:\n" << Distcoeffs_means << std::endl;
	std::cout << "SD of distcoeffs:\n" << Distcoeffs_means << std::endl;

	//calculate the mean and the standard deviation of each position (rotation and translation)
	/*
	Mean(Rot_means, rvecs);
	std::cout << "Mean of rvecs:\n" << Rot_means << std::endl;
	StandardDeviation(Rot_SDs, rvecs, Rot_means);
	std::cout << "Standard Deviation of rvecs:\n" << Rot_SDs << std::endl;
	Mean(Trans_means, tvecs);
	std::cout << "Mean of tvecs:\n" << Trans_means << std::endl;
	StandardDeviation(Trans_SDs, tvecs, Trans_means);
	std::cout << "Standard Deviation of tvecs:\n" << Trans_SDs << std::endl;
	*/
	
	
	
	





	cv::waitKey(-1);

	
	cv::destroyWindow("RGB captured image");
	cv::destroyWindow("BnW with corners");
	// Release the camera again
	is_ExitCamera(hCam);
	return 0;
}

void ImshowResize(cv::Mat image,float resizefact, std::string windowname)
{
	cv::Mat imtoshow;
	cv::resize(image, imtoshow, cv::Size(), resizefact, resizefact, cv::INTER_LANCZOS4);
	cv::imshow(windowname, imtoshow);
	cv::waitKey(1);
}

void Mean(cv::Mat Means, std::vector<cv::Mat> vecs)
{
	for (int i = 0; i < vecs.size(); i++)
	{
		for (int j = 0; j < vecs[0].rows; j++)
		{
			for (int k = 0; k < vecs[0].cols; k++)
			{
				Means.at<double>(j, k) += vecs[i].at<double>(j,k);
			}
		}
	}
	for (int j = 0; j < vecs[0].rows; j++)
	{
		for (int k = 0; k < vecs[0].cols; k++)
		{
			Means.at<double>(j, k) /= vecs.size();
		}
	}
}

void StandardDeviation(cv::Mat SDs, std::vector<cv::Mat> vecs, cv::Mat Means)
{
	for (int i = 0; i < vecs.size(); i++)
	{
		for (int j = 0; j < vecs[0].rows; j++)
		{
			for (int k = 0; k < vecs[0].cols; k++)
			{
				SDs.at<double>(j, k) += pow(Means.at<double>(j,k)-vecs[i].at<double>(j,k),2);
			}
		}
	}
	for (int j = 0; j < vecs[0].rows; j++)
	{
		for (int k = 0; k < vecs[0].cols; k++)
		{
			SDs.at<double>(j, k) = sqrt(SDs.at<double>(j, k)/(vecs.size()-1));
		}
	}
}