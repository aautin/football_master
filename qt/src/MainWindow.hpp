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

class Database;
class MainWindow : public QMainWindow {
    Q_OBJECT

	public:
		explicit	MainWindow(char** envp, QWidget* parent = nullptr);

	private:
		void			windowUi();
		void			headerUi();
		void			centralUi();
		void			sidebarUi();

		QVBoxLayout*	getScrollAreaLayout(QScrollArea* area);
		void			fillButtonsGroup(
			QStringList buttons, QBoxLayout* layout, QButtonGroup* group);

		QGridLayout*	grid;
		QWidget*		dataVisualizer;

		QVBoxLayout*	championshipsLayout;
		QButtonGroup*	championshipsGroup;

		QVBoxLayout*	teamsLayout;
		QButtonGroup*	teamsGroup;

		Database*		db;
};