//
// Created by xing on 2022/5/11.
//

#include "ThreadView.h"
#include "App.h"
#include "LLDBCore.h"


ThreadView::ThreadView()
	:KDDockWidgets::DockWidget("线程"),
	m_tableWidget(new QTableWidget(this))
{
	m_tableWidget->setColumnCount(3);
	m_tableWidget->setHorizontalHeaderLabels(QStringList() << "id" << "pc" << "注释");
	m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	setWidget(m_tableWidget);
	connect(App::get(), &App::onStopState, this, &ThreadView::refresh);

	connect(m_tableWidget, &QTableWidget::doubleClicked, this, [this](const QModelIndex &index)
	{
		auto item = m_tableWidget->item(index.row(), 0);
		if (!item) return;

		bool ok = false;
		auto tid = item->text().toULongLong(&ok);
		if (!ok) return;	// TODO: log
		emit App::get()->onThreadFrameChanged(tid, 0);
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
	auto &process = App::get()->getDbgCore()->getProcess();
	auto num = process.GetNumThreads();
	m_tableWidget->setRowCount(int(num));
	for (int i = 0; i < num; ++i)
	{
		auto thread = process.GetThreadAtIndex(i);
		m_tableWidget->setItem(i, 0, newItem(QString::number(thread.GetThreadID())));
		m_tableWidget->setItem(i, 1, newItem(QStringLiteral("%1").arg(thread.GetSelectedFrame().GetPC(), 16, 16, QLatin1Char('0'))));
		lldb::SBStream description;
		thread.GetDescription(description);
		m_tableWidget->setItem(i, 2, newItem(description.GetData()));
	}
}
