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
//	clear();
	auto &target = App::get()->getDbgCore()->getTarget();

	auto frame = thread.GetFrameAtIndex(index);
	m_pcAddress = frame.GetPCAddress().GetLoadAddress(target);
//	auto funcName = frame.GetDisplayFunctionName();
//	if (funcName) setTitle(QString("反汇编-").append(funcName));
//	auto sc = frame.GetSymbolContext(lldb::eSymbolContextFunction | lldb::eSymbolContextSymbol);
//	if (sc.GetFunction())
//	{
//
//	}
//	else if (sc.GetSymbol())
//	{
//		frame.GetLineEntry()
//	}
//	else
//	{
//		frame.
//	}





	auto func = frame.GetFunction();
	lldb::SBInstructionList insts;
	if (func.IsValid())
	{
//		func.GetStartAddress()
		insts = func.GetInstructions(target, "intel");
	}
	else
	{
		auto &process = App::get()->getDbgCore()->getProcess();
		lldb::SBMemoryRegionInfo region_info;
		auto err = process.GetMemoryRegionInfo(frame.GetPC(), region_info);
		if (err.Fail())
		{
			// TODO: log
			return;
		}

		auto base = region_info.GetRegionBase();
		auto sz = region_info.GetRegionEnd() - base;
		std::vector<uint8_t> buffer(sz);

		lldb::SBError error;
		sz = process.ReadMemory(base, buffer.data(), sz, error);
		buffer.resize(sz);

		insts = target.GetInstructionsWithFlavor(base, "intel", buffer.data(), buffer.size());
	}

//	auto& process = App::get()->getDbgCore()->getProcess();
//	auto base = frame.GetSymbol().GetStartAddress().GetLoadAddress(target);
//	auto sz = frame.GetSymbol().GetEndAddress().GetLoadAddress(target) - base;
//	std::vector<uint8_t> buffer(sz);
//
//	lldb::SBError error;
//	sz = process.ReadMemory(base, buffer.data(), sz, error);
//	buffer.resize(sz);
//
//	auto insts = target.GetInstructionsWithFlavor(base, "intel", buffer.data(), buffer.size());


//	int scrollLine = 0;
//	for (int i = 0; i < insts.GetSize(); ++i)
//	{
//		auto inst = insts.GetInstructionAtIndex(i);
//		auto addr = inst.GetAddress();
//		if (addr == frame.GetPCAddress())
//		{
//			setTextBackgroundColor(Qt::blue);
//			scrollLine = i;
//		}
//		else setTextBackgroundColor(Qt::white);
//		append(QString("%1: %2 %3  ;%4\n")
//							   .arg(addr.GetLoadAddress(target))
//							   .arg(inst.GetMnemonic(target))
//							   .arg(inst.GetOperands(target))
//							   .arg(inst.GetComment(target)));
//	}

	for (int i = 0; i < insts.GetSize(); ++i)
	{
		auto inst = insts.GetInstructionAtIndex(i);
		auto address = inst.GetAddress().GetLoadAddress(target);
		if (address == m_pcAddress)
		{
			scrollToLine(i);
		}
		m_insts.emplace_back(Instruction{
			address,
			QString("%1 %2").arg(inst.GetMnemonic(target)).arg(inst.GetOperands(target)),
			QString(inst.GetComment(target))
		});
	}

//	auto cur = extCursor();
//	cur.setPosition(scrollLine);
//	setTextCursor(cur);

//	setPlainText(frame.Disassemble());

//	find("-> ");

	refresh();
}
int DisassemblyView::lineCount()
{
	return int(m_insts.size());
}

void DisassemblyView::drawLine(QPainter *p, int scrollLine, int currLine)
{
	auto const& inst = m_insts.at(currLine);
	auto cg = isActiveWindow()?QPalette::Active:QPalette::Inactive;
	auto bkgColor = palette().color(cg, inst.address == m_pcAddress? QPalette::Highlight: QPalette::Base);
	auto txtColor = palette().color(cg, inst.address == m_pcAddress? QPalette::HighlightedText: QPalette::Text);
	p->setPen(QPen(txtColor));
	QRect rc{0, (currLine - scrollLine) * lineHeight(), viewport()->width(), lineHeight()};
	p->fillRect(rc, bkgColor);
	auto addressWidth = headerSectionSize(0);
	rc.setWidth(addressWidth);
	p->drawText(rc, 0, QString("%1").arg(inst.address, 16, 16, QLatin1Char('0')));
	auto disasmWidth = headerSectionSize(1);
	rc.setLeft(rc.left() + rc.width());
	rc.setWidth(disasmWidth);
	p->drawText(rc, 0, inst.disassembly);
	auto commentWidth = headerSectionSize(2);
	rc.setLeft(rc.left() + rc.width());
	rc.setWidth(commentWidth);
	p->drawText(rc, 0, inst.comment);
}
void DisassemblyView::init()
{
	setColumns(QStringList() << "Address" << "Disassembly" << "Comment");
	AbstractTableView::init();
}

