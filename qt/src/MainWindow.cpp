#include <QGuiApplication>
#include <QApplication>
#include <QScreen>
#include <QShortcut>
#include <QMessageBox>
#include <QWindow>
#include <QValueAxis>
#include <QCategoryAxis>

#include "MainWindow.hpp"
#include "Database.hpp"
#include "Scraper.hpp"
#include "Analyzer.hpp"
#include "utils.hpp"

// ---------- Constructor Destructor ----------
MainWindow::MainWindow(char** envp, QWidget* parent)
    : QMainWindow(parent), _debug(isDebug(envp)) {

	// UI Setup
	windowUi();
	centralUi();
	headerUi();
	sidebarUi();

	// Services Setup
	database = new Database();
	scraper = new Scraper();
	analyzer = new Analyzer();

	database->moveToThread(databaseThread = new QThread(this));
	scraper->moveToThread(scraperThread = new QThread(this));
	analyzer->moveToThread(analyzerThread = new QThread(this));

	wireServicesSignals();
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
	analyzerThread->start();
}

MainWindow::~MainWindow() {
	delete databaseThread;
	delete scraperThread;
	delete analyzerThread;
	delete database;
	delete scraper;
	delete analyzer;

	delete competitionsGroup;
	delete teamsGroup;
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
	grid->addWidget(titleBar, 0, 0, 1, 12);
	titleBar->setText("Football Master");
	titleBar->setAlignment(Qt::AlignCenter);
	titleBar->setFont(QFont("Arial", 16, QFont::Bold));

	updateDate = new QLabel(this);
	grid->addWidget(updateDate, 0, 9, 1, 3);
	updateDate->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	updateDate->setFont(QFont("Arial", 7, QFont::Normal));

	btRefresh = new QPushButton(this);
	btRefresh->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btRefresh, 0, 12, 1, 1);
	btRefresh->setIcon(QIcon(":/assets/refresh.png"));
	btRefresh->setIconSize(btRefresh->size() * 0.9);
	btRefresh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	btMinimize = new QPushButton(this);
	btMinimize->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btMinimize, 0, 13, 1, 1);
	btMinimize->setIcon(QIcon(":/assets/minimize.png"));
	btMinimize->setIconSize(btMinimize->size() * 0.9);
	btMinimize->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	btClose = new QPushButton(this);
	btClose->setStyleSheet("background: #8e737d; border: 2px solid #e8e8e8ff");
	grid->addWidget(btClose, 0, 14, 1, 1);
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
	for (int col = 0; col < 15; ++col) {
		grid->setColumnMinimumWidth(col, this->width() / 15);
		grid->setColumnStretch(col, 0);
	}

	for (int i = 0; i < 4; ++i) {
	    chartViews[i] = new QChartView(new QChart());
		clearChart(chartViews[i]);
		chartViews[i]->chart()->setBackgroundBrush(QBrush(QColor("#6b7888")));
		chartViews[i]->setRenderHint(QPainter::Antialiasing);
		chartViews[i]->setProperty("variant", "chartView");
		chartViews[i]->setStyleSheet("[variant=chartView] { background: #6b7888; border: 2px solid #e8e8e8ff }");
		chartViews[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		chartViews[i]->chart()->setMargins(QMargins(0, 0, 0, 0));
	}
	
	grid->addWidget(chartViews[0], 1, 0, 6, 6);   // Top-left
	grid->addWidget(chartViews[1], 1, 6, 6, 6);   // Top-right
	grid->addWidget(chartViews[2], 7, 0, 6, 6);   // Bottom-left
	grid->addWidget(chartViews[3], 7, 6, 6, 6);   // Bottom-right
}

void MainWindow::sidebarUi() {
	QScrollArea* competitionsArea = new QScrollArea();
	competitionsArea->setProperty("variant", "area");
	competitionsArea->setStyleSheet("[variant=area] { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	competitionsArea->setWidgetResizable(true);
	competitionsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	competitionsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grid->addWidget(competitionsArea, 1, 12, 6, 3);
	competitionsLayout = getScrollAreaLayout(competitionsArea);
	competitionsLayout->setAlignment(Qt::AlignTop);
	competitionsGroup = new QButtonGroup();
	competitionsGroup->setExclusive(true);

	QScrollArea* teamsArea = new QScrollArea();
	teamsArea->setProperty("variant", "area");
	teamsArea->setStyleSheet("[variant=area] { background: #6b7888; border: 2px solid #e8e8e8ff; }");
	teamsArea->setWidgetResizable(true);
	teamsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	teamsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grid->addWidget(teamsArea, 7, 12, 6, 3);
	teamsLayout = getScrollAreaLayout(teamsArea);
	teamsLayout->setAlignment(Qt::AlignTop);
	teamsGroup = new QButtonGroup();
	teamsGroup->setExclusive(false);
}



// ---------- Signals ----------
void MainWindow::wireServicesSignals() {
	// Database <-> UI
	connect(database, &Database::initialized, this, [this]() {
		QMetaObject::invokeMethod(database, "fetchCompetitions", Qt::QueuedConnection);

		debug("Database is initialized.", _debug);
	});
	connect(database, &Database::updated, this, [this]() {
		QMetaObject::invokeMethod(database, "fetchCompetitions", Qt::QueuedConnection);
		QMetaObject::invokeMethod(database, "fetchLastUpdateDate", Qt::QueuedConnection);

		debug("Database is updated.", _debug);
	});
	connect(database, &Database::competitionsFetched, this, [this](const QList<Competition>& competitions) {
		removeButtons(competitionsLayout, competitionsGroup);
		fillButtonsGroup(competitionsLayout, competitionsGroup, extractNames(competitions));
		competitionsGroup->buttons().first()->click();

		debug("Database is competitions fetched.", _debug);
	});
	connect(database, &Database::teamsFetched, this, [this](const QList<Team>& teams) {
		removeButtons(teamsLayout, teamsGroup);
		fillButtonsGroup(teamsLayout, teamsGroup, extractNames(teams));
		for (QAbstractButton* btn : teamsGroup->buttons())
			if (selectedTeam == btn->property("fullName").toString()) btn->setChecked(true);

		debug("Database is teams fetched.", _debug);
	});
	connect(database, &Database::lastUpdateDateFetched, this, [this](const QDateTime& date) {
		updateDate->setText(QString("Last update:\n %1").arg(date.toString("yyyy-MM-dd HH:mm")));
		
		debug("Database last update date fetched.", _debug);
	});
	connect(database, &Database::destroyed, this, [this]() {
		debug("Database is destroyed.", _debug);
	});
	connect(competitionsGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* btn) {
	    QMetaObject::invokeMethod(database, "fetchTeams", Qt::QueuedConnection, Q_ARG(QString, btn->property("fullName").toString()));
	});


	// Scraper <-> UI
	connect(scraper, &Scraper::initialized, this, [this]() {
		btRefresh->setEnabled(true);
		connect(btRefresh, &QPushButton::clicked, scraper, &Scraper::run);	// UI -> Scraper

		debug("Scraper is initialized.", _debug);
	});
	connect(scraper, &Scraper::running, this, [this]() {
		btRefresh->setEnabled(false);
	});
	connect(scraper, &Scraper::ran, this, [this]() {
    	btRefresh->setEnabled(true);
		QMetaObject::invokeMethod(database, "update", Qt::QueuedConnection);

		debug("Scraping is ran.", _debug);
	});
	connect(scraper, &Scraper::destroyed, this, [this]() {
		btRefresh->setEnabled(false);

		debug("Scraping is destroyed.", _debug);
	});

	// Database <-> Analyzer
	connect(database, &Database::matchesFetched, this, [this](const QString& teamName, const QList<Match>& matches) {
		QMetaObject::invokeMethod(analyzer, "analyze", Qt::QueuedConnection, Q_ARG(QString, teamName), Q_ARG(QList<Match>, matches));

		debug("Database is matches fetched.", _debug);
	});

	// Analyzer <-> UI
	connect(analyzer, &Analyzer::analyzed, this, [this](const QString& teamName, const QList<QList<QPointF>>& points, const QList<QString>& opponents) {
		if (teamName != selectedTeam) return;
		for (auto chartView : chartViews) clearChart(chartView);
		chartViews[0]->chart()->setTitle(QString("Performance of %1").arg(teamName));

		chartViews[0]->chart()->addSeries(createLineSeries(points[0], "Goals For", 4, QColor(100, 200, 100)));
		chartViews[0]->chart()->addSeries(createLineSeries(points[1], "Goals Against", 2, QColor(220, 100, 100)));
		chartViews[0]->chart()->createDefaultAxes();
		chartViews[0]->chart()->axes(Qt::Vertical).first()->setRange(0, 10);
		chartViews[0]->chart()->axes(Qt::Horizontal).first()->hide();
		QValueAxis* axisY = qobject_cast<QValueAxis*>(chartViews[0]->chart()->axes(Qt::Vertical).first());
		axisY->setTickCount(11);
		axisY->setLabelFormat("%d");

		QValueAxis* axisX = qobject_cast<QValueAxis*>(chartViews[0]->chart()->axes(Qt::Horizontal).first());
		QCategoryAxis* axisXcat = new QCategoryAxis();
		if (opponents[0].size() > 6)
			axisXcat->append(opponents[0].left(6), 1);
		else
			axisXcat->append(opponents[0], 1);
		for (int i = 1; i < opponents.size(); ++i)
			axisXcat->append(opponents[i], i + 1);
		axisXcat->setRange(1, opponents.size());
		axisXcat->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
		QFont axisFont = axisXcat->labelsFont();
		axisFont.setPointSize(7);
		axisXcat->setLabelsFont(axisFont);

		chartViews[0]->chart()->addAxis(axisXcat, Qt::AlignBottom);
		for (auto series : chartViews[0]->chart()->series())
			series->attachAxis(axisXcat);

		debug("Analyzer is analyzed.", _debug);
	});
}

void MainWindow::wireOtherSignals() {
	connect(btMinimize, &QPushButton::clicked, this, [this]() { this->showMinimized(); });
	connect(btClose, &QPushButton::clicked, this, [this]() {
		databaseThread->quit();
		scraperThread->quit();
		analyzerThread->quit();
		databaseThread->wait();
		scraperThread->wait();
		analyzerThread->wait();

		this->close();
	});

	connect(teamsGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* btn) {
		if (btn && btn->isChecked()) {
			selectedTeam = btn->property("fullName").toString();
			QMetaObject::invokeMethod(database, "fetchMatches", Qt::QueuedConnection, Q_ARG(QString, selectedTeam));
		}
		else {
			selectedTeam = "None";
			for (auto chartView : chartViews) clearChart(chartView);
		}

		for (auto b : teamsGroup->buttons())
			if (b != btn) b->setChecked(false);

		debug(QString(selectedTeam), _debug);
	});
}



// ---------- Utils ----------
void MainWindow::removeButtons(QBoxLayout* layout, QButtonGroup* group) {
	while (!group->buttons().isEmpty()) {
		QAbstractButton* btn = group->buttons().first();
		group->removeButton(btn);

		QWidget* widget = layout->itemAt(0)->widget();
		layout->removeWidget(widget);
		widget->deleteLater();
	}
}

void MainWindow::fillButtonsGroup(QBoxLayout* layout, QButtonGroup* group, const QStringList& buttons) {
	for (const QString& buttonText : buttons) {
		QPushButton* button = new QPushButton(buttonText, this);
		button->setStyleSheet(
			"QPushButton { background: #8e737d; border: 2px solid #e8e8e8ff; color: white; }"
			"QPushButton:checked { background: #4a5a6a; color: #ffd700; }"
		);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		QFontMetrics fm(button->font());
		QString elidedText = fm.elidedText(button->text(), Qt::ElideRight, button->width() + 15);
		button->setText(elidedText);
		button->setProperty("fullName", buttonText);
		button->setCheckable(true);
		button->setFixedHeight(40);
		group->addButton(button);
		layout->addWidget(button);
	}
}

QVBoxLayout* MainWindow::getScrollAreaLayout(QScrollArea *area) {
	QWidget* container = new QWidget();
	area->setWidget(container);
	container->setStyleSheet("background: #6b7888;");

	QVBoxLayout* layout = new QVBoxLayout();
	container->setLayout(layout);

	return layout;
}