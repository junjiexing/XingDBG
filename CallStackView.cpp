//
// Created by xing on 2022/5/11.
//

#include "CallStackView.h"
#include "App.h"

CallStackView::CallStackView()
	:KDDockWidgets::DockWidget("调用堆栈"),
	m_listWidget(new QListWidget)
{
	setWidget(m_listWidget);

	connect(App::get(), &App::onStopState, this, [this]
	{
		auto process = App::get()->getDbgCore()->getProcess();
		setThread(process.GetSelectedThread());
	});
	connect(App::get(), &App::onThreadFrameChanged, this, [this](uint64_t tid, int)
	{
		if (sender() == this) return;

		auto process = App::get()->getDbgCore()->getProcess();
		setThread(process.GetThreadByID(tid));
	});
	connect(m_listWidget, &QListWidget::doubleClicked, this, [this](const QModelIndex &index)
	{
		emit App::get()->onThreadFrameChanged(m_tid, index.row());
	});
}

void CallStackView::setThread(lldb::SBThread thread)
{
	m_tid = thread.GetThreadID();

	m_listWidget->clear();

	for (int i = 0; i < thread.GetNumFrames(); ++i)
	{
		auto frame = thread.GetFrameAtIndex(i);
		lldb::SBStream description;
		frame.GetDescription(description);
		m_listWidget->addItem(description.GetData());
	}
}
