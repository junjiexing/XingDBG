#include "BreakpointView.h"
#include <QtWidgets>
#include "LLDBCore.h"
#include "App.h"
#include "Utils.h"


BreakpointView::BreakpointView()
	: QWidget(nullptr)
{
	m_tree = new QTreeWidget;
	m_tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_tree->setColumnCount(10);
	m_tree->setHeaderLabels(QStringList() << tr("ID") << tr("Location")
		<< tr("Condition") << tr("Hit count") << tr("Ignore count") << tr("Auto continue")
		<< tr("Enabled") << tr("Hardware") << tr("Internal") << tr("Valid")
	);

	auto vlay = new QVBoxLayout(this);
	vlay->addWidget(m_tree);
}

void BreakpointView::refresh()
{
	auto target = core()->getTarget();
	m_tree->clear();
	for (int i = 0; i < target.GetNumBreakpoints(); ++i)
	{
		auto bp = target.GetBreakpointAtIndex(i);
		auto item = new QTreeWidgetItem;
		item->setText(0, QString::number(bp.GetID()));
		item->setText(2, bp.GetCondition());
		item->setText(3, QString::number(bp.GetHitCount()));
		item->setText(4, QString::number(bp.GetIgnoreCount()));
		item->setText(5, bp.GetAutoContinue() ? tr("True") : tr("False"));
		item->setText(6, bp.IsEnabled() ? tr("True") : tr("False"));
		item->setText(7, bp.IsHardware() ? tr("True") : tr("False"));
		item->setText(8, bp.IsInternal() ? tr("True") : tr("False"));
		item->setText(9, bp.IsValid() ? tr("True") : tr("False"));

		auto locNum = bp.GetNumLocations();
		if (locNum == 1)
		{
			auto loc = bp.GetLocationAtIndex(0);

			item->setText(1, Utils::hex(loc.GetLoadAddress()) + "(" + loc.GetAddress().GetSymbol().GetName() + ")");
			// XXX: 只有一个Location的时候是否需要把其他列也改成该location的值???
		}
		else if (locNum > 1)
		{
			for (int i = 0; i < locNum; ++i)
			{
				auto loc = bp.GetLocationAtIndex(i);
				auto childItem = new QTreeWidgetItem(item);
				childItem->setText(0, QString::number(loc.GetID()));
				childItem->setText(1, Utils::hex(loc.GetLoadAddress()));
				childItem->setText(2, loc.GetCondition());
				childItem->setText(3, QString::number(loc.GetHitCount()));
				childItem->setText(4, QString::number(loc.GetIgnoreCount()));
				childItem->setText(5, loc.GetAutoContinue() ? tr("True") : tr("False"));
				childItem->setText(6, loc.IsEnabled() ? tr("True") : tr("False"));
				childItem->setText(9, loc.IsValid() ? tr("True") : tr("False"));
			}
		}

		m_tree->addTopLevelItem(item);
	}
}

void BreakpointView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu;
	menu.addAction(tr("Refresh"), this, &BreakpointView::refresh);

	//TODO: 添加断点菜单项.

	auto currItem = m_tree->currentItem();
	if (currItem)
	{
		auto parentItem = currItem->parent();
		if (parentItem)
		{
			// 有parent，是location
			auto target = core()->getTarget();
			auto bpId = parentItem->text(0).toInt();
			auto bp = target.FindBreakpointByID(bpId);
			if (!bp)
				app()->w(tr("invalid breakpoint id: %1").arg(bpId));
			else
			{
				auto locId = currItem->text(0).toInt();
				auto loc = bp.FindLocationByID(locId);
				if (!loc)
					app()->w(tr("invalid location id: %1").arg(locId));
				else
				{
					if (loc.IsEnabled())
						menu.addAction(tr("Disable"), [loc]() mutable {loc.SetEnabled(false);});
					else
						menu.addAction(tr("Enable"), [loc]() mutable {loc.SetEnabled(true);});
				}
			}
		}
		else
		{
			//没有parent，是breakpoint
			auto target = core()->getTarget();
			auto bpId = currItem->text(0).toInt();
			auto bp = target.FindBreakpointByID(bpId);
			if (!bp)
				app()->w(tr("invalid breakpoint id: %1").arg(bpId));
			else
			{
				if (bp.IsEnabled())
					menu.addAction(tr("Disable"), [bp]() mutable {bp.SetEnabled(false);});
				else
					menu.addAction(tr("Enable"), [bp]() mutable {bp.SetEnabled(true);});
			}
			menu.addAction(tr("Delete"), [bpId, this]
				{
					if (!core()->getTarget().BreakpointDelete(bpId))
						app()->w(tr("Delete breakpoint failed, id:").arg(bpId));
					refresh();
				});
		}
	}

	menu.exec(event->globalPos());
}
