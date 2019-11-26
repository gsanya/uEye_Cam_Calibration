#pragma once

#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>


void ImshowResize(cv::Mat image, float resizefact, std::string windowname);
std::vector<cv::Point3f> loadCorners(std::string input);

int main()
{
	int numBoards = 11;
	//inner corners
	int numCornersHor =6;
	int numCornersVer = 4;
	double squareSizeInmm =30;

	int numSquares = numCornersHor * numCornersVer;
	cv::Size board_sz = cv::Size(numCornersHor, numCornersVer);

	std::vector<std::vector<cv::Point3f>> object_points;
	std::vector<std::vector<cv::Point2f>> image_points;

	std::vector<cv::Point2f> corners;
	int successes = 0;

	cv::Mat image;
	cv::Mat gray_image;

	std::vector<cv::Point3f> obj= loadCorners("D:/Users/Sanya/Drive/BME/BME_Mechatronika_MSc/2_semester/Projektfeladat/ueye_project/boardcoordinates.txt");
	for (int j = 0; j < numSquares; j++)
		obj.push_back(cv::Point3f(squareSizeInmm*(j / numCornersHor), squareSizeInmm*(j%numCornersHor), 0.0f));

	//create a String vector with the filenames
	std::vector<std::string> fileNames;
	//I have 30 pictures
	fileNames.reserve(30);
	std::stringstream ss;
	for (int i = 0; i < 11; i++)
	{
		if(i<9)
			ss << "D:/Users/Sanya/Pictures/uEye/12-11-2019_11h14m24s/0" << i + 1 << ".png";
		else
			ss << "D:/Users/Sanya/Pictures/uEye/12-11-2019_11h14m24s/" << i + 1 << ".png";
		fileNames.push_back(ss.str());
		ss.str("");
		std::cout << fileNames[i] << std::endl;
	}
	cv::namedWindow("image");
	cv::namedWindow("gray_image");
	image = cv::imread(fileNames[0], CV_LOAD_IMAGE_COLOR);
	int i = 0;
	while (i < numBoards)
	{
		cv::cvtColor(image, gray_image, CV_BGR2GRAY);
		ImshowResize(image, 0.5, "image");
		bool found = cv::findChessboardCorners(gray_image, board_sz, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if (found)
		{
			cv::cornerSubPix(gray_image, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			cv::drawChessboardCorners(gray_image, board_sz, corners, found);
		}
		ImshowResize(gray_image, 0.5, "gray_image");
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



	cv::waitKey(0);

	cv::destroyWindow("image");
	cv::destroyWindow("gray_image");
}

void ImshowResize(cv::Mat image, float resizefact, std::string windowname)
{
	cv::Mat imtoshow;
	cv::resize(image, imtoshow, cv::Size(), resizefact, resizefact, cv::INTER_LANCZOS4);
	cv::imshow(windowname, imtoshow);
	cv::waitKey(1);
}

std::vector<cv::Point3f> loadCorners(std::string input)
{
	//TODO: write this function
	std::vector<cv::Point3f> pvector;
	std::ifstream inputFile;
	inputFile.open(input);
	std::string line;
	std::string delimiter = ",";
	if (inputFile.is_open())
	{
		while (std::getline(inputFile, line))
		{
			//end of x substring
			line = line.erase(line.find('['), 1);
			line = line.erase(line.find(']'), 1);
			int pos1 = line.find(delimiter);
			std::string x = line.substr(0, pos1);
			//end of y substring
			int pos2 = line.find(delimiter, pos1 + 1);
			std::string y = line.substr(pos1+2, pos2-pos1-2);
			//end of z substring
			std::string z = line.substr(pos2+2, line.length()-pos2-2);

			float fx = std::stof(x);
			float fy = std::stof(y);
			float fz = std::stof(z);
			cv::Point3f point = cv::Point3f(fx,fy,fz);
			pvector.push_back(point);
			std::cout << line << '\n';
		}
		inputFile.close();
	}
	return pvector;
}