//
// Created by xing on 2022/5/11.
//

#include "RegisterView.h"
#include "App.h"
#include "LLDBCore.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QApplication>
#include <QClipboard>


RegisterView::RegisterView()
	: QTreeWidget(nullptr)
{
	setColumnCount(2);
	setHeaderHidden(true);
	setFrameStyle(QFrame::NoFrame);

	connect(app(), &App::onStopState, this, [this]
	{
		auto const &process = core()->getProcess();
		setThreadFrame(process.GetSelectedThread(), 0);
	});
	connect(app(), &App::onThreadFrameChanged, this, [this](uint64_t tid, int index)
	{
		auto process = core()->getProcess();
		setThreadFrame(process.GetThreadByID(tid), index);
	});
}

void RegisterView::setThreadFrame(lldb::SBThread thread, int index)
{
	// TODO: process.GetAddressByteSize()
	clear();
	auto const &target = core()->getTarget();

	auto frame = thread.GetFrameAtIndex(index);

	auto regs = frame.GetRegisters();
	for (int i = 0; i < regs.GetSize(); ++i)
	{
		auto reg = regs.GetValueAtIndex(i);
		addValue(reg, nullptr);
	}

	expandAll();
}

void RegisterView::addValue(lldb::SBValue value, QTreeWidgetItem *parent)
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
		addTopLevelItem(item);
	}
}

void RegisterView::contextMenuEvent(QContextMenuEvent * event)
{
	auto item = currentItem();
	if (!item) return;

	auto txt = item->text(1);
	auto ok = false;
	auto value = txt.toULongLong(&ok, 16);
	if (!ok) return;

	QMenu menu;
	menu.addAction(tr("Copy"), this, [txt]
	{
		qApp->clipboard()->setText(txt);
	});

	{
		auto process = core()->getProcess();
		lldb::SBMemoryRegionInfo info;
		auto err = process.GetMemoryRegionInfo(value, info);

		if (err.Success() && info.IsReadable())
		{
			menu.addAction("View in memory dump", this, [value]
			{
				emit app()->gotoMemory(value);
			});
		}
	}

	menu.exec(event->globalPos());
}
