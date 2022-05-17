//
// Created by xing on 2022/5/11.
//

#pragma once
#include <QListWidget>
#include <kddockwidgets/DockWidget.h>
#include "LLDBCore.h"


class CallStackView : public KDDockWidgets::DockWidget
{
	Q_OBJECT
public:
	CallStackView();

	void setThread(lldb::SBThread thread);

private:
	uint64_t m_tid = 0;
	QListWidget* m_listWidget;
};


