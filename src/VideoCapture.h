#pragma once
#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H

#include <QPixmap>
#include <QImage>
#include <QThread>

#include <Qstring>
#include <QReadWriteLock>
#include "yoloTracking.h"
class VideoCapture : public QThread {
	Q_OBJECT
public:
	VideoCapture(QObject *parent = nullptr);

	// dane proste synchroniczne dla w¹tku
	QAtomicInteger<bool> enableAutomaticTransmission;
	QAtomicInteger<int> trackedObjectID;
	QAtomicInteger<int> trackedObjectNumber;

	// metody do pól zwiazanych z w¹tkami
	void setUARTTransmissionAutonomously(bool enabled);
	void VideoCapture::setTrackedObjectID(int objectID);
	void VideoCapture::setTrackedObjectNumber(int objectNumber);

signals:
	void newPixmapCaptured(QPixmap pixmap); //przechwycony obraz
	void displayLocalisationOfObject(QString loc); //lokalizacja objektu z yolo
	void sendCapturedData(QString msg);
	void workerError(QString message, const char* StatusToString);

protected:
	void run() override;

private:
	QImage  cvMatToQImage(const cv::Mat &inMat);
	QPixmap cvMatToQPixmap(const cv::Mat &inMat);
	YoloTracking yolo;
};

#endif //VIDEOCAPTURE_H
