#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <unistd.h>

#include "Scrapper.hpp"

#include <iostream>
// ---------- Constructors Destructors ----------
Scrapper::Scrapper(QUrl baseUrl, QObject* parent) : QObject(parent),
	baseUrl(baseUrl), networkManager(new QNetworkAccessManager()) {
}

Scrapper::~Scrapper() { delete networkManager; }

// ---------- Methods ----------
void Scrapper::run() {
    if (!networkManager) networkManager = new QNetworkAccessManager(this);

    QNetworkRequest req(QUrl(baseUrl.toString() + "/do-it"));
    auto* reply = networkManager->post(req, QByteArray{});

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(5000);
    loop.exec();

    if (timer.isActive() && reply->error() == QNetworkReply::NoError) {
        // OK: la requête a atteint le serveur
        // Optionnel: parse la réponse si besoin
    } else {
        // qWarning() << "do-it failed:" << reply->errorString();
        if (reply->isRunning()) reply->abort();
    }
    reply->deleteLater();

	size_t time = 2;
	while (isRunning()) {
		QThread::sleep(time);
		if (time < 10) time *= 2;
	}
	emit scrappingFinished();
}

bool Scrapper::isRunning() const {
	QNetworkRequest request(baseUrl.toString() + "/status");
	
	auto* reply = networkManager->get(request);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(3000); // timeout 3s
    loop.exec();       // bloque jusqu'à finished() ou timeout

    bool running = false;
    if (timer.isActive() && reply->error() == QNetworkReply::NoError) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
            running = doc.object().value("running").toBool(false);
    } else {
        // Optionnel: qWarning() << "Status request failed:" << reply->errorString();
        if (reply->isRunning()) reply->abort();
    }

    reply->deleteLater();
    return running;
}