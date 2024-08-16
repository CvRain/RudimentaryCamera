//
// Created by cvrain-thinkbook on 2024/8/16.
//

#ifndef _MAIN_WINDOW_HPP_
#define _MAIN_WINDOW_HPP_

#include <QCamera>
#include <QComboBox>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QMediaRecorder>
#include <QPushButton>
#include <QVideoWidget>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
	class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget {
Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow() override;
protected:
	void setupCamera();
	void recording();

protected slots:
	void onCameraChanged(const QString& cameraName);
	void onCameraFormatChanged(const QString& cameraFormatName);

private:
	Ui::MainWindow* ui;
	QList<QCameraDevice> camera_devices;
	QCamera* current_camera;
	QMediaRecorder* current_recorder;
	QMediaCaptureSession capture_session;
	QImageCapture* image_capture;
	QVideoWidget* video_widget;
	QList<QCameraFormat> camera_formats;

	bool is_recording = false;
	QComboBox* camera_list;
	QComboBox* camera_format_list;
	QPushButton* start_record_button;
};

#endif//_MAIN_WINDOW_HPP_
