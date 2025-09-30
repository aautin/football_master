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
#include <QChartView>

#include "data.hpp"

class Analyzer;
class Database;
class Scraper;
class MainWindow : public QMainWindow {
	Q_OBJECT

	public:
		explicit	MainWindow(char** envp, QWidget* parent = nullptr);
		~MainWindow() override;

	private:
		// UI Setup
		void	windowUi();
		void	headerUi();
		void	centralUi();
		void	sidebarUi();

		// Wire signals
		void	wireServicesSignals();
		void	wireOtherSignals();

		// Utils
		void			fillButtonsGroup(QBoxLayout* layout, QButtonGroup* group, const QStringList& buttons);
		void			removeButtons(QBoxLayout* layout, QButtonGroup* group);
		QVBoxLayout*	getScrollAreaLayout(QScrollArea* area);

		// UI Elements
		QLabel*			updateDate;
		QPushButton*	btRefresh;
		QPushButton*	btMinimize;
		QPushButton*	btClose;

		QGridLayout*	grid;
		QChartView*		chartViews[4];

		QVBoxLayout*	competitionsLayout;
		QButtonGroup*	competitionsGroup;

		QVBoxLayout*	teamsLayout;
		QButtonGroup*	teamsGroup;

		// Services
		Database*		database;
		QThread*		databaseThread;
		Scraper*		scraper;
		QThread*		scraperThread;
		Analyzer*		analyzer;
		QThread*		analyzerThread;

		QString			selectedTeam;

		bool 			_debug;
};