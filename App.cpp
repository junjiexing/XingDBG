//
// Created by w4264 on 2021/1/8.
//

#include "App.h"

#include "LLDBCore.h"

App* App::get()
{
	static App app;

	return &app;
}

App::App()
    :lldbCore(std::make_unique<LLDBCore>())
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

LLDBCore *App::getDbgCore()
{
	return lldbCore.get();
}

