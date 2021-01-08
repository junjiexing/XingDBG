//
// Created by w4264 on 2021/1/8.
//

#pragma once
#include <QObject>
#include <QColor>

class App : public QObject
{
	Q_OBJECT

public:
	static App* get();

	void logError(QString const& msg);
	void logWarn(QString const& msg);
	void logInfo(QString const& msg);

signals:
	void outputMsg(QString const& msg, QColor const& color);

private:
	App();
};


