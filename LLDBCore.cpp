//
// Created by w4264 on 2021/1/8.
//

#include "LLDBCore.h"
#include "App.h"


LLDBCore::LLDBCore(QString const& path, QString const& args)
	:m_path(path), m_args(args)
{
}

void LLDBCore::run()
{
	App::get()->logInfo(QString("LLDB: run '%1', args: '%2'").arg(m_path).arg(m_args));
	emit debugeeExited(-1);
}

bool LLDBCore::init()
{
//	auto err = lldb::SBDebugger::InitializeWithErrorHandling();
//	if (err.Fail())
//	{
//		return false;
//	}
	return true;
}
