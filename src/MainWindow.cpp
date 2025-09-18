#include <QVBoxLayout>
#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include <QShortcut>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>
#include <QScrollArea>

#include "MainWindow.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    buildUi();
}

void MainWindow::customizeWindow() {
	this->setWindowFlags(Qt::FramelessWindowHint);
	this->setFixedSize(QSize(700, 650));
	this->setStyleSheet("border: 2px solid #e8e8e8ff");
	
	QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
	this->move(
		(screenGeometry.width() - this->width()) / 2,
		(screenGeometry.height() - this->height()) / 2
	);
}

void MainWindow::buildUi() {
	QShortcut* escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
	connect(escShortcut, &QShortcut::activated, this, []() {
		QApplication::quit();
	});
	this->customizeWindow();

	// Central widget : grid layout
	QWidget* centralWidget = new QWidget(this);
	auto* grid = new QGridLayout();
	centralWidget->setLayout(grid);
	this->setCentralWidget(centralWidget);
	this->setObjectName("centralWidget");
	this->setStyleSheet("#centralWidget { border: 2px solid #e8e8e8ff; background: #9fadbd;}");

	for (int row = 0; row < 13; ++row) {
		grid->setRowMinimumHeight(row, this->height() / 13);
		grid->setRowStretch(row, 0);
	}
	for (int col = 0; col < 14; ++col) {
		grid->setColumnMinimumWidth(col, this->width() / 14);
		grid->setColumnStretch(col, 0);
	}

	// Title bar
	QLabel* titleBar = new QLabel(this);
	grid->addWidget(titleBar, 0, 0, 1, 11);
	titleBar->setText("Football Master");
	titleBar->setAlignment(Qt::AlignCenter);
	titleBar->setFont(QFont("Arial", 16, QFont::Bold));


	// Buttons
	QPushButton* btRefresh = new QPushButton(this);
	btRefresh->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btRefresh, 0, 11, 1, 1);
	btRefresh->setIcon(QIcon("assets/refresh.png"));
	btRefresh->setIconSize(btRefresh->size() * 0.9);
	btRefresh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QPushButton* btMinimize = new QPushButton(this);
	btMinimize->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btMinimize, 0, 12, 1, 1);
	btMinimize->setIcon(QIcon("assets/minimize.png"));
	btMinimize->setIconSize(btMinimize->size() * 0.9);
	btMinimize->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(btMinimize, &QPushButton::clicked, this, [this]() {
		this->showMinimized();
	});

	QPushButton* btClose = new QPushButton(this);
	btClose->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btClose, 0, 13, 1, 1);
	btClose->setIcon(QIcon("assets/close.png"));
	btClose->setIconSize(btClose->size() * 1);
	btClose->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(btClose, &QPushButton::clicked, this, [this]() {
		this->close();
	});

	// Data Visualizer
	QWidget* dataVisualizer = new QWidget(this);
	dataVisualizer->setStyleSheet("background: #6b7888; border: 2px solid #e8e8e8ff");
	grid->addWidget(dataVisualizer, 1, 0, 12, 11);


	// Side bar
	QWidget* sideBar = new QWidget(this);
	grid->addWidget(sideBar, 1, 11, 12, 3);
	sideBar->setLayout(new QVBoxLayout());
	sideBar->layout()->setContentsMargins(0, 0, 0, 0);


	// Side bar selectors
	QString styleSheet = R"(
			QPushButton {
				padding: 8px;
				background: #8e737d;
				border: 1px solid #e8e8e8;
			}
			QPushButton:checked {
				background: #3a4b5d;
				color: white;
			}
			QPushButton:hover { background: #3d4c5cff; color: white; }
			QPushButton:checked:hover { background: #3a4b5d; color: white; }
			QPushButton:focus { outline: none; }
	)";

	QWidget* visualSelector = new QWidget();
	sideBar->layout()->addWidget(visualSelector);
	visualSelector->setObjectName("visualSelector");
	visualSelector->setStyleSheet("#visualSelector { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	QVBoxLayout* visualLayout = new QVBoxLayout();
	visualSelector->setLayout(visualLayout);

	QLabel* entitySelector = new QLabel("", sideBar);
	sideBar->layout()->addWidget(entitySelector);
	entitySelector->setObjectName("entitySelector");
	entitySelector->setStyleSheet("#entitySelector { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	QVBoxLayout* entityLayout = new QVBoxLayout();
	entitySelector->setLayout(entityLayout);

	QLabel* dataSelector = new QLabel("", sideBar);
	sideBar->layout()->addWidget(dataSelector);
	dataSelector->setObjectName("dataSelector");
	dataSelector->setStyleSheet("#dataSelector { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	QVBoxLayout* dataLayout = new QVBoxLayout();
	dataSelector->setLayout(dataLayout);


	// Visual Selector
	QLabel* visualLabel = new QLabel("Visual", visualSelector);
	visualLayout->addWidget(visualLabel);
	visualLabel->setAlignment(Qt::AlignHCenter);
	visualLabel->setStyleSheet("background: transparent;");

	QButtonGroup* group = new QButtonGroup(this);
	group->setExclusive(true);
	QPushButton* b1 = new QPushButton("General", visualSelector);
	b1->setCheckable(true);
	group->addButton(b1);
	visualLayout->addWidget(b1);
	QPushButton* b2 = new QPushButton("Progress", visualSelector);
	b2->setCheckable(true);
	group->addButton(b2);
	visualLayout->addWidget(b2);
	QPushButton* b3 = new QPushButton("1v1", visualSelector);
	b3->setCheckable(true);
	group->addButton(b3);
	visualLayout->addWidget(b3);
	QPushButton* b4 = new QPushButton("Oncomings", visualSelector);
	b4->setCheckable(true);
	group->addButton(b4);
	visualLayout->addWidget(b4);

	b1->setStyleSheet(styleSheet);
	b2->setStyleSheet(styleSheet);
	b3->setStyleSheet(styleSheet);
	b4->setStyleSheet(styleSheet);
	b1->setChecked(true);


	// Entity Selector
	QLabel* entityLabel = new QLabel("Entity", entitySelector);
	entityLayout->addWidget(entityLabel);
	entityLabel->setAlignment(Qt::AlignHCenter);
	entityLabel->setStyleSheet("background: transparent;");

	QButtonGroup* entityGroup = new QButtonGroup(this);
	entityGroup->setExclusive(true);
	QPushButton* entiryb1 = new QPushButton("Ligue 1", visualSelector);
	entiryb1->setCheckable(true);
	entityGroup->addButton(entiryb1);
	entityLayout->addWidget(entiryb1);
	QPushButton* entiryb2 = new QPushButton("Bundesliga", visualSelector);
	entiryb2->setCheckable(true);
	entityGroup->addButton(entiryb2);
	entityLayout->addWidget(entiryb2);
	QPushButton* entiryb3 = new QPushButton("Premier League", visualSelector);
	entiryb3->setCheckable(true);
	entityGroup->addButton(entiryb3);
	entityLayout->addWidget(entiryb3);
	QPushButton* entiryb4 = new QPushButton("La Liga", visualSelector);
	entiryb4->setCheckable(true);
	entityGroup->addButton(entiryb4);
	entityLayout->addWidget(entiryb4);

	entiryb1->setStyleSheet(styleSheet);
	entiryb2->setStyleSheet(styleSheet);
	entiryb3->setStyleSheet(styleSheet);
	entiryb4->setStyleSheet(styleSheet);
	entiryb1->setChecked(true);



	// Data Selector
	QLabel* dataLabel = new QLabel("Data", dataSelector);
	dataLayout->addWidget(dataLabel);
	dataLabel->setAlignment(Qt::AlignHCenter);
	dataLabel->setStyleSheet("background: transparent;");


	QButtonGroup* data = new QButtonGroup(this);
	QPushButton* datab1 = new QPushButton("Goals", dataSelector);
	data->setExclusive(false);
	datab1->setCheckable(true);
	data->addButton(datab1);
	dataLayout->addWidget(datab1);
	QPushButton* datab2 = new QPushButton("Rank", dataSelector);
	datab2->setCheckable(true);
	data->addButton(datab2);
	dataLayout->addWidget(datab2);
	QPushButton* datab3 = new QPushButton("Ball Possession", dataSelector);
	datab3->setCheckable(true);
	data->addButton(datab3);
	dataLayout->addWidget(datab3);
	QPushButton* datab4 = new QPushButton("Expected Goals", dataSelector);
	datab4->setCheckable(true);
	data->addButton(datab4);
	dataLayout->addWidget(datab4);

	datab1->setStyleSheet(styleSheet);
	datab2->setStyleSheet(styleSheet);
	datab3->setStyleSheet(styleSheet);
	datab4->setStyleSheet(styleSheet);
}
