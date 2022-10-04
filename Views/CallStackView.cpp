//
// Created by xing on 2022/5/11.
//

#include "CallStackView.h"
#include "App.h"

CallStackView::CallStackView()
	: QListWidget(nullptr)
{
	setFrameStyle(QFrame::NoFrame);

	connect(app(), &App::onStopState, this, [this]
	{
		auto process = core()->getProcess();
		setThread(process.GetSelectedThread());
	});
	connect(app(), &App::onThreadFrameChanged, this, [this](uint64_t tid, int)
	{
		if (sender() == this) return;

		auto process = core()->getProcess();
		setThread(process.GetThreadByID(tid));
	});
	connect(this, &QListWidget::doubleClicked, this, [this](const QModelIndex &index)
	{
		emit app()->onThreadFrameChanged(m_tid, index.row());
	});
}

void CallStackView::setThread(lldb::SBThread thread)
{
	m_tid = thread.GetThreadID();

	clear();

	for (int i = 0; i < thread.GetNumFrames(); ++i)
	{
		auto frame = thread.GetFrameAtIndex(i);
		lldb::SBStream description;
		frame.GetDescription(description);
		addItem(description.GetData());
	}
}
