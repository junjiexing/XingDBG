#include "VariableView.h"
#include "App.h"
#include <QtWidgets>


VariableView::VariableView(QWidget *parent)
	: QWidget(parent)
{
	m_varTree = new QTreeWidget;
	m_varTree->setColumnCount(3);

	connect(app(), &App::onStopState, this, [this]
	{
		auto process = core()->getProcess();
		auto thread = process.GetSelectedThread();
		m_frame = thread.GetFrameAtIndex(0);
		refresh();
	});
	connect(app(), &App::onThreadFrameChanged, this, [this](uint64_t tid, int index)
	{
		auto process = core()->getProcess();
		auto thread = process.GetThreadByID(tid);
		m_frame = thread.GetFrameAtIndex(index);
		refresh();
	});

	auto lay = new QVBoxLayout(this);
	lay->setSpacing(0);
	lay->addWidget(m_varTree);
}

VariableView::~VariableView()
{

}


QTreeWidgetItem* VariableView::newParentItem(QString const& name)
{
	auto item = new QTreeWidgetItem(m_varTree);
	item->setTextAlignment(0, Qt::AlignLeft);
	item->setTextAlignment(1, Qt::AlignLeft);
	item->setTextAlignment(2, Qt::AlignLeft);
	item->setText(0, name);

	return item;
}


void VariableView::refresh()
{
	auto vars = m_frame.GetVariables(true, true, true, false);
	if (!vars.IsValid())
	{
		app()->w(tr("get frame variavles failed"));
		return;
	}

	auto aItem = newParentItem("Argument:");
	auto lItem = newParentItem("Local:");
	auto sItem = newParentItem("Static:");
	auto gItem = newParentItem("Global:");
	auto cItem = newParentItem("Const:");
	auto tItem = newParentItem("Thread local:");

	for (auto i = 0; i < vars.GetSize(); ++i)
	{
		auto v = vars.GetValueAtIndex(i);
		switch (v.GetValueType())
		{
		case lldb::eValueTypeVariableGlobal:
			addVariable(gItem, v);
			break;
		case lldb::eValueTypeVariableStatic:
			addVariable(sItem, v);
			break;
		case lldb::eValueTypeVariableArgument:
			addVariable(aItem, v);
			break;
		case lldb::eValueTypeVariableLocal:
			addVariable(lItem, v);
			break;
		case lldb::eValueTypeConstResult:
			addVariable(cItem, v);
			break;
		case lldb::eValueTypeVariableThreadLocal:
			addVariable(tItem, v);
			break;
		default:
			break;
		}
	}
}

void VariableView::addVariable(QTreeWidgetItem* parent, lldb::SBValue v)
{
	auto item = new QTreeWidgetItem(parent);
	item->setTextAlignment(0, Qt::AlignLeft);
	item->setTextAlignment(1, Qt::AlignLeft);
	item->setTextAlignment(2, Qt::AlignLeft);
	item->setText(0, v.GetName());
	item->setText(1, v.GetDisplayTypeName());
	item->setText(2, v.GetValue());
}
