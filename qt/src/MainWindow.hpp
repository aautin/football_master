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

#include "Data.hpp"

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
		QStringList		extractNames(const QList<Competition>& items);
		QStringList		extractNames(const QList<Team>& items);

		// UI Elements
		QLabel*			updateDate;
		QPushButton*	btRefresh;
		QPushButton*	btMinimize;
		QPushButton*	btClose;

		QGridLayout*	grid;
		QWidget*		dataVisualizer;

		QVBoxLayout*	competitionsLayout;
		QButtonGroup*	competitionsGroup;

		QVBoxLayout*	teamsLayout;
		QButtonGroup*	teamsGroup;

		// Services
		Database*		database;
		QThread*		databaseThread;
		Scraper*		scraper;
		QThread*		scraperThread;
};