#include <QVBoxLayout>
#include <QGuiApplication>
#include <QScreen>


#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    buildUi();
}

void MainWindow::customizeWindow() {
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setFixedSize(QSize(800, 600));
	
	QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
	int x = (screenGeometry.width() - this->width()) / 2;
	int y = (screenGeometry.height() - this->height()) / 2;
	this->move(x, y);
}

void MainWindow::buildUi() {
	this->customizeWindow();

	// Central widget
	QWidget* centralWidget = new QWidget(this);
	auto* vbox = new QVBoxLayout();
	centralWidget->setLayout(vbox);
	this->setCentralWidget(centralWidget);
	centralWidget->setStyleSheet("background: rgba(185, 185, 185, 1);");

	// Header widget
	QGridLayout* headerLayout = new QGridLayout();
	QWidget* headerWidget = new QWidget(this);
	headerWidget->setLayout(headerLayout);
	headerWidget->setStyleSheet("background: rgba(255, 255, 255, 1);");
	headerWidget->setFixedHeight(40);
	vbox->addWidget(headerWidget);

	// Body widget
	QGridLayout* bodyLayout = new QGridLayout();
	QWidget* bodyWidget = new QWidget(this);
	bodyWidget->setLayout(bodyLayout);
	bodyWidget->setStyleSheet("background: rgba(255, 255, 255, 1);");
	vbox->addWidget(bodyWidget);
}
