#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QPushButton>
#include <QBoxLayout>
#include <QStringList>
#include <QScrollArea>
#include <QWidget>
#include <QLabel>
#include <QThread>

class Database;
class Scraper;
class MainWindow : public QMainWindow {
    Q_OBJECT

	public:
		explicit	MainWindow(char** envp, QWidget* parent = nullptr);
		~MainWindow() override;

	private:
		// UI Setup
		void			windowUi();
		void			headerUi();
		void			centralUi();
		void			sidebarUi();

		// Wire signals
		void			wireServicesSignals();
		void			wireOtherSignals();

		// Utils
		void			removeButtons(QBoxLayout* layout, QButtonGroup* group);

		// UI Elements
		QLabel*			updateDate;
		QPushButton*	btRefresh;
		QPushButton*	btMinimize;
		QPushButton*	btClose;

		QGridLayout*	grid;
		QWidget*		dataVisualizer;

		QVBoxLayout*	championshipsLayout;
		QButtonGroup*	championshipsGroup;

		QVBoxLayout*	teamsLayout;
		QButtonGroup*	teamsGroup;

		// Services
		Database*		database;
		QThread*		databaseThread;
		Scraper*		scraper;
		QThread*		scraperThread;
};