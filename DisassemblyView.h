//
// Created by xing on 2022/5/9.
//

#pragma once
#include <QTextEdit>
#include <kddockwidgets/DockWidget.h>
#include "LLDBCore.h"


class DisassemblyView : public KDDockWidgets::DockWidget
{
	Q_OBJECT

public:
	DisassemblyView();

	void setThreadFrame(lldb::SBThread thread, int index);

private:
	QTextEdit* m_textEdit;
};


