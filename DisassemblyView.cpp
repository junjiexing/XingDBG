//
// Created by xing on 2022/5/9.
//

#include "DisassemblyView.h"
#include "App.h"


DisassemblyView::DisassemblyView()
	: AbstractTableView(nullptr)
{
	setFrameStyle(QFrame::NoFrame);

	connect(App::get(), &App::onStopState, this, [this]
	{
		auto process = App::get()->getDbgCore()->getProcess();
		setThreadFrame(process.GetSelectedThread(), 0);
	});
	connect(App::get(), &App::onThreadFrameChanged, this, [this](uint64_t tid, int index)
	{
		auto process = App::get()->getDbgCore()->getProcess();
		setThreadFrame(process.GetThreadByID(tid), index);
	});
}

void DisassemblyView::setThreadFrame(lldb::SBThread thread, int index)
{
	m_insts.Clear();
	auto &target = core()->getTarget();
	auto frame = thread.GetFrameAtIndex(index);
	m_pcAddress = frame.GetPCAddress();

	auto sc = frame.GetSymbolContext(lldb::eSymbolContextFunction | lldb::eSymbolContextSymbol);
	auto func = sc.GetFunction();
	if (func.IsValid())
	{
		m_insts = func.GetInstructions(target, "intel");
	}
	else
	{
		auto sym = sc.GetSymbol();
		if (sym.IsValid() && (sym.GetType() == lldb::eSymbolTypeAbsolute || sym.GetStartAddress().IsValid()))
		{
			m_insts = sym.GetInstructions(target, "intel");
		}
		else
		{
			auto &process = core()->getProcess();
			lldb::SBMemoryRegionInfo region_info;
			auto err = process.GetMemoryRegionInfo(frame.GetPC(), region_info);
			if (err.Fail())
			{
				app()->w(tr("Cannot find memory region info: ") + err.GetCString());
				return;
			}

			// TODO: 更好的反汇编方式
//			auto base = region_info.GetRegionBase();
			auto base = m_pcAddress.GetLoadAddress(target);
			auto sz = region_info.GetRegionEnd() - base;
			std::vector<uint8_t> buffer(sz);

			lldb::SBError error;
			sz = process.ReadMemory(base, buffer.data(), sz, error);
			if (error.Fail())
			{
				app()->w(tr("Cannot read process memory"));
				return;
			}
			buffer.resize(sz);

			m_insts = target.GetInstructionsWithFlavor(base, "intel", buffer.data(), buffer.size());
		}
	}

	int i = 0;
	for (; i != m_insts.GetSize(); ++i)
	{
		auto inst = m_insts.GetInstructionAtIndex(i);
		if (inst.GetAddress() == m_pcAddress)
		{
			break;
		}
	}

	updateScrollBar();
	refresh();

	if (i != m_insts.GetSize())
	{
		scrollToLine(i);
	}
	else
	{
		// TODO: 花指令 or 反汇编错误？
	}

}
int DisassemblyView::lineCount()
{
	return int(m_insts.GetSize());
}

void DisassemblyView::drawLine(QPainter *p, int scrollLine, int currLine)
{
	auto target = core()->getTarget();
	auto inst = m_insts.GetInstructionAtIndex(currLine);
	auto cg = isActiveWindow() ? QPalette::Active : QPalette::Inactive;
	auto bkgColor = palette().color(cg, inst.GetAddress() == m_pcAddress ? QPalette::Highlight : QPalette::Base);
	auto txtColor = palette().color(cg, inst.GetAddress() == m_pcAddress ? QPalette::HighlightedText : QPalette::Text);
	p->setPen(QPen(txtColor));
	QRect rc{0, (currLine - scrollLine) * lineHeight(), viewport()->width(), lineHeight()};
	p->fillRect(rc, bkgColor);
	auto addressWidth = headerSectionSize(0);
	rc.setWidth(addressWidth);
	p->drawText(rc,0, QString("%1").arg(
		inst.GetAddress().GetLoadAddress(target),
		16, 16, QLatin1Char('0')));
	auto disasmWidth = headerSectionSize(1);
	rc.setLeft(rc.left() + rc.width());
	rc.setWidth(disasmWidth);
	p->drawText(rc, 0, QString(inst.GetMnemonic(target)) + " " + inst.GetOperands(target));
	auto commentWidth = headerSectionSize(2);
	rc.setLeft(rc.left() + rc.width());
	rc.setWidth(commentWidth);
	p->drawText(rc, 0, inst.GetComment(target));
}
void DisassemblyView::init()
{
	setColumns(QStringList() << "Address" << "Disassembly" << "Comment");
	AbstractTableView::init();
}

