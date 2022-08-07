//
// Created by xing on 2022/5/17.
//

#include "OutputView.h"
#include "App.h"


OutputView::OutputView()
	: QTextEdit(nullptr)
{
	setReadOnly(true);
	setFrameStyle(QFrame::NoFrame);
	connect(App::get(), &App::outputMsg, this, [this](QString const& msg, QColor const& color)
	{
		setTextColor(color);
		append(msg);
	});
}
