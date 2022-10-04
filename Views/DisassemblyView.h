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
	bool gotoAddress(uint64_t address);
	int lineCount() override;
	void init() override;
protected:
	void drawLine(QPainter *p, int scrollLine, int currLine) override;
	void updateDisasmShow(lldb::SBAddress const& address);
	void contextMenuEvent(QContextMenuEvent *event) override;

private:
	bool disasmSymbolContext(lldb::SBSymbolContext& ctx, lldb::SBTarget& target);
	bool disasmAddress(uint64_t address, lldb::SBTarget& target);

	lldb::SBBreakpoint findBreakpointByAddress(lldb::addr_t addr);

private:
	lldb::SBInstructionList m_insts;
	lldb::SBAddress m_pcAddress;
};


