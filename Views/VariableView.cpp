#include "VariableView.h"
#include "App.h"
#include <QtWidgets>


VariableView::VariableView(QWidget *parent)
	: QWidget(parent)
{
	m_varTree = new QTreeWidget;
	m_varTree->setColumnCount(4);
	m_varTree->setHeaderLabels(QStringList() << tr("Name") << tr("Value") << tr("Type") << tr("Scope"));

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


const char* VariableView::convertValueType(lldb::ValueType t)
{
	switch (t)
	{
	case lldb::eValueTypeVariableGlobal:
		return "Global";
	case lldb::eValueTypeVariableStatic:
		return "Static";
	case lldb::eValueTypeVariableArgument:
		return "Argument";
	case lldb::eValueTypeVariableLocal:
		return "Local";
	case lldb::eValueTypeRegister:
		return "Register";
	case lldb::eValueTypeRegisterSet:
		return "RegisterSet";
	case lldb::eValueTypeConstResult:
		return "Const";
	case lldb::eValueTypeVariableThreadLocal:
		return "ThreadLocal";
	default:
		break;
	}

	return "None";
}

void VariableView::setItemsHide()
{
	for (auto i = 0; i < m_varTree->topLevelItemCount(); ++i)
	{
		auto item = m_varTree->topLevelItem(i);
		item->setHidden(true);
		item->setForeground(1, QBrush());
	}
}

void VariableView::removeHiddenItems()
{
	std::vector<QTreeWidgetItem*> itemsToRemove;
	for (auto i = 0; i < m_varTree->topLevelItemCount(); ++i)
	{
		auto item = m_varTree->topLevelItem(i);
		if (item->isHidden())
			itemsToRemove.emplace_back(item);
	}

	for (auto item : itemsToRemove)
	{
		delete item;
	}
}

void VariableView::refresh()
{
	auto vars = m_frame.GetVariables(true, true, true, false);
	if (!vars.IsValid())
	{
		app()->w(tr("get frame variavles failed"));
		return;
	}

	setItemsHide();

	for (auto i = 0; i < vars.GetSize(); ++i)
	{
		auto v = vars.GetValueAtIndex(i);
		addVariable(v);
	}

	removeHiddenItems();
}

void VariableView::addVariable(lldb::SBValue v)
{
	for (auto i = 0; i < m_varTree->topLevelItemCount(); ++i)
	{
		auto item = m_varTree->topLevelItem(i);
		if (item->text(0) == v.GetName() && item->text(3) == convertValueType(v.GetValueType()))
		{
			item->setHidden(false);
			if (item->text(1) != v.GetValue())
			{
				item->setText(1, v.GetValue());
				item->setForeground(1, Qt::red);
			}
			item->setText(2, v.GetDisplayTypeName());
			return;
		}
	}

	auto item = new QTreeWidgetItem;
	item->setTextAlignment(0, Qt::AlignLeft);
	item->setTextAlignment(1, Qt::AlignLeft);
	item->setTextAlignment(2, Qt::AlignLeft);
	item->setText(0, v.GetName());
	item->setText(1, v.GetValue());	// TODO: Ö§³Östd::string vectorµÈ
	item->setText(2, v.GetDisplayTypeName());
	item->setText(3, convertValueType(v.GetValueType()));
	item->setHidden(false);

	m_varTree->addTopLevelItem(item);
}
