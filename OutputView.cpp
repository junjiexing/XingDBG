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

	connect(App::get(), &App::outputMsg, this, [=](QString const& msg, QColor const& color)
	{
		outEdt->setTextColor(color);
		outEdt->append(msg);
	});

	connect(cmdEdt, &QLineEdit::returnPressed, this, [=]
	{
		auto cmd = cmdEdt->text();
		cmdEdt->clear();
		auto& dbg = core()->getDebugger();
		auto cmdIntp = dbg.GetCommandInterpreter();
		lldb::SBCommandReturnObject ret;
		auto status = cmdIntp.HandleCommand(cmd.toLocal8Bit(), ret, false);
		// TODO: NoResult的返回结果也有可能有output
		if (status == lldb::eReturnStatusInvalid)
		{
			app()->w(tr("Invalid command:") + ret.GetError());
			return;
		}
		if (status == lldb::eReturnStatusFailed)
		{
			app()->w(tr("Run command failed:") + ret.GetError());
			return;
		}
		if (status == lldb::eReturnStatusSuccessContinuingNoResult)
		{
			app()->i(tr("Command is running"));
			return;
		}
		if (status == lldb::eReturnStatusSuccessFinishNoResult)
		{
			app()->i(tr("Command success:") + ret.GetOutput());
			return;
		}
		if (status == lldb::eReturnStatusSuccessContinuingResult)
		{
			app()->i(tr("Command is running:") + ret.GetOutput());
			return;
		}
		if (status == lldb::eReturnStatusSuccessFinishResult)
		{
			app()->i(tr("Command success:") + ret.GetOutput());
			return;
		}
	});

	auto vlay = new QVBoxLayout(this);
	vlay->addWidget(outEdt, 1);
	auto hlay = new QHBoxLayout;
	vlay->addLayout(hlay);
	hlay->addWidget(lldbLab);
	hlay->addWidget(cmdEdt, 1);
}
