//
// Created by xing on 2022/5/9.
//

#include "DisassemblyView.h"
#include "App.h"
#include "LLDBCore.h"

DisassemblyView::DisassemblyView()
	: KDDockWidgets::DockWidget("反汇编"),
	m_textEdit(new QTextEdit(this))
{
	m_textEdit->setReadOnly(true);
	setWidget(m_textEdit);
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
//	m_textEdit->clear();
	auto &target = App::get()->getDbgCore()->getTarget();

	auto frame = thread.GetFrameAtIndex(index);
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





//	auto func = frame.GetFunction();
//	lldb::SBInstructionList insts;
//	if (func.IsValid())
//	{
////		func.GetStartAddress()
//		insts = func.GetInstructions(target, "intel");
//	}
//	else
//	{
//		auto& process = App::get()->getDbgCore()->getProcess();
//		lldb::SBMemoryRegionInfo region_info;
//		auto err = process.GetMemoryRegionInfo(frame.GetPC(), region_info);
//		if (err.Fail())
//		{
//			// TODO: log
//			return;
//		}
//
//		auto base = region_info.GetRegionBase();
//		auto sz = region_info.GetRegionEnd() - base;
//		std::vector<uint8_t> buffer(sz);
//
//		lldb::SBError error;
//		sz = process.ReadMemory(base, buffer.data(), sz, error);
//		buffer.resize(sz);
//
//		insts = target.GetInstructionsWithFlavor(base, "intel", buffer.data(), buffer.size());
//	}

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
//
//
//	int scrollLine = 0;
//	for (int i = 0; i < insts.GetSize(); ++i)
//	{
//		auto inst = insts.GetInstructionAtIndex(i);
//		auto addr = inst.GetAddress();
//		if (addr == frame.GetPCAddress())
//		{
//			m_textEdit->setTextBackgroundColor(Qt::blue);
//			scrollLine = i;
//		}
//		else m_textEdit->setTextBackgroundColor(Qt::white);
//		m_textEdit->append(QString("%1: %2 %3  ;%4\n")
//							   .arg(addr.GetLoadAddress(target))
//							   .arg(inst.GetMnemonic(target))
//							   .arg(inst.GetOperands(target))
//							   .arg(inst.GetComment(target)));
//	}
//
//	auto cur = m_textEdit->textCursor();
//	cur.setPosition(scrollLine);
//	m_textEdit->setTextCursor(cur);

	m_textEdit->setPlainText(frame.Disassemble());

	m_textEdit->find("-> ");
}

