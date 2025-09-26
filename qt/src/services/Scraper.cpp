#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

#include "Scraper.hpp"

// ---------- Public slots ----------
void Scraper::initialize(QString url) {
	if (isInitialized) return;

	this->baseUrl = url;
	networkManager = new QNetworkAccessManager(this);
	statusTimer = new QTimer(this);
	connect(statusTimer, &QTimer::timeout, this, [this]() {
		QNetworkRequest statusRequest(QUrl(this->baseUrl + "/status"));
		QNetworkReply* statusReply = networkManager->get(statusRequest);

		connect(statusReply, &QNetworkReply::finished, this, [this, statusReply]() {
			if (statusReply->error() == QNetworkReply::NoError) {
				QJsonDocument doc = QJsonDocument::fromJson(statusReply->readAll());
				QJsonObject obj = doc.object();
				
				if (obj.contains("running") && !obj["running"].toBool()) {
					emit ran();
					statusTimer->stop();
				}
				else {
					emit running();
					statusTimer->start(3000);
				}
			}
			statusReply->deleteLater();
		});
	});

	isInitialized = true;

	for (const auto& slot : pendingSlots) slot();
	pendingSlots.clear();

	emit initialized();
}

void Scraper::run() {
	if (!isInitialized) {
		pendingSlots.enqueue([this]() { run(); });
		return;
	}

	QNetworkRequest doItRequest(QUrl(baseUrl + "/do-it"));
	QNetworkReply* doItReply = networkManager->post(doItRequest, QByteArray());

	connect(doItReply, &QNetworkReply::finished, this, [this, doItReply]() {
		if (doItReply->error() == QNetworkReply::NoError) {
			emit running();
			statusTimer->start(3000);
		}
		doItReply->deleteLater();
	});
}

void Scraper::destroy() {
	if (!isInitialized) {
		pendingSlots.enqueue([this]() { destroy(); });
		return;
	}

	isInitialized = false;

	networkManager->deleteLater();
	statusTimer->stop();
	statusTimer->deleteLater();
	pendingSlots.clear();

	emit destroyed();
}