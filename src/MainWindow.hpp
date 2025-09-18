#pragma once
#include <QMainWindow>
#include <QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

	public:
		explicit	MainWindow(QWidget* parent = nullptr);

	private:
		void	customizeWindow();
		void	buildUi();
};