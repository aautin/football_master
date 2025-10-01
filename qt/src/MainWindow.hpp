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
	// Color constants for easy UI customization
	const QString COLOR_BG = "rgba(8, 72, 135, 1)";
	const QString COLOR_HEADER_TEXT = "rgba(247, 245, 251, 1)";
	const QString COLOR_BUTTON_TEXT = "rgba(247, 245, 251, 1)";
	const QString COLOR_BUTTON_BG = "rgba(214, 127, 45, 1)";
	const QString COLOR_BUTTON_BG_HOVER = "rgba(143, 105, 69, 1)";
	const QString COLOR_BUTTON_BG_CHECKED = "rgba(107, 66, 27, 1)";
	const QString COLOR_BORDER = "rgba(214, 211, 226, 1)";
	const QString COLOR_CHART_BG = "rgba(144, 156, 194, 1)";
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
		void	styleSheetUi();

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