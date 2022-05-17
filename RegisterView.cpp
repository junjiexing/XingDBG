//
// Created by xing on 2022/5/11.
//

#include "RegisterView.h"
#include "App.h"
#include "LLDBCore.h"


RegisterView::RegisterView()
	:KDDockWidgets::DockWidget("寄存器"),
	m_treeWidget(new QTreeWidget(this))
{
	m_treeWidget->setColumnCount(2);
	m_treeWidget->setHeaderHidden(true);
	setWidget(m_treeWidget);
	connect(App::get(), &App::onStopState, this, [this]
	{
		auto const &process = App::get()->getDbgCore()->getProcess();
		setThreadFrame(process.GetSelectedThread(), 0);
	});
	connect(App::get(), &App::onThreadFrameChanged, this, [this](uint64_t tid, int index)
	{
		auto process = App::get()->getDbgCore()->getProcess();
		setThreadFrame(process.GetThreadByID(tid), index);
	});
}

void RegisterView::setThreadFrame(lldb::SBThread thread, int index)
{
	// TODO: process.GetAddressByteSize()
	m_treeWidget->clear();
	auto const &target = App::get()->getDbgCore()->getTarget();

	auto frame = thread.GetFrameAtIndex(index);

	auto regs = frame.GetRegisters();
	for (int i = 0; i < regs.GetSize(); ++i)
	{
		auto reg = regs.GetValueAtIndex(i);
		addValue(reg, nullptr);
	}

	m_treeWidget->expandAll();
}
void RegisterView::addValue(lldb::SBValue value, QTreeWidgetItem* parent)
{
	auto item = new QTreeWidgetItem(parent);
	item->setTextAlignment(0, Qt::AlignLeft);
	item->setTextAlignment(1, Qt::AlignLeft);
	item->setText(0, value.GetName());
	if (value.GetNumChildren() == 0)
	{
		lldb::SBError err;
		item->setText(1, QStringLiteral("%1").arg(value.GetData().GetUnsignedInt64(err, 0), 16, 16, QLatin1Char('0')));
	}
	else
	{
		for (int i = 0; i < value.GetNumChildren(); ++i)
		{
			auto child = value.GetChildAtIndex(i);
			addValue(child, item);
		}
	}

	if (!parent)
	{
		m_treeWidget->addTopLevelItem(item);
	}
}
