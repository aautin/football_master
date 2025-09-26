#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include <QShortcut>
#include <QMessageBox>
#include <QWindow>

#include "MainWindow.hpp"
#include "Database.hpp"
#include "Scraper.hpp"

// ---------- Constructor Destructor ----------
MainWindow::MainWindow(char** envp, QWidget* parent)
    : QMainWindow(parent) {

	// UI Setup
	windowUi();
	centralUi();
	headerUi();
	sidebarUi();


	// Services Setup
	database = new Database();
	scraper = new Scraper();

	database->moveToThread(databaseThread = new QThread(this));
	scraper->moveToThread(scraperThread = new QThread(this));

	wireDatabaseSignals();
	wireScraperSignals();
	wireOtherSignals();

	connect(databaseThread, &QThread::started, database, [this]() {
	    database->initialize(QString::fromStdString(std::getenv("DB_HOST")),
			QString::fromStdString(std::getenv("DB_USER")),
			QString::fromStdString(std::getenv("DB_PASSWORD")),
			QString::fromStdString(std::getenv("DB_NAME")),
			5432
		);
	});
	connect(scraperThread, &QThread::started, scraper, [this]() {
	    scraper->initialize(QString("http://%1:8000").arg(QString::fromStdString(std::getenv("SCRAPER_HOST"))));
	});
	databaseThread->start();
	scraperThread->start();
}

MainWindow::~MainWindow() {
	databaseThread->quit();
	databaseThread->wait();
	delete databaseThread;
	delete database;

	scraperThread->quit();
	scraperThread->wait();
	delete scraperThread;
	delete scraper;
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

	updateDate = new QLabel(this);
	grid->addWidget(updateDate, 0, 8, 1, 3);
	updateDate->setText("Last Update: \n" + db->getDate().toString("dd/MM/yyyy hh:mm"));
	updateDate->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	updateDate->setFont(QFont("Arial", 7, QFont::Normal));

	btRefresh = new QPushButton(this);
	btRefresh->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btRefresh, 0, 11, 1, 1);
	btRefresh->setIcon(QIcon(":/assets/refresh.png"));
	btRefresh->setIconSize(btRefresh->size() * 0.9);
	btRefresh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	btMinimize = new QPushButton(this);
	btMinimize->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btMinimize, 0, 12, 1, 1);
	btMinimize->setIcon(QIcon(":/assets/minimize.png"));
	btMinimize->setIconSize(btMinimize->size() * 0.9);
	btMinimize->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	btClose = new QPushButton(this);
	btClose->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btClose, 0, 13, 1, 1);
	btClose->setIcon(QIcon(":/assets/close.png"));
	btClose->setIconSize(btClose->size() * 1);
	btClose->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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
	championshipsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
	teamsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grid->addWidget(teamsArea, 7, 11, 6, 3);
	teamsLayout = getScrollAreaLayout(teamsArea);
	teamsLayout->setAlignment(Qt::AlignTop);
	teamsGroup = new QButtonGroup();
	teamsGroup->setExclusive(false);

	QStringList competition_buttons;
	for (const Competition& comp : db->getTable<Competition>())
		competition_buttons.append(comp.name);
	fillButtonsGroup(competition_buttons, championshipsLayout, championshipsGroup);

	for (QAbstractButton* btn : championshipsGroup->buttons())
		connect(btn, &QPushButton::clicked, this, [this, btn]() {
			QStringList team_buttons;
			for (const Team& team : db->getTable<Team>())
				if (db->getName(team.competition_id) == btn->text()) team_buttons.append(team.name);
			fillButtonsGroup(team_buttons, teamsLayout, teamsGroup);
		});
	championshipsGroup->buttons().first()->click();
}




// ---------- Signals ----------
void MainWindow::wireServicesSignals() {
	// Database
	connect(database, &Database::initialized, this, [this]() {
		connect(championshipsGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* button) {
		    removeButtons(teamsLayout, teamsGroup);

			// Add the new teams buttons corresponding to the clicked competition
		});

		QMessageBox msgBox;
		msgBox.setText("Database is initialized.");
		msgBox.exec();
	});
	connect(database, &Database::destroyed, this, [this]() {

		QMessageBox msgBox;
		msgBox.setText("Database is destroyed.");
		msgBox.exec();
	});


	// Scraper
	connect(scraper, &Scraper::initialized, this, [this]() {
		btRefresh->setEnabled(true);
		connect(btRefresh, &QPushButton::clicked, scraper, &Scraper::run);		

		QMessageBox msgBox;
		msgBox.setText("Scraper is initialized.");
		msgBox.exec();
	});
	connect(scraper, &Scraper::running, this, [this]() {
		btRefresh->setEnabled(false);

		QMessageBox msgBox;
		msgBox.setText("Scraping is running.");
		msgBox.exec();
	});
	connect(scraper, &Scraper::ran, this, [this]() {
    	btRefresh->setEnabled(true);
		QMetaObject::invokeMethod(database, "update", Qt::QueuedConnection);

		QMessageBox msgBox;
		msgBox.setText("Scraping is ran.");
		msgBox.exec();
	});
	connect(scraper, &Scraper::destroyed, this, [this]() {
		btRefresh->setEnabled(false);

		QMessageBox msgBox;
		msgBox.setText("Scraping is destroyed.");
		msgBox.exec();
	});
}

void MainWindow::wireOtherSignals() {
	connect(btMinimize, &QPushButton::clicked, this, [this]() {this->showMinimized();});
	connect(btClose, &QPushButton::clicked, this, [this]() {this->close();});
}










// ---------- Utils ----------
void	MainWindow::removeButtons(QBoxLayout* layout, QButtonGroup* group) {
	while (!group->buttons().isEmpty()) {
		QAbstractButton* btn = group->buttons().first();
		group->removeButton(btn);

		QWidget* widget = layout->itemAt(0)->widget();
		layout->removeWidget(widget);
		widget->deleteLater();
	}
}

// QVBoxLayout* MainWindow::getScrollAreaLayout(QScrollArea *area) {
// 	QWidget* container = new QWidget();
// 	area->setWidget(container);
// 	container->setStyleSheet("background: #6b7888;");

// 	QVBoxLayout* layout = new QVBoxLayout();
// 	container->setLayout(layout);

// 	return layout;
// }

// void MainWindow::fillButtonsGroup(
// 	QStringList buttons, QBoxLayout* layout, QButtonGroup* group) {
// 	while (QLayoutItem* item = layout->takeAt(0)) {
// 		if (QWidget* widget = item->widget()) {
// 			group->removeButton(qobject_cast<QAbstractButton*>(widget));
// 			widget->deleteLater();
// 		}
// 		delete item;
// 	}

// 	for (const QString& buttonText : buttons) {
// 		QPushButton* button = new QPushButton(buttonText, this);
// 		button->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff; color: white;");
// 		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
// 		QFontMetrics fm(button->font());
// 		QString elidedText = fm.elidedText(button->text(), Qt::ElideRight, button->width() + 15);
// 		button->setText(elidedText);
// 		button->setCheckable(true);
// 		button->setFixedHeight(40);
// 		group->addButton(button);
// 		layout->addWidget(button);
// 	}
// }