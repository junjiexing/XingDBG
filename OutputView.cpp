//
// Created by xing on 2022/5/17.
//

#include "OutputView.h"
#include "App.h"
#include "LLDBCore.h"
#include <QtWidgets>


OutputView::OutputView()
	: QWidget(nullptr)
{
	auto outEdt = new QTextEdit;
	outEdt->setReadOnly(true);
	outEdt->setFrameStyle(QFrame::NoFrame);

	auto lldbLab = new QLabel("LLDB");
	auto cmdEdt = new QLineEdit;

	connect(app(), &App::outputMsg, this, [=](QString const& msg, QColor const& color)
	{
		outEdt->setTextColor(color);
		outEdt->append(msg);
	});

	connect(cmdEdt, &QLineEdit::returnPressed, this, [=]
	{
		auto cmd = cmdEdt->text();
		cmdEdt->clear();
		auto& dbg = app()->getDebugger();
		auto cmdIntp = dbg.GetCommandInterpreter();
		lldb::SBCommandReturnObject ret;
		auto status = cmdIntp.HandleCommand(cmd.toLocal8Bit(), ret, false);
		switch (status)
		{
		case lldb::eReturnStatusInvalid:
			app()->w(tr("Invalid command"));
			break;
		case lldb::eReturnStatusFailed:
			app()->w(tr("Command failed"));
			break;
		case lldb::eReturnStatusSuccessContinuingNoResult:
			app()->i(tr("Command is running"));
			break;
		case lldb::eReturnStatusSuccessFinishNoResult:
			app()->i(tr("Command success"));
			break;
		case lldb::eReturnStatusSuccessContinuingResult:
			app()->i(tr("Command is running"));
			break;
		case lldb::eReturnStatusSuccessFinishResult:
			app()->i(tr("Command success"));
			break;
		case lldb::eReturnStatusStarted:
			app()->i(tr("Command started"));
			break;
		case lldb::eReturnStatusQuit:
			app()->i(tr("Command quit"));
			break;
		}

		if (ret.GetOutputSize() != 0)
			app()->i(ret.GetOutput());
		if (ret.GetErrorSize() != 0)
			app()->w(ret.GetError());
	});

	auto vlay = new QVBoxLayout(this);
	vlay->addWidget(outEdt, 1);
	auto hlay = new QHBoxLayout;
	vlay->addLayout(hlay);
	hlay->addWidget(lldbLab);
	hlay->addWidget(cmdEdt, 1);
}
