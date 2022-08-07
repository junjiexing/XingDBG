//
// Created by xing on 2022/5/9.
//

#pragma once
#include <QTextEdit>
#include "LLDBCore.h"
#include "AbstractTableView.h"
#include <vector>


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
	struct Instruction
	{
		uint64_t address;
		QString disassembly;
		QString comment;
	};

	std::vector<Instruction> m_insts;

	uint64_t m_pcAddress;
};


