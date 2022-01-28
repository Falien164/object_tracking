#pragma once
#include <vector>
#include <memory>
#include <string>
#include "CocoFiles.h"
//#define OPENCV and YOLO
#include "yolo_v2_class.hpp"	// imported functions from DLL
#include "opencv2\opencv.hpp"
#include "Tracker/tracker/PAOT.h"
#include "Tracker/tracker/Tracker.h"
#include "common.h"

class YoloTrackingSettings {
public:
	std::string cfg_filename = "yolov3.cfg";
	std::string weight_filename = "yolov3.weights";
	std::string cameraName = "vtest.avi";

	YoloTrackingSettings& setCameraName(std::string name) {
		cameraName = name;
		return *this;
	}
	YoloTrackingSettings& setCfg_fileName(std::string name) {
		cfg_filename = name;
		return *this;
	}
	YoloTrackingSettings& setWeight_fileName(std::string name) {
		weight_filename = name;
		return *this;
	}

};




class YoloTracking {
public:

	//Methods
	StatusCode initialize(const YoloTrackingSettings& args);
	StatusCode tracking(int trackedObjectsID = 0);
	std::string sendLocalization(int i = 0);
	std::string sendMessageUART(int k = 0);

	cv::VideoCapture kamera;
	std::shared_ptr<Detector> detector;
	cv::Mat frame;
	double dT = 0.00;
	double ticks = 0;
	PAOT tracker;
	std::vector <Tracking> trackedPeople;
	std::vector <Detection> detectedPeople;
	bool isInitialized = false;

	struct detectedPeople {
		unsigned int x;
		unsigned int y;
		unsigned int w;
		unsigned int h;
		detectedPeople(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
			: x(x), y(y), w(w), h(h) {}
	};



};