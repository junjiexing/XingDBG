//
// Created by xing on 2022/5/9.
//

#include "DisassemblyView.h"
#include "App.h"
#include "Utils.h"
#include <QContextMenuEvent>
#include <QtWidgets>


DisassemblyView::DisassemblyView()
	: AbstractTableView(nullptr)
{
	setFrameStyle(QFrame::NoFrame);

	connect(app(), &App::onStopState, this, [this]
	{
		auto process = core()->getProcess();
		setThreadFrame(process.GetSelectedThread(), 0);
	});
	connect(app(), &App::onThreadFrameChanged, this, [this](uint64_t tid, int index)
	{
		auto process = core()->getProcess();
		setThreadFrame(process.GetThreadByID(tid), index);
	});

	auto bpAct = new QAction;
	bpAct->setShortcut(Qt::Key_F2);
	bpAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(bpAct, &QAction::triggered, this, [this]
		{
			auto ins = m_insts.GetInstructionAtIndex(selectedLine());
			if (!ins.IsValid())
			{
				app()->w(tr("set breakpoint failed"));
				return;
			}

			auto& target = core()->getTarget();
			auto oldBp = findBreakpointByAddress(ins.GetAddress().GetLoadAddress(target));
			if (oldBp)
			{
				target.BreakpointDelete(oldBp.GetID());
				app()->i("delete breakpoint");
				return;
			}

			auto addr = ins.GetAddress();
			auto br = target.BreakpointCreateBySBAddress(addr);// 这个参数为啥不是const引用？奇怪.
			if (!br.IsValid())
			{
				app()->w(tr("set breakpoint failed"));
				return;
			}

			app()->i(tr("set breakpoint successful"));
		});
	addAction(bpAct);
}

void DisassemblyView::setThreadFrame(lldb::SBThread thread, int index)
{
	m_insts.Clear();
	auto &target = core()->getTarget();
	auto frame = thread.GetFrameAtIndex(index);
	m_pcAddress = frame.GetPCAddress();

	auto sc = frame.GetSymbolContext(lldb::eSymbolContextFunction | lldb::eSymbolContextSymbol);
	if (!disasmSymbolContext(sc, target))
		disasmAddress(m_pcAddress.GetLoadAddress(target), target);

	updateDisasmShow(m_pcAddress);
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
	auto bkgColorRol = QPalette::Base;
	auto txtColorRol = QPalette::Text;
	if (inst.GetAddress() == m_pcAddress)
		bkgColorRol = QPalette::AlternateBase;
	else if (findBreakpointByAddress(inst.GetAddress().GetLoadAddress(target)))	//TODO: performance???
	{
		bkgColorRol = QPalette::Dark;	// TODO:改成QColor.
	}
	else if (currLine == selectedLine())
	{
		bkgColorRol = QPalette::Highlight;
		txtColorRol = QPalette::HighlightedText;
	}
	auto bkgColor = palette().color(cg, bkgColorRol);
	auto txtColor = palette().color(cg, txtColorRol);
	p->setPen(QPen(txtColor));
	QRect rc{0, (currLine - scrollLine) * lineHeight(), viewport()->width(), lineHeight()};
	p->fillRect(rc, bkgColor);
	auto addressWidth = headerSectionSize(0);
	rc.setWidth(addressWidth);
	p->drawText(rc, 0, Utils::hex(inst.GetAddress().GetLoadAddress(target)));
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

bool DisassemblyView::gotoAddress(uint64_t address)
{
	auto target = core()->getTarget();

	lldb::SBAddress sbAddress;
	sbAddress.SetLoadAddress(address, target);
	if (m_insts.IsValid() && m_insts.GetSize() != 0)
	{
		auto startAddress= m_insts.GetInstructionAtIndex(0).GetAddress().GetLoadAddress(target);
		auto endAddress = m_insts.GetInstructionAtIndex(m_insts.GetSize() - 1).GetAddress().GetLoadAddress(target);
		if (address >= startAddress && address <= endAddress)
		{
			updateDisasmShow(sbAddress);
			return true;
		}
	}


	auto sc = target.ResolveSymbolContextForAddress(
		sbAddress, lldb::eSymbolContextFunction | lldb::eSymbolContextSymbol);

	if (!disasmSymbolContext(sc, target) && !disasmAddress(m_pcAddress.GetLoadAddress(target), target))
		return false;

	updateDisasmShow(sbAddress);

	return true;
}

bool DisassemblyView::disasmSymbolContext(lldb::SBSymbolContext &ctx, lldb::SBTarget &target)
{
	auto func = ctx.GetFunction();
	if (func.IsValid())
	{
		m_insts = func.GetInstructions(target, "intel");
		return true;
	}
	auto sym = ctx.GetSymbol();
	if (sym.IsValid() && (sym.GetType() == lldb::eSymbolTypeAbsolute || sym.GetStartAddress().IsValid()))
	{
		m_insts = sym.GetInstructions(target, "intel");

		return true;
	}

	return false;
}

bool DisassemblyView::disasmAddress(uint64_t address, lldb::SBTarget &target)
{
	auto &process = core()->getProcess();
	lldb::SBMemoryRegionInfo region_info;
	auto err = process.GetMemoryRegionInfo(address, region_info);
	if (err.Fail())
	{
		app()->w(tr("Cannot find memory region info: ") + err.GetCString());
		return false;
	}

	// TODO: 更好的反汇编方式
//			auto base = region_info.GetRegionBase();
	auto base = address;
	auto sz = region_info.GetRegionEnd() - base;
	std::vector<uint8_t> buffer(sz);

	lldb::SBError error;
	sz = process.ReadMemory(base, buffer.data(), sz, error);
	if (error.Fail())
	{
		app()->w(tr("Cannot read process memory"));
		return false;
	}
	buffer.resize(sz);

	m_insts = target.GetInstructionsWithFlavor(base, "intel", buffer.data(), buffer.size());

	return true;
}

lldb::SBBreakpoint DisassemblyView::findBreakpointByAddress(lldb::addr_t addr)
{
	auto& target = core()->getTarget();

	for (int i = 0; i < target.GetNumBreakpoints(); ++i)
	{
		auto bp = target.GetBreakpointAtIndex(i);
		auto loc = bp.FindLocationByAddress(addr);
		if (loc.IsValid())
		{
			return bp;
		}
	}

	return {};
}

void DisassemblyView::updateDisasmShow(lldb::SBAddress const& address)
{
	int i = 0;
	for (; i != m_insts.GetSize(); ++i)
	{
		auto inst = m_insts.GetInstructionAtIndex(i);
		if (inst.GetAddress() == address)
			break;
	}

	updateScrollBar();
	refresh();

	if (i != m_insts.GetSize())
	{
		m_selectedLine = i;
		scrollToLine(i);
	}
	else
	{
		// TODO: 花指令 or 反汇编错误？
	}
}
void DisassemblyView::contextMenuEvent(QContextMenuEvent *event)
{
	//TODO: action放到构造函数.
	QMenu menu;
	menu.addAction(tr("Go to"), this, [this]
	{
		QDialog dlg(this);
		dlg.setWindowTitle(tr("Go to address"));
		auto edt = new QLineEdit;
		auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
		connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
		auto lay = new QFormLayout(&dlg);
		lay->addRow(tr("Address:"), edt);
		lay->addRow(btnBox);

		if (dlg.exec() != QDialog::Accepted) return;
		auto txt = edt->text();
		if (txt.isEmpty())
		{
			QMessageBox::information(this, tr("Tip"), tr("address cannot be empty"));
			return;
		}
		auto address = txt.toULongLong(nullptr, 16);
		if (address == 0)
		{
			QMessageBox::information(this, tr("Tip"), tr("invalid address"));
			return;
		}

		if (!gotoAddress(address))
			QMessageBox::warning(this, tr("Error"), tr("disasm failed"));
	});

	menu.exec(event->globalPos());
}

