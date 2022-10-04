//
// Created by xing on 2022/5/11.
//

#include "ThreadView.h"
#include "App.h"
#include "LLDBCore.h"


ThreadView::ThreadView()
	: QTableWidget(nullptr)
{
	setColumnCount(3);
	setFrameStyle(QFrame::NoFrame);

	setHorizontalHeaderLabels(QStringList() << "id" << "pc" << "注释");
	setSelectionBehavior(QAbstractItemView::SelectRows);
	connect(app(), &App::onStopState, this, &ThreadView::refresh);

	connect(this, &QTableWidget::doubleClicked, this, [this](const QModelIndex &index)
	{
		auto it = item(index.row(), 0);
		if (!it) return;

		bool ok = false;
		auto tid = it->text().toULongLong(&ok);
		if (!ok) return;	// TODO: log
		emit app()->onThreadFrameChanged(tid, 0);
	});
}

static QTableWidgetItem* newItem(QString const& s)
{
	auto item = new QTableWidgetItem(s);
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
	return item;
}

void ThreadView::refresh()
{
	auto &process = core()->getProcess();
	auto num = process.GetNumThreads();
	setRowCount(int(num));
	for (int i = 0; i < num; ++i)
	{
		auto thread = process.GetThreadAtIndex(i);
		setItem(i, 0, newItem(QString::number(thread.GetThreadID())));
		setItem(i, 1, newItem(QStringLiteral("%1").arg(thread.GetSelectedFrame().GetPC(), 16, 16, QLatin1Char('0'))));
		lldb::SBStream description;
		thread.GetDescription(description);
		setItem(i, 2, newItem(description.GetData()));
	}
}
