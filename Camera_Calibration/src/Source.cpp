#pragma once
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include <ueye.h>

#include "LoadImage.hpp"
#include "Properties.hpp"

void ImshowResize(cv::Mat image, float resizefact, std::string windowname);
void Mean(cv::Mat Means, std::vector<cv::Mat> rvecs);
void StandardDeviation(cv::Mat SDs, std::vector<cv::Mat> vecs, cv::Mat Means);
std::vector<std::vector<cv::Point3f>> loadCorners(std::string input);


int main()
{
	// ---------------------------------------------------------------------------------------------------------------
	// START BY CONFIGURING THE INTERFACE WITH THE UEYE CAMERA
	// ---------------------------------------------------------------------------------------------------------------
	std::stringstream ss,folder;
	time_t t = time(NULL);
	struct tm  curtime = *localtime(&t);
	ss << "md D:\\Users\\Sanya\\Pictures\\uEye\\" << std::put_time(&curtime, "%d-%m-%Y_%Hh%Mm%Ss");
	folder<<"D:\\Users\\Sanya\\Pictures\\uEye\\" << std::put_time(&curtime, "%d-%m-%Y_%Hh%Mm%Ss");
	system((ss.str()).c_str());

	std::cout << "OpenCV version : " << CV_VERSION << std::endl;
	std::cout << "Major version : " << CV_MAJOR_VERSION << std::endl;
	std::cout << "Minor version : " << CV_MINOR_VERSION << std::endl;
	std::cout << "Subminor version : " << CV_SUBMINOR_VERSION << std::endl;

	// Camera initialisation
	// Index 1 means taking the USB camera
	HIDS hCam = 1;
	initializeCameraInterface(&hCam);

	// ---------------------------------------------------------------------------------------------------------------
	// INIT FOR CALIBRATION
	// ---------------------------------------------------------------------------------------------------------------

	cv::Size board_size = cv::Size(numCornersHor, numCornersVer);

	std::vector<std::vector<cv::Point3f>> object_points;
	std::vector<std::vector<cv::Point2f>> image_points;

	std::vector<cv::Point2f> corners;

	cv::Mat current_image(2048, 2048, CV_8UC3);
	cv::Mat gray_image;

	//obj is the global coordinate of the corners

	std::vector<std::vector<cv::Point3f>> obj;
	std::vector<cv::Point3f > test;
	obj = loadCorners(pointsFileLocation);

	//consistency checks
	if (obj.size() != numBoards)
	{
		std::cout << "The number of boards in the file at pointsFileLocation is not consistent with the numBoards set up in Properties.hpp.";
		char end;
		std::cin >> end;
		return -1;
	}

	for (int i = 0; i < obj.size(); i++)
	{
		if (obj[i].size() != numCornersHor*numCornersVer)
		{
			std::cout << "The number of points in the file at pointsFileLocation is not consistent with the horizontal and vertical number of corners set up in Properties.hpp.";
			char end;
			std::cin >> end;
			return -1;
		}
	}
	

	//windows to show the results
	cv::namedWindow("RGB captured image");
	cv::namedWindow("BnW with corners");
	cv::waitKey(1);

	std::vector<cv::Mat> rvecsall;
	std::vector<cv::Mat> tvecsall;
	std::vector<cv::Mat> intrinsicvec;
	std::vector<cv::Mat> distcoeffvec;
	//it's only mat, so I can use my functions for mat
	std::vector<cv::Mat> errorsqrvec;

	int key = 0;
	
	// ---------------------------------------------------------------------------------------------------------------
	// CAPTURE DATA AND PROCESS
	// ---------------------------------------------------------------------------------------------------------------
	for(int ii=0; ii<numRuns;ii++)
	{
		int successes = 0;
		
		while (successes < numBoards)
		{
			bool enterPressed = false;
			//grab a frame when enter is pressed	(press enter for a little longer)	
			std::cout << "press enter to capture image\n";
			do{
				getFrame(&hCam, 2048, 2048, current_image);
				ImshowResize(current_image, resizeFactor, "RGB captured image");
				if(cv::waitKey(1000)==13)
					enterPressed = true;
			} while (!enterPressed || !requireUserInput);
			std::cout << "image captured. processing...\n";
			cv::cvtColor(current_image, gray_image, CV_BGR2GRAY);
			bool found = cv::findChessboardCorners(gray_image, board_size, corners, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
			if (found)
			{
				std::cout << "pattern found press enter to save.\n";
				cv::cornerSubPix(gray_image, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
				cv::drawChessboardCorners(gray_image, board_size, corners, found);
				ImshowResize(gray_image, resizeFactor, "BnW with corners");
				//we wait till a key is pressed If it is enter we will save the points, and increment successes
				//if not we jump to next pic
				if (requireUserInput)
				{
					key = cv::waitKey(0);
				}
				//add found 
				if (key == 13 || !requireUserInput)
				{
					image_points.push_back(corners);
					object_points.push_back(obj[successes]);

					std::cout <<"pattern saved: " <<successes + 1 << "/" << numBoards << "\n";
					successes++;
					std::stringstream ss;
					ss.str("");
					if (successes < 10)
						ss << folder.str() << "/0" << successes << ".png";
					else
						ss << folder.str() <<"/" << successes << ".png";
					cv::imwrite(ss.str(), current_image);	
				}
			}
			else
			{
				std::cout << "pattern not found     \r";
			}
			//if escapes is pressed we quit
			if (key == 27)
				return -1;
		}

//		

//		[10043.26386153522, 0, 1338.099919383922;
//		0, 9897.809152447015, 960.1074820443306;
//		0, 0, 1]


		cv::Mat intrinsic;

		if (onlyIntrinsics)
		{
			intrinsic = cv::Mat(3, 3, CV_64FC1);
			intrinsic.ptr<double>(0)[0] = 1;
			intrinsic.ptr<double>(1)[1] = 1;
		}
		else
		{
			intrinsic = (cv::Mat_<double>(3, 3) <<	9709.819864652645, 0, 1080.865185087339,
													0, 9720.449830896274, 944.9779849500916,
													0, 0, 1);
		}

		cv::Mat distCoeffs = cv::Mat(1,5, CV_64FC1);
		std::vector<cv::Mat> rvecs;
		std::vector<cv::Mat> tvecs;
		cv::Mat errorsqr = cv::Mat(1, 1, CV_64FC1);

		if (onlyIntrinsics)
		{
			try
			{
				errorsqr.at<double>(0, 0) = cv::calibrateCamera(object_points, image_points, current_image.size(), intrinsic, distCoeffs, rvecs, tvecs);
			}
			catch (cv::Exception & e)
			{
				std::cerr << e.msg << std::endl; // output exception message
			}
		}
		else
		{
			try
			{
				errorsqr.at<double>(0, 0) = cv::calibrateCamera(object_points, image_points, current_image.size(), intrinsic, distCoeffs, rvecs, tvecs, CV_CALIB_USE_INTRINSIC_GUESS);
			}
			catch (cv::Exception & e)
			{
				std::cerr << e.msg << std::endl; // output exception message
			}
		}
		

		
		
		errorsqrvec.push_back(errorsqr);
		for (int p = 0; p < numBoards; p++)
		{
			rvecsall.push_back(rvecs[p]);
			tvecsall.push_back(tvecs[p]);
		}

		intrinsicvec.push_back(intrinsic);
		distcoeffvec.push_back(distCoeffs);

		std::cout << "\n------------------------------------------------------Results------------------------------------------------------\n\n";

		std::cout <<"Errorsquare:\t"<<errorsqr.at<double>(0,0) <<"\nintrinsics:\n"<<intrinsic<<"\ndistortion coeffs:\n"<<distCoeffs<<std::endl;

		object_points.clear();
		image_points.clear();
	}


	cv::Mat Intrinsic_means = cv::Mat(3, 3, CV_64FC1,  cv::Scalar(0));
	cv::Mat Intrinsic_SDs = cv::Mat(3, 3, CV_64FC1, cv::Scalar(0));
	cv::Mat Distcoeffs_means = cv::Mat(1, 5, CV_64FC1, cv::Scalar(0));
	cv::Mat Distcoeffs_SDs = cv::Mat(1, 5, CV_64FC1, cv::Scalar(0));
	cv::Mat errorsqr_mean = cv::Mat(1, 1, CV_64FC1, cv::Scalar(0));
	cv::Mat errorsqr_SD = cv::Mat(1, 1, CV_64FC1, cv::Scalar(0));

	cv::Mat Rot_means = cv::Mat(3, 1, CV_64FC1,cv::Scalar(0));
	cv::Mat Rot_SDs = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));
	cv::Mat Trans_means = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));
	cv::Mat Trans_SDs = cv::Mat(3, 1, CV_64FC1, cv::Scalar(0));

	Mean(Intrinsic_means, intrinsicvec);
	StandardDeviation(Intrinsic_SDs, intrinsicvec, Intrinsic_means);
	Mean(Distcoeffs_means, distcoeffvec);
	StandardDeviation(Distcoeffs_SDs, distcoeffvec, Distcoeffs_means);
	Mean(errorsqr_mean, errorsqrvec);
	StandardDeviation(errorsqr_SD, errorsqrvec, errorsqr_mean);
	Mean(Rot_means, rvecsall);
	StandardDeviation(Rot_SDs, rvecsall, Rot_means);
	Mean(Trans_means, tvecsall);
	StandardDeviation(Trans_SDs, tvecsall, Trans_means);
	

	std::cout << "\n------------------------------------------------------Result means and SDs------------------------------------------------------\n\n";
	
	//It has only meaning if we calibrate more times 
	if (numRuns > 1)
	{
		std::cout << "Mean of error squares:\t" << errorsqr_mean.at<double>(0, 0) << std::endl;
		std::cout << "SD of error squares:\t" << errorsqr_SD.at<double>(0, 0) << std::endl;
		std::cout << "Mean of intrinsics:\n" << Intrinsic_means << std::endl;
		std::cout << "SD of intrinsics:\n" << Intrinsic_SDs << std::endl;
		std::cout << "Mean of distcoeffs:\n" << Distcoeffs_means << std::endl;
		std::cout << "SD of distcoeffs:\n" << Distcoeffs_SDs << std::endl;
	}
	std::cout << "Mean of rvecsall:\n" << Rot_means << std::endl;
	std::cout << "Standard Deviation of rvecsall:\n" << Rot_SDs << std::endl;
	std::cout << "Mean of tvecsall:\n" << Trans_means << std::endl;
	std::cout << "Standard Deviation of tvecsall:\n" << Trans_SDs << std::endl;
	
	std::cout << "\n------------------------------------------------------Printing all of tvecs:------------------------------------------------------\n";
	for (int i = 0; i < tvecsall.size(); i++)
	{
		std::cout << tvecsall[i] << std::endl;
	}
	







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

std::vector<std::vector<cv::Point3f>> loadCorners(std::string input)
{
	//TODO: write this function
	std::vector<std::vector<cv::Point3f>> pvector;
	pvector.resize(numBoards);
	std::ifstream inputFile;
	inputFile.open(input);
	std::string line;
	std::string delimiter = ",";
	if (inputFile.is_open())
	{
		std::getline(inputFile, line);
		if(line=="board"){
			//std::getline(inputFile, line);
			for (int i=0; i<numBoards; i++)
			{
				while (std::getline(inputFile, line)&& line != "board")
				{
					//end of x substring
					line = line.erase(line.find('['), 1);
					line = line.erase(line.find(']'), 1);
					int pos1 = line.find(delimiter);
					std::string x = line.substr(0, pos1);
					//end of y substring
					int pos2 = line.find(delimiter, pos1 + 1);
					std::string y = line.substr(pos1 + 2, pos2 - pos1 - 2);
					//end of z substring
					std::string z = line.substr(pos2 + 2, line.length() - pos2 - 2);

					float fx = std::stof(x);
					float fy = std::stof(y);
					float fz = std::stof(z);
					cv::Point3f point = cv::Point3f(fx, fy, fz);
					pvector[i].push_back(point);
				}
			}		
		}	
		else {
			std::cout << "wrong file format\n";
		}
		std::cout << "file read\n";
		inputFile.close();
	}
	else
	{
		std::cout << "wrong filepath or file doesn't exists\n";
	}
	return pvector;
}