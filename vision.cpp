#include "vision.hpp"
namespace dahun {
	void Image_Pretreatment(Mat& frame) {
		cvtColor(frame, frame, COLOR_BGR2GRAY);
		frame = frame + (100 - mean(frame)[0]);
		threshold(frame, frame,145, 255, THRESH_BINARY);
		frame = frame(Rect(0, frame.rows * 3 / 4, frame.cols, frame.rows / 4));
	}
	Point2d find_object(Mat& labels, Mat& stats, Mat& centroids, Mat& frame, Point2d crnt_pt) {
		int cnt = connectedComponentsWithStats(frame, labels, stats, centroids);
		cvtColor(frame, frame, COLOR_GRAY2BGR);

		vector <double> distance;
		double mindistance;
		int min_index;
		double pt_distance;
		static Point2d prev_pt(frame.cols/ 2, frame.rows / 2);
		

		if (cnt > 1) {
			for (int i = 1; i < cnt; i++) {
				double* cpt = centroids.ptr<double>(i);
				pt_distance = abs(cpt[0] - prev_pt.x);
				distance.push_back(pt_distance);
			}
			mindistance = *min_element(distance.begin(), distance.end());
			min_index = min_element(distance.begin(), distance.end()) - distance.begin();
			crnt_pt = Point2d(centroids.at<double>(min_index + 1, 0), centroids.at<double>(min_index + 1, 1));
			if (abs(crnt_pt.x-prev_pt.x) > 100) crnt_pt = prev_pt;
			distance.clear();
		}
		else {
			crnt_pt = prev_pt;
		}
		prev_pt = crnt_pt;

		circle(frame, crnt_pt, 2, Scalar(0, 0, 255), 2);
		return crnt_pt;
	}
	int get_error(Point2d cpt, Mat frame) {
		int error = (frame.cols / 2 -cpt.x) * GAIN;
		return error;
	}
	void set_speed(int& Lspeed, int& Rspeed,int error) {
		Lspeed = INIT_SPEED - error;
		Rspeed = -(INIT_SPEED + error);
	}
}