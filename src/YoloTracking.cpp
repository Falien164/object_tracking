#include <iostream>
#include <chrono>
#include <QDebug>
#include "YoloTracking.h"

using namespace cv;
using namespace std;




StatusCode YoloTracking::initialize(const YoloTrackingSettings& args) {
	if (isInitialized)
		return StatusCode::OK;

//	kamera.open(args.cameraName); //"vtest.avi"
	//kamera.open("trim.mp4"); //"vtest.avi"
	kamera.open(0); //"vtest.avi"

	if (!kamera.isOpened()) {
		qDebug() << "Nie pod³¹czona kamera";
		return StatusCode::CameraMissing;   //zabezpieczenie - jesli nie ma nowej klatki, przerywa dzialanie
	}
	detector = std::make_shared<Detector>(args.cfg_filename, args.weight_filename);
	kamera.set(CAP_PROP_FRAME_WIDTH, 1280);
	kamera.set(CAP_PROP_FRAME_HEIGHT, 720);

	isInitialized = true;
	return StatusCode::OK;
}

//VideoWriter output("1.avi", CV_FOURCC('M', 'J', 'P', 'G'), 25, Size(frame_width, frame_height));
StatusCode YoloTracking::tracking(const int trackedObjectsID) {

	// pomiar okresu petli
	double precTick = ticks;
	ticks = (double)cv::getTickCount();
	dT = (ticks - precTick) / cv::getTickFrequency(); //seconds
	qDebug() << "czestotliwoœæ pobierania ramek =" << 1 / dT << "Hz" << endl;
	
	// pobranie obrazu z kamery 
	kamera >> frame;
//	output.write(frame);

	if (frame.data == NULL) {
		qDebug() << "Brak nowej klatki";
		return StatusCode::MissingFrame;   //zabezpieczenie - jesli nie ma nowej klatki, przerywa dzialanie
	}
	

	// Get bounding boxes from neural network
	vector<bbox_t> result_vec = detector->detect(frame, 0.5);
	for (auto &i : result_vec) {
		if (i.obj_id == trackedObjectsID) { // 39 to butelka
			BoundingBox bb = BoundingBox(i.x, i.y, i.w, i.h); 
			detectedPeople.emplace_back(i.obj_id, i.prob, bb);
		}
	}

	// rysuje linie
	//line(frame, Point(640, 0), Point(640, 720), CV_RGB(255, 255, 0), 5);
	//line(frame, Point(0, 360), Point(1280, 360), CV_RGB(255, 255, 0), 5);

	trackedPeople = tracker.track(detectedPeople);

	// stworz ramke dooko³a wykrytych obiektow
	for (int i = 0; i < trackedPeople.size();i++) {
		putText(frame, to_string(trackedPeople[i].ID), Point(trackedPeople[i].bb.cx, trackedPeople[i].bb.cy), 0, 1, CV_RGB(255, 0, 0), 2);
		rectangle(frame, Rect(detectedPeople[i].bb.cx, detectedPeople[i].bb.cy, detectedPeople[i].bb.width, detectedPeople[i].bb.height), Scalar(50, 200, 50), 3);
	}

	detectedPeople.clear();
	waitKey(1);
	return StatusCode::OK;   //zabezpieczenie - jesli nie ma nowej klatki, przerywa dzialanie

}



string YoloTracking::sendLocalization(int i) {
	if (trackedPeople.size() > i) {
		return  " x: " + to_string (trackedPeople[i].bb.cx + trackedPeople[i].bb.width/2)+ " y: " + to_string(trackedPeople[i].bb.cy + trackedPeople[i].bb.height / 2);
	}
	return "Not Found";
}

string YoloTracking::sendMessageUART(int k) {
	int i = k;
	qDebug() << "i =  " << i;

	for (int j = 0; j < trackedPeople.size(); j++) {
		if (trackedPeople[j].ID == i) {
			i = j;
		}
	}
	qDebug() << "i =  " << i;
	
	if (trackedPeople.size() > i) {

		qDebug() << "tracked ID = " << trackedPeople[i].ID;

		int x = (trackedPeople[i].bb.cx + trackedPeople[i].bb.width/2);
		int y = (trackedPeople[i].bb.cy + trackedPeople[i].bb.height/3);
		rectangle(frame, Rect(trackedPeople[i].bb.cx, trackedPeople[i].bb.cy, trackedPeople[i].bb.width, trackedPeople[i].bb.height), Scalar(200, 50, 200), 3);

		//if(trackedPeople.ID)
		int y_send = -(y - 360) / 10 + 500; // 0-499 - liczba ujemna, 500-999 dodatnia
		int x_send = -(x - 640) / 16 + 500; // 0-499 - liczba ujemna, 500-999 dodatnia
		qDebug() << "y_send: " << y_send << " x_send: " << x_send;
		string y_send_str = to_string(static_cast<int>(y_send));
		string x_send_str = to_string(static_cast<int>(x_send));
		//circle(frame, Point(x,y), 10, CV_RGB(0, 0, 255),5);
		return  "###" + y_send_str + x_send_str;
	}
	return "Not Found";
}