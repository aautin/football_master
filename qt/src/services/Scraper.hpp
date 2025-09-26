#pragma once

#include <QObject>
#include <QQueue>
#include <QNetworkAccessManager>
#include <QTimer>

class Scraper : public QObject {
	Q_OBJECT

	public slots:
		void	initialize(QString baseUrl);
		void	run();
		void	destroy();

	signals:
		void	initialized();
		void	running();
		void	ran();
		void	destroyed();

	private:
		QNetworkAccessManager*	networkManager;
		QString					baseUrl;
		QTimer*					statusTimer;

		bool							isInitialized = false;
		QQueue<std::function<void()>>	pendingSlots;
};