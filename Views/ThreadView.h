//
// Created by xing on 2022/5/11.
//

#pragma once
#include <QTableWidget>


class ThreadView : public QTableWidget
{
	Q_OBJECT

public:
	ThreadView();

public slots:
	void refresh();

};



