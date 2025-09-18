#pragma once

#include <QObject>
#include <QVector>
#include <QPair>
#include <QString>

#include "MainWindow.hpp"

enum Type {
	PLAYER = (1 << 0),
	TEAM = (1 << 1),
	CHAMPIONSHIP = (1 << 2),
	STAT = (1 << 3)
};

class Param {
	public:
		Param(const QString &name, const Type accessibility, )

};

class Selector : QObject {
	Q_OBJECT

	public:
		explicit Selector(QObject *parent = nullptr);

		void	getSelection(QString &mode, QString &scope, QString &data);
		void	getAvailables(QString &mode, QString &scope, QString &data);

	signals:
		void	selectionChanged(const QString &mode, const QString &scope, const QString &data);

	private:
		QString				selectedMode;
		QVector<QString>	selectedScope;
		QVector<QString>	selectedData;

		const QVector<QString>				mode;
		const QVector<QPair<Type, QString>>	scope;
		const QVector<QPair<Type, QString>>	data;
};