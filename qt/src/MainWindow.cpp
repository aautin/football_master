#include <QVBoxLayout>
#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include <QShortcut>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QScrollArea>

#include <QDebug>
#include <QWindow>

#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
	windowUi();
	centralUi();
	headerUi();
	sidebarUi();
}



// ---------- UI ----------
void MainWindow::windowUi() {
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setFixedSize(QSize(700, 650));
	this->setProperty("variant", "mainWindow");
	this->setStyleSheet("[variant=mainWindow] { border: 2px solid #e8e8e8ff; }");
	
	QScreen* primaryScreen = QGuiApplication::primaryScreen();
	QRect screenGeometry = primaryScreen->geometry();
	int x = screenGeometry.x() + (screenGeometry.width() - this->width()) / 2;
	int y = screenGeometry.y() + (screenGeometry.height() - this->height()) / 2;
	this->move(x, y);
}

void MainWindow::headerUi() {
	QLabel* titleBar = new QLabel(this);
	grid->addWidget(titleBar, 0, 0, 1, 11);
	titleBar->setText("Football Master");
	titleBar->setAlignment(Qt::AlignCenter);
	titleBar->setFont(QFont("Arial", 16, QFont::Bold));

	QPushButton* btRefresh = new QPushButton(this);
	btRefresh->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btRefresh, 0, 11, 1, 1);
	btRefresh->setIcon(QIcon(":/assets/refresh.png"));
	btRefresh->setIconSize(btRefresh->size() * 0.9);
	btRefresh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QPushButton* btMinimize = new QPushButton(this);
	btMinimize->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btMinimize, 0, 12, 1, 1);
	btMinimize->setIcon(QIcon(":/assets/minimize.png"));
	btMinimize->setIconSize(btMinimize->size() * 0.9);
	btMinimize->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(btMinimize, &QPushButton::clicked, this, [this]() { this->showMinimized(); });

	QPushButton* btClose = new QPushButton(this);
	btClose->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btClose, 0, 13, 1, 1);
	btClose->setIcon(QIcon(":/assets/close.png"));
	btClose->setIconSize(btClose->size() * 1);
	btClose->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(btClose, &QPushButton::clicked, this, [this]() { this->close(); });
}

void MainWindow::centralUi() {
	// Central widget : grid layout
	QWidget* centralWidget = new QWidget(this);
	grid = new QGridLayout();
	centralWidget->setLayout(grid);
	this->setCentralWidget(centralWidget);

	for (int row = 0; row < 13; ++row) {
		grid->setRowMinimumHeight(row, this->height() / 13);
		grid->setRowStretch(row, 0);
	}
	for (int col = 0; col < 14; ++col) {
		grid->setColumnMinimumWidth(col, this->width() / 14);
		grid->setColumnStretch(col, 0);
	}

	dataVisualizer = new QWidget(this);
	dataVisualizer->setProperty("variant", "dataVisualizer");
	dataVisualizer->setStyleSheet("[variant=dataVisualizer] { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	grid->addWidget(dataVisualizer, 1, 0, 12, 11);
}

void MainWindow::sidebarUi() {
	QScrollArea* championshipsArea = new QScrollArea();
	championshipsArea->setProperty("variant", "area");
	championshipsArea->setStyleSheet("[variant=area] { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	championshipsArea->setWidgetResizable(true);
	championshipsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grid->addWidget(championshipsArea, 1, 11, 6, 3);
	championshipsLayout = getScrollAreaLayout(championshipsArea);
	championshipsLayout->setAlignment(Qt::AlignTop);
	championshipsGroup = new QButtonGroup();
	championshipsGroup->setExclusive(true);

	QScrollArea* teamsArea = new QScrollArea();
	teamsArea->setProperty("variant", "area");
	teamsArea->setStyleSheet("[variant=area] { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	teamsArea->setWidgetResizable(true);
	teamsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grid->addWidget(teamsArea, 7, 11, 6, 3);
	teamsLayout = getScrollAreaLayout(teamsArea);
	teamsLayout->setAlignment(Qt::AlignTop);
	teamsGroup = new QButtonGroup();
	teamsGroup->setExclusive(false);
}



// ---------- Utils ----------
QVBoxLayout* MainWindow::getScrollAreaLayout(QScrollArea *area) {
	QWidget* container = new QWidget();
	area->setWidget(container);
	container->setStyleSheet("background: #6b7888;");

	QVBoxLayout* layout = new QVBoxLayout();
	container->setLayout(layout);

	return layout;
}

void MainWindow::fillButtonsGroup(
	QStringList buttons, void (*btnCustomizer)(QPushButton*),
	QBoxLayout* layout, QButtonGroup* group) {

	for (const QString& buttonText : buttons) {
		QPushButton* button = new QPushButton(buttonText, this);
		btnCustomizer(button);
		group->addButton(button);
		layout->addWidget(button);
	}
}
