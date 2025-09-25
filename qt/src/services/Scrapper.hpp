#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QUrl>

class Scrapper : public QObject {
	Q_OBJECT

	public slots:
		void	initialize(QUrl baseUrl);
		void	run();

	signals:
		void	scrappingFinished();
		void	running();
		void	ran();

	private:
		QNetworkAccessManager*	networkManager;
		QUrl					baseUrl;
};