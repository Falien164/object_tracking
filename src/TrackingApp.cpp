#include "TrackingApp.h"
#include <QDebug>
#include "VideoCapture.h"
#include <QIntValidator>

TrackingApp::TrackingApp(QWidget *parent)
    : QMainWindow(parent)
{
	ui.setupUi(this);
	ui.btnShoot->setVisible(false);
	ui.trackedObjectNumber_ledit->setValidator(new QIntValidator(0, INT_MAX,this));  // sprawdza czy liczba jest z zakresu (0,inf)

	mOpenCV_videoCapture = new VideoCapture(this);
	connect(mOpenCV_videoCapture, &VideoCapture::newPixmapCaptured, this, [&](QPixmap pixmap ) {
		ui.Frame_Qt->setPixmap(pixmap);
		}, Qt::QueuedConnection);

	connect(mOpenCV_videoCapture, &VideoCapture::displayLocalisationOfObject, this, [&](QString loc) {
		ui.txtConsoleVideo->appendPlainText(loc);
		}, Qt::QueuedConnection);

	connect(mOpenCV_videoCapture, &VideoCapture::sendCapturedData, this, [&](QString msg) {
		msg += ui.btnShoot->isDown() ? ':' : ':';
		ui.txtConsoleVideo->appendPlainText(msg);
		auto rawData = msg.toUtf8();
		serial.write(rawData);
		}, Qt::QueuedConnection);

	connect(mOpenCV_videoCapture, &VideoCapture::workerError, this, [&](QString message, const char* StatusToString) {
		ui.txtConsole->appendPlainText(message + QString::fromLatin1(StatusToString));
		if (mOpenCV_videoCapture->isRunning()) {
			mOpenCV_videoCapture->requestInterruption();
			mOpenCV_videoCapture->wait();
		}
		}, Qt::QueuedConnection);


	// Wczytanie bazy nazw ID    
	auto status = cocoFiles.LoadCOCOFile("coco.names");   //"x64/data/coco.names"
	for (int i = 0; i < cocoFiles.NamesCocoArray.size(); i++) {
		ui.cocoIDcomboBox->insertItem(i, QString::fromStdString(cocoFiles.NamesCocoArray[i]));
	}
	cocoFiles.~cocoFiles();
	if (status != StatusCode::OK)
		ui.txtConsole->appendPlainText("Cannot load to comboBox!");

	// Wczytanie wolnych portów COM
	setSerialPortComboBoxData();
}


void TrackingApp::on_btnInitYolo_clicked()
{
	mOpenCV_videoCapture->start(QThread::HighPriority);
	ui.txtConsole->appendPlainText("Start thread with YOLO");
	ui.btnInitYolo->setDisabled(true);
	ui.btnCaptureVideo->setDisabled(false);
}

void TrackingApp::on_btnCaptureVideo_clicked()
{
	mOpenCV_videoCapture->requestInterruption();
	mOpenCV_videoCapture->wait();
	ui.txtConsole->appendPlainText("Stopped thread with YOLO");
	ui.btnInitYolo->setDisabled(false);
	ui.btnCaptureVideo->setDisabled(true);
}

void TrackingApp::on_btnOpenSerialPort_clicked()
{
	ui.txtConsole->appendPlainText("Openning Serial Port");
	OpenSerialPort(ui.serialPortComboBox->currentText());
	ui.btnOpenSerialPort->setDisabled(true);
	ui.btnCloseSerialPort->setDisabled(false);
}

void TrackingApp::on_btnCloseSerialPort_clicked()
{
	serial.close();
	ui.txtConsole->appendPlainText("Closing Serial Port");
	ui.btnOpenSerialPort->setDisabled(false);
	ui.btnCloseSerialPort->setDisabled(true);
	setSerialPortComboBoxData();
}

void TrackingApp::on_btnShoot_pressed()
{
	ui.txtConsole->appendPlainText("OPEN FIRE!");
	if (ui.checkBoxSendUART->isEnabled()) {
		serial.write("###5005001");
	}

}

void TrackingApp::on_btnShoot_released()
{
	ui.txtConsole->appendPlainText("HOLD YOUR FIRE!");
	if (!ui.checkBoxSendUART->isEnabled()) {
		serial.write("###5005000");
	}
}
void TrackingApp::on_txtX_returnPressed() {
	int x = ui.txtX->text().toInt() + 500; // 0-499 ujemne, 500-999 dodatnie;
	QString str = "###" + QString::number(x) + "5000";

	ui.txtConsole->appendPlainText(str);
	QByteArray rawData = str.toUtf8();
	serial.write(rawData);
}

void TrackingApp::on_txtY_returnPressed() {

	int y = ui.txtY->text().toInt() + 500; // 0-499 ujemne, 500-999 dodatnie;
	QString str = "###500"+ QString::number(y) +"0";
	ui.txtConsole->appendPlainText(str);
	QByteArray rawData = str.toUtf8();
	serial.write(rawData);
}

void TrackingApp::on_trackedObjectNumber_ledit_returnPressed() {

	ui.txtConsole->appendPlainText("Tracked object number is set to " + ui.trackedObjectNumber_ledit->text());
	mOpenCV_videoCapture->setTrackedObjectNumber(ui.trackedObjectNumber_ledit->text().toInt());
}

void TrackingApp::on_cocoIDcomboBox_currentIndexChanged(int index) {

	ui.txtConsole->appendPlainText("Tracked object ID is set to " + ui.cocoIDcomboBox->currentText());
	mOpenCV_videoCapture->setTrackedObjectID(ui.cocoIDcomboBox->currentIndex());
}
void TrackingApp::on_checkBoxSendUART_toggled(bool enabled) {
	mOpenCV_videoCapture->setUARTTransmissionAutonomously(enabled);
	ui.txtConsole->appendPlainText(("Transmission state is " + std::string(enabled ? "enabled" : "disabled")).c_str());
}


void TrackingApp::OpenSerialPort(const QString& portName) {
	serial.setPortName(portName);
	serial.setBaudRate(QSerialPort::Baud115200);
	serial.setDataBits(QSerialPort::Data8);
	serial.setParity(QSerialPort::NoParity);
	serial.setStopBits(QSerialPort::OneStop);
	serial.setFlowControl(QSerialPort::NoFlowControl);
	serial.open(QIODevice::WriteOnly);
}


TrackingApp::~TrackingApp() {
	if (mOpenCV_videoCapture->isRunning()) {
		mOpenCV_videoCapture->requestInterruption();
		mOpenCV_videoCapture->wait();
	}
}



void TrackingApp::setSerialPortComboBoxData() {
	// Wczytanie wolnych portów COM
	ui.serialPortComboBox->clear();
	QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
	QSerialPortInfo port;

	foreach(port, com_ports)
	{
		ui.serialPortComboBox->addItem(port.portName());
	}
}

