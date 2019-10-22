#include <iostream>
#include <stdio.h>
#include <stddef.h>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>

#include <ueye.h>

void initializeCameraInterface(HIDS* hCam_internal);
void getFrame(HIDS* hCam, int width, int height, cv::Mat& mat);
void coutError(int Error);