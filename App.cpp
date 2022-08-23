//
// Created by w4264 on 2021/1/8.
//

#include "App.h"
#include <memory>
#include "LLDBCore.h"

static void log(const char *msg, void *)
{
	app()->i(QString("LLDB log: %1").arg(msg));
}

App* App::get()
{
	static App app;

	return &app;
}

App::App()
	: m_debugger(lldb::SBDebugger::Create(true, &log, nullptr))
{
}

App::~App()
{
	lldb::SBDebugger::Destroy(m_debugger);
}

void App::logError(const QString& msg)
{
	emit outputMsg(msg, Qt::red);
}

void App::logWarn(const QString& msg)
{
	emit outputMsg(msg, Qt::darkYellow);
}

void App::logInfo(const QString& msg)
{
	emit outputMsg(msg, Qt::black);
}

void App::resetCore()
{
	lldbCore = std::make_unique<LLDBCore>();
}

LLDBCore *App::getDbgCore()
{
	return lldbCore.get();
}


