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
	this->setStyleSheet(QString("background-color: %1; border: 2px solid %2;").arg(COLOR_BG, COLOR_BORDER));

	QScreen* screen = QGuiApplication::primaryScreen();
	QRect screenGeometry = screen->geometry();
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
	titleBar->setStyleSheet(QString("color: %1; background-color: %2; border: none;").arg(COLOR_HEADER_TEXT, COLOR_BG));

	updateDate = new QLabel(this);
	grid->addWidget(updateDate, 0, 9, 1, 3);
	updateDate->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	updateDate->setFont(QFont("Arial", 7, QFont::Normal));
	updateDate->setStyleSheet(QString("color: %1; background-color: %2; border: none;").arg(COLOR_HEADER_TEXT, COLOR_BG));

	btRefresh = new QPushButton(this);
	grid->addWidget(btRefresh, 0, 12, 1, 1);
	btRefresh->setIcon(QIcon(":/assets/refresh.png"));
	btRefresh->setIconSize(btRefresh->size() * 0.9);
	btRefresh->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	btRefresh->setCheckable(true);
	btRefresh->setStyleSheet(QString(
		"QPushButton { color: %1; background-color: %2; border: 2px solid %3; } "
		"QPushButton:hover { background-color: %4; } "
		"QPushButton:checked { background-color: %5; }"
	).arg(COLOR_BUTTON_TEXT, COLOR_BUTTON_BG, COLOR_BORDER, COLOR_BUTTON_BG_HOVER, COLOR_BUTTON_BG_CHECKED));

	btMinimize = new QPushButton(this);
	grid->addWidget(btMinimize, 0, 13, 1, 1);
	btMinimize->setIcon(QIcon(":/assets/minimize.png"));
	btMinimize->setIconSize(btMinimize->size() * 0.9);
	btMinimize->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	btMinimize->setCheckable(true);
	btMinimize->setStyleSheet(QString(
		"QPushButton { color: %1; background-color: %2; border: 2px solid %3; } "
		"QPushButton:hover { background-color: %4; } "
		"QPushButton:checked { background-color: %5; }"
	).arg(COLOR_BUTTON_TEXT, COLOR_BUTTON_BG, COLOR_BORDER, COLOR_BUTTON_BG_HOVER, COLOR_BUTTON_BG_CHECKED));

	btClose = new QPushButton(this);
	grid->addWidget(btClose, 0, 14, 1, 1);
	btClose->setIcon(QIcon(":/assets/close.png"));
	btClose->setIconSize(btClose->size() * 1);
	btClose->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	btClose->setCheckable(true);
	btClose->setStyleSheet(QString(
		"QPushButton { color: %1; background-color: %2; border: 2px solid %3; } "
		"QPushButton:hover { background-color: %4; } "
		"QPushButton:checked { background-color: %5; }"
	).arg(COLOR_BUTTON_TEXT, COLOR_BUTTON_BG, COLOR_BORDER, COLOR_BUTTON_BG_HOVER, COLOR_BUTTON_BG_CHECKED));
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
		chartViews[i]->setRenderHint(QPainter::Antialiasing);
		chartViews[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		chartViews[i]->chart()->setMargins(QMargins(0, 0, 0, 0));
		chartViews[i]->setStyleSheet(QString("background-color: %1; border: 2px solid %2;").arg(COLOR_CHART_BG, COLOR_BORDER));
		chartViews[i]->chart()->setBackgroundBrush(QBrush(QColor(qcolorFromRgbaString(COLOR_CHART_BG))));
		grid->addWidget(chartViews[i], 1 + (6 * (i / 2)), (i % 2) * 6, 6, 6);
	}
}

void MainWindow::sidebarUi() {
	QScrollArea* competitionsArea = new QScrollArea();
	competitionsArea->setWidgetResizable(true);
	competitionsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	competitionsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grid->addWidget(competitionsArea, 1, 12, 6, 3);
	competitionsLayout = getScrollAreaLayout(competitionsArea);
	competitionsArea->setStyleSheet(QString("border: 2px solid %1;").arg(COLOR_BORDER));
	competitionsArea->widget()->setStyleSheet(QString("background-color: %1; border: none;").arg(COLOR_CHART_BG, COLOR_BORDER));
	competitionsLayout->setAlignment(Qt::AlignTop);
	competitionsGroup = new QButtonGroup();
	competitionsGroup->setExclusive(true);

	QScrollArea* teamsArea = new QScrollArea();
	teamsArea->setWidgetResizable(true);
	teamsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	teamsArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	grid->addWidget(teamsArea, 7, 12, 6, 3);
	teamsLayout = getScrollAreaLayout(teamsArea);
	teamsArea->widget()->setStyleSheet(QString("background-color: %1; border: none;").arg(COLOR_CHART_BG, COLOR_BORDER));
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

		const QString seriesInfo[4][2] = {
			{ QStringLiteral("Goals For"),		QStringLiteral("Goals Against") },
			{ QStringLiteral("Possession (%)"),	QString("") },
			{ QStringLiteral("Shots"),			QStringLiteral("Shots on target") },
			{ QStringLiteral("Passes"),			QStringLiteral("Successful Passes") },
		};
		const int yAxes[4][2] = { { 1, 10 }, { 10, 100 }, { 5, 40 }, { 100, 1000 } };
		for (int i = 0; i < 4; ++i) {
			chartViews[i]->chart()->addSeries(createLineSeries(points[i * 2], seriesInfo[i][0], 4, QColor(50, 220, 50)));
			if (!seriesInfo[i][1].isEmpty())
				chartViews[i]->chart()->addSeries(createLineSeries(points[i * 2 + 1], seriesInfo[i][1], 2, QColor(240, 50, 50)));

			chartViews[i]->chart()->createDefaultAxes();
			chartViews[i]->chart()->axes(Qt::Horizontal).first()->hide();

			QValueAxis* axisY = qobject_cast<QValueAxis*>(chartViews[i]->chart()->axes(Qt::Vertical).first());
			axisY->setTickCount((yAxes[i][1] / yAxes[i][0]) + 1);
			axisY->setLabelFormat("%d");
			axisY->setRange(0, yAxes[i][1]);

		}
		
		for (int i = 0; i < 4; ++i) {
			QCategoryAxis* axisXcat = new QCategoryAxis();
			if (opponents[0].size() > 6)
				axisXcat->append(opponents[0].left(6), 1);
			else
				axisXcat->append(opponents[0], 1);
			for (int j = 1; j < opponents.size(); ++j)
				axisXcat->append(opponents[j], j + 1);

			axisXcat->setRange(1, opponents.size());
			axisXcat->setLabelsPosition(QCategoryAxis::AxisLabelsPositionOnValue);
			QFont axisFont = axisXcat->labelsFont();
			axisFont.setPointSize(5);
			axisXcat->setLabelsFont(axisFont);

			chartViews[i]->chart()->addAxis(axisXcat, Qt::AlignBottom);
			for (auto series : chartViews[i]->chart()->series())
				series->attachAxis(axisXcat);
		}

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
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		QFontMetrics fm(button->font());
		QString elidedText = fm.elidedText(button->text(), Qt::ElideRight, button->width() + 15);
		button->setText(elidedText);
		button->setProperty("fullName", buttonText);
		button->setCheckable(true);
		button->setFixedHeight(40);
		button->setStyleSheet(QString(
			"QPushButton { color: %1; background-color: %2; border: 2px solid %3; } "
			"QPushButton:hover { background-color: %4; } "
			"QPushButton:checked { background-color: %5; }"
		).arg(COLOR_BUTTON_TEXT, COLOR_BUTTON_BG, COLOR_BORDER, COLOR_BUTTON_BG_HOVER, COLOR_BUTTON_BG_CHECKED));
		group->addButton(button);
		layout->addWidget(button);
	}
}

QVBoxLayout* MainWindow::getScrollAreaLayout(QScrollArea *area) {
	QWidget* container = new QWidget(this);
	area->setWidget(container);

	QVBoxLayout* layout = new QVBoxLayout();
	container->setLayout(layout);

	return layout;
}