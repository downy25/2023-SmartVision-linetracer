#ifndef _VISION_HPP_
#define _VISION_HPP_

#include <iostream>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include "dxl.hpp"
#include "opencv2/opencv.hpp"
#define GAIN 0.11
#define INIT_SPEED 100
using namespace std;
using namespace cv;
namespace dahun {
	void Image_Pretreatment(Mat& frame);
	Point2d find_object(Mat& labels, Mat& stats, Mat& centroids, Mat& frame, Point2d crnt_pt);
	int get_error(Point2d cpt, Mat frame);
	void set_speed(int& Lspeed, int& Rspeed, int error);
}
#endif