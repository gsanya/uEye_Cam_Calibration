#pragma once

#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>


int main()
{
	int numBoards = 10;
	//inner corners
	int numCornersHor =25;
	int numCornersVer = 9;
	double squareSizeInmm =30;

	int numSquares = numCornersHor * numCornersVer;
	cv::Size board_sz = cv::Size(numCornersHor, numCornersVer);

	std::vector<std::vector<cv::Point3f>> object_points;
	std::vector<std::vector<cv::Point2f>> image_points;

	std::vector<cv::Point2f> corners;
	int successes = 0;

	cv::Mat image;
	cv::Mat gray_image;

	std::vector<cv::Point3f> obj;
	for (int j = 0; j < numSquares; j++)
		obj.push_back(cv::Point3f(squareSizeInmm*(j / numCornersHor), squareSizeInmm*(j%numCornersHor), 0.0f));

	//create a String vector with the filenames
	std::vector<std::string> fileNames;
	//I have 30 pictures
	fileNames.reserve(30);
	std::stringstream ss;
	for (int i = 0; i < 30; i++)
	{
		if(i<9)
			ss << "D:/Users/Sanya/Pictures/uEye/0" << i + 1 << ".png";
		else
			ss << "D:/Users/Sanya/Pictures/uEye/" << i + 1 << ".png";
		fileNames.push_back(ss.str());
		ss.str("");
		std::cout << fileNames[i] << std::endl;
	}
	cv::namedWindow("win1");
	cv::namedWindow("win2");
	image = cv::imread(fileNames[0], CV_LOAD_IMAGE_COLOR);
	int i = 1;
	while (successes < numBoards)
	{
		cv::cvtColor(image, gray_image, CV_BGR2GRAY);
		cv::imshow("win1", image);
		cv::waitKey(1);
		cv::imshow("win2", gray_image);
		cv::waitKey(1);
		bool found = cv::findChessboardCorners(gray_image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if (found)
		{
			cv::cornerSubPix(gray_image, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			cv::drawChessboardCorners(gray_image, board_sz, corners, found);
		}
		cv::imshow("win1", image);
		cv::imshow("win2", gray_image);
		int key = cv::waitKey(1);
		image = cv::imread(fileNames[i], CV_LOAD_IMAGE_COLOR);
		if (key == 27)
			return 0;
		if (found)
		{
			image_points.push_back(corners);
			object_points.push_back(obj);

			std::cout<<"Snap stored!"<<std::endl;
			successes++;
		}
		i++;
	}


	cv::Mat intrinsic = cv::Mat(3, 3, CV_32FC1);
	cv::Mat distCoeffs;
	std::vector<cv::Mat> rvecs;
	std::vector<cv::Mat> tvecs;

	intrinsic.ptr<float>(0)[0] = 1;
	intrinsic.ptr<float>(1)[1] = 1;

	cv::calibrateCamera(object_points, image_points, image.size(), intrinsic, distCoeffs, rvecs, tvecs);
	std::cout << "intrinsics:\n" << intrinsic << std::endl;
	std::cout << "distCoeffs:\n" << distCoeffs << std::endl;
	std::cout << "rvecs:\n" << rvecs[0] << std::endl;
	std::cout << "tvecs:\n[" << tvecs[0] << std::endl;



	cv::waitKey(-1);

	cv::destroyWindow("win1");
	cv::destroyWindow("win2");
}