//
// Created by w4264 on 2021/1/8.
//

#include "App.h"

#include <memory>

#include "LLDBCore.h"

App* App::get()
{
	static App app;

	return &app;
}

App::App()
{

}

App::~App()
{

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


