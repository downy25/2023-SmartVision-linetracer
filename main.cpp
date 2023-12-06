#include "vision.hpp"
using namespace dahun;
bool ctrl_c_pressed=false;
bool s_pressed=false;
void ctrlc(int){ctrl_c_pressed=true;}
int main(void) {
	Dxl mx;
	struct timeval start,end1;
	double time1;

	signal(SIGINT,ctrlc);

	if(!mx.open()){cout<<"dynamixel open error"<<endl; return -1;}


	//VideoCapture cap("6_lt_ccw_100rpm_out.mp4");
	string src = "nvarguscamerasrc sensor-id=0 ! video/x-raw(memory:NVMM), width=(int)640, height=(int)360, format=(string)NV12 ! \
    nvvidconv flip-method=0 ! video/x-raw, width=(int)640, height=(int)360, format=(string)BGRx ! videoconvert ! \
    video/x-raw, format=(string)BGR !appsink";

	VideoCapture cap(src,CAP_GSTREAMER);

	string dst1 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! \
			rtph264pay pt=96 ! udpsink host=192.168.0.11 port=8001 sync=false";
	VideoWriter writer(dst1, 0, (double)30, Size(640, 360), true);
	if(!writer.isOpened())  { cout << "Writer error" << endl; return -1;}

	string dst2 = "appsrc ! videoconvert ! video/x-raw, format=BGRx ! nvvidconv ! nvv4l2h264enc insert-sps-pps=true ! h264parse ! \
			rtph264pay pt=96 ! udpsink host=192.168.0.11 port=8002 sync=false";
	VideoWriter writer1(dst2, 0, (double)30, Size(640, 90), true);
	if(!writer.isOpened())  { cout << "Writer error" << endl; return -1;}

	if (!cap.isOpened()) {
		cerr << "Video open failed!" << endl;
		return 0;
	}

	Mat frame, dst;
	Mat labels, stats, centroids;
	Point2d crnt_pt;
	int Rspeed = 0, Lspeed = 0;
	

	while (true) {
		gettimeofday(&start,NULL);
		if(mx.kbhit()){
			char c =mx.getch();
			if(c=='s')s_pressed=true;
		}
		cap >> frame;
		writer<<frame;
		if (frame.empty())break;
		dst = frame.clone();
		Image_Pretreatment(frame);
		Point2d cpt=find_object(labels, stats, centroids, frame, crnt_pt);
		int error = get_error(cpt, dst);
		set_speed(Lspeed, Rspeed, error);
		writer1<<frame;
		writer<<dst;
		if (ctrl_c_pressed) break;
		if(s_pressed) mx.setVelocity(Lspeed,Rspeed);
		usleep(10*1000);
		gettimeofday(&end1,NULL);
		time1 = end1.tv_sec-start.tv_sec +(end1.tv_usec-start.tv_usec)/1000000.0;
		cout <<"error: "<<error<<"     lvel: "<<Lspeed<<"    rvel:"<<Rspeed << "   time:" << time1 << endl;
		
	}
	destroyAllWindows();
	mx.close();
	return 0;
}