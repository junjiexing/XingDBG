//
// Created by xing on 2022/5/11.
//

#pragma once
#include <kddockwidgets/DockWidget.h>
#include <QTableWidget>


class ThreadView : public KDDockWidgets::DockWidget
{
	Q_OBJECT

public:
	ThreadView();

public slots:
	void refresh();

private:
	QTableWidget* m_tableWidget;
};



