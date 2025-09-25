#include <QNetworkRequest>
#include <QNetworkReply>

#include "Scrapper.hpp"

// ---------- Public slots ----------
void Scrapper::initialize(QUrl baseUrl) {
    baseUrl = baseUrl;
    networkManager = new QNetworkAccessManager(this);


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