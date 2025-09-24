#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

class Scrapper : public QObject {
	Q_OBJECT

	public:
		explicit Scrapper(QUrl baseUrl, QObject* parent = nullptr);
		~Scrapper();

		void run();
		bool isRunning() const;
	
	signals:
		void scrappingFinished();

	private:
		QNetworkAccessManager*	networkManager;
		QUrl					baseUrl;
};