//
// Created by xing on 2022/5/9.
//

#pragma once
#include <QTextEdit>
#include "LLDBCore.h"
#include "AbstractTableView.h"


class DisassemblyView : public AbstractTableView
{
	Q_OBJECT

public:
	DisassemblyView();

	void setThreadFrame(lldb::SBThread thread, int index);
	int lineCount() override;
	void init() override;
protected:
	void drawLine(QPainter *p, int scrollLine, int currLine) override;

private:
	lldb::SBInstructionList m_insts;
	lldb::SBAddress m_pcAddress;
};


