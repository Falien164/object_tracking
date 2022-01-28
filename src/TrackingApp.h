#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TrackingApp.h"
#include "VideoCapture.h"

#include <Qserialport>
#include <QSerialPortInfo>
class VideoCapture;

class TrackingApp : public QMainWindow
{
    Q_OBJECT

public:

    TrackingApp(QWidget *parent = Q_NULLPTR);
	TrackingApp::~TrackingApp();

public slots:
	void on_btnInitYolo_clicked();
	void on_btnCaptureVideo_clicked();
	void on_txtX_returnPressed();
	void on_txtY_returnPressed();
	void on_btnOpenSerialPort_clicked();
	void on_btnCloseSerialPort_clicked();
	void on_btnShoot_pressed();
	void on_btnShoot_released();
	void on_checkBoxSendUART_toggled(bool enabled);
	void on_cocoIDcomboBox_currentIndexChanged(int index);
	void on_trackedObjectNumber_ledit_returnPressed();
signals:

private:
	void TrackingApp::OpenSerialPort(const QString& portName);
	void TrackingApp::setSerialPortComboBoxData();

	CocoFiles cocoFiles;
	QSerialPort serial;
    Ui::TrackingAppClass ui;
	VideoCapture *mOpenCV_videoCapture;

};



