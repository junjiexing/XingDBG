//
// Created by w4264 on 2021/1/8.
//

#include "LLDBCore.h"

#include <utility>
#include "App.h"


LLDBCore::LLDBCore(QString  path, QString  args)
	:m_path(std::move(path)), m_args(std::move(args))
{
}

void LLDBCore::run()
{
	App::get()->logInfo(QString("LLDB: run '%1', args: '%2'").arg(m_path).arg(m_args));
	emit debugeeExited(-1);
}

bool LLDBCore::init()
{
	auto err = lldb::SBDebugger::InitializeWithErrorHandling();
	if (err.Fail())
	{
		App::get()->logError(QString("LLDBCore: 初始化lldb引擎失败: %1").arg(err.GetCString()));
		return false;
	}
	App::get()->logInfo(QString("LLDBCore: 初始化lldb引擎成功"));
	return true;
}
