//
// Created by xing on 2022/5/11.
//

#pragma once
#include <kddockwidgets/DockWidget.h>
#include <QTreeWidget>
#include "LLDBCore.h"



class RegisterView final : public KDDockWidgets::DockWidget
{
	Q_OBJECT

public:
	RegisterView();

public slots:
	void setThreadFrame(lldb::SBThread thread, int index);

private:
	void addValue(lldb::SBValue value, QTreeWidgetItem* parent);

private:
	QTreeWidget* m_treeWidget;
};


