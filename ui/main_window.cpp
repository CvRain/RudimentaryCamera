#include "main_window.hpp"
#include "ui_main_window.h"

#include <QTime>
#include <QMessageBox>
#include <QDir>
#include <QObject>

MainWindow::MainWindow(QWidget* parent)
		:QWidget(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	current_camera = new QCamera();
	current_recorder = new QMediaRecorder();

	camera_list = ui->camera_combo_box;
	camera_format_list = ui->resolution_combo_box;
	video_widget = ui->camera_display;
	start_record_button = ui->pushButton;

	const auto func_check_connect = [this](const QMetaObject::Connection& connection){
		if(!connection){
			throw std::runtime_error("Connection slot failed");
		}else{
			qDebug() << "Connection success";
		}
	};

	func_check_connect(connect(camera_list, &QComboBox::currentTextChanged, this, &MainWindow::onCameraChanged));
	func_check_connect(connect(camera_format_list, &QComboBox::currentTextChanged, this, &MainWindow::onCameraFormatChanged));
	func_check_connect(connect(start_record_button, &QPushButton::clicked, this, &MainWindow::recording));

	setupCamera();
}

void MainWindow::setupCamera()
{
	camera_devices = QMediaDevices::videoInputs();
	if (camera_devices.empty()) {
		QMessageBox::warning(this, "Warning", "No camera found!");
		qDebug() << "No camera found";
		return;
	}
	for (const auto& camera : camera_devices) {
		qDebug() << "Camera found: " << camera.description();
		camera_list->addItem(camera.description());
	}

	if (!camera_devices.isEmpty()) {
		camera_list->setCurrentIndex(0);
	}
}

void MainWindow::recording()
{
	if(is_recording){
		start_record_button->setText ("结束录制");
		//获取当前的时间，并格式化为字符串
		const auto current_time = QTime::currentTime ().toString ("hh-mm-ss");
		//将文件保存在当前目录下的 hh-mm-ss.mp4
		current_recorder->setOutputLocation (QDir::currentPath () + "/" + current_time + ".mp4");
		current_recorder->record();
	}else{
		start_record_button->setText ("开始录制");
		current_recorder->stop();
	}
	is_recording = !is_recording;
}

MainWindow::~MainWindow()
{
	camera_devices.clear();
	camera_formats.clear();

	current_camera->deleteLater();
	current_recorder->deleteLater();
	image_capture->deleteLater();
	video_widget->deleteLater();
}

void MainWindow::onCameraChanged(const QString& cameraName)
{
	if (current_camera) {
		current_camera->stop();
	}

	if (current_recorder) {
		current_recorder->stop();
	}

	for (const auto& camera : camera_devices) {
		if (camera.description()==cameraName) {
			qDebug() << "Camera changed: " << cameraName;

			if(current_camera !=nullptr){
				current_camera->deleteLater();
				current_camera = nullptr;
			}
			current_camera = new QCamera(camera);

			capture_session.setCamera(current_camera);
			capture_session.setVideoOutput(video_widget);

			camera_format_list->clear();

			camera_formats = camera.videoFormats();
			for (const auto& format : camera_formats) {
				QString formatDescription = QString("%1 x %2 @ %3 FPS")
						.arg(format.resolution().width())
						.arg(format.resolution().height())
						.arg(format.maxFrameRate());
				camera_format_list->addItem(formatDescription, QVariant::fromValue(format));
			}

			if (!camera_formats.isEmpty()) {
				camera_format_list->setCurrentIndex(0);
				onCameraFormatChanged(camera_list->currentText());
			}

			if(current_recorder !=nullptr){
				current_recorder->deleteLater();
				current_recorder = nullptr;
			}
			current_recorder = new QMediaRecorder(current_camera);

			capture_session.setRecorder(current_recorder);
			current_camera->start();
			break;
		}
	}

}

void MainWindow::onCameraFormatChanged(const QString& cameraFormatName)
{
	if(current_recorder){
		current_recorder->stop();
	}
	if(current_camera){
		current_camera->stop();
	}
	if(current_camera ==nullptr){
		return;
	}

	QCameraFormat selected_format;
	for (const auto& format : camera_formats) {
		if (QString("%1 x %2 @ %3 FPS")
				.arg(format.resolution().width())
				.arg(format.resolution().height())
				.arg(format.maxFrameRate()) == cameraFormatName) {
			selected_format = format;
			break;
		}
	}

	current_camera->setCameraFormat (selected_format);

	current_recorder = new QMediaRecorder(current_camera);
	capture_session.setRecorder(current_recorder);

	current_camera->start();
}

