#include "VideoCapture.h"
#include <QDebug>
// przerobione z https://asmaloney.com/2013/11/code/converting-between-cvmat-and-qimage-or-qpixmap/

VideoCapture::VideoCapture(QObject *parent) 
	: QThread{ parent }{
	
}

void VideoCapture::run() {

	
	auto status = yolo.initialize(YoloTrackingSettings());
	if (status != StatusCode::OK) {
		qDebug() << "Cannot initialize yolo" << StatusToString(status);
		emit workerError("Cannot initialize yolo ", StatusToString(status));
	}
	forever{
		if (isInterruptionRequested()) {
			break;
		}
		status = yolo.tracking(trackedObjectID);
		if (status != StatusCode::OK) {
			qDebug() << "Tracking failed: " << StatusToString(status);
			emit workerError("Tracking failed: ", StatusToString(status));
		}


		auto loc = yolo.sendLocalization(trackedObjectNumber);
		emit displayLocalisationOfObject(QString::fromStdString(loc));  // loc of actual tracked object

		if (enableAutomaticTransmission) {
			qDebug() << "is transmiting";
			auto msg = yolo.sendMessageUART(trackedObjectNumber);
			emit sendCapturedData(QString::fromStdString(msg));
		}

		auto pixmap = cvMatToQPixmap(yolo.frame);
		emit newPixmapCaptured(pixmap.scaled(640, 540));
	}
	
}

void VideoCapture::setTrackedObjectID(int objectID) {
	trackedObjectID = objectID;
}
void VideoCapture::setTrackedObjectNumber(int objectNumber) {
	trackedObjectNumber = objectNumber;
}


void VideoCapture::setUARTTransmissionAutonomously(bool enabled) {
	enableAutomaticTransmission = enabled;
}

QImage  VideoCapture::cvMatToQImage(const cv::Mat &inMat)
{
	switch (inMat.type())
	{
		// 8-bit, 4 channel
	case CV_8UC4:
	{
		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_ARGB32);

		return image;
	}

	// 8-bit, 3 channel
	case CV_8UC3:
	{
		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_RGB888);

		return image.rgbSwapped();
	}

	// 8-bit, 1 channel
	case CV_8UC1:
	{
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_Grayscale8);
#else
		static QVector<QRgb>  sColorTable;

		// only create our color table the first time
		if (sColorTable.isEmpty())
		{
			sColorTable.resize(256);

			for (int i = 0; i < 256; ++i)
			{
				sColorTable[i] = qRgb(i, i, i);
			}
		}

		QImage image(inMat.data,
			inMat.cols, inMat.rows,
			static_cast<int>(inMat.step),
			QImage::Format_Indexed8);

		image.setColorTable(sColorTable);
#endif

		return image;
	}

	default:
		qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
		break;
	}

	return QImage();
}

QPixmap VideoCapture::cvMatToQPixmap(const cv::Mat &inMat)
{
	return QPixmap::fromImage(cvMatToQImage(inMat));
}