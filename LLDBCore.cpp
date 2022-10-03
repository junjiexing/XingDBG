//
// Created by w4264 on 2021/1/8.
//

#include "LLDBCore.h"
#include "App.h"
#include <vector>

LLDBCore::LLDBCore()
{
}

void LLDBCore::run()
{
	for (;;)
	{
		if (!m_listener.WaitForEvent(std::numeric_limits<uint32_t>::max(), m_event))
		{
			app()->w(tr("WaitForEvent failed, debug will stop."));
			break;
		}

		app()->i(tr("on debug event:") + lldb::SBEvent::GetCStringFromEvent(m_event));

		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitStateChanged) > 0)
		{
			auto state = lldb::SBProcess::GetStateFromEvent(m_event);
			if(state == lldb::StateType::eStateExited || state == lldb::StateType::eStateCrashed)
			{
				auto exit_status = state == lldb::StateType::eStateCrashed ? -1 : m_process.GetExitStatus();
				app()->i(QString("LLDBCore: 被调试进程已退出，代码：%1").arg(exit_status));
				emit app()->debugeeExited(exit_status);
				break;
			}
			if (state == lldb::StateType::eStateStopped)	//断点或异常
			{
				emit app()->onStopState();

				auto thread = m_process.GetSelectedThread();
				app()->i(tr("LLDBCore: 目标中断，线程id：%1").arg(thread.GetThreadID()));
				continue;
			}
		}

		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitSTDOUT) > 0)
		{
			char buffer[100];
			size_t n;
			QString str = "stdout: ";
			while((n = m_process.GetSTDOUT(buffer, 100)) != 0)
				str += QString::fromLocal8Bit(buffer, qsizetype(n));
			app()->i(str);
		}
		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitSTDERR) > 0)
		{
			char buffer[100];
			size_t n;
			QString str = "stderr: ";
			while((n = m_process.GetSTDERR(buffer, 100)) != 0)
				str += QString::fromLocal8Bit(buffer, qsizetype(n));
			app()->i(str);
		}
		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitInterrupt) > 0)
		{
//			auto state = lldb::SBProcess::GetStateFromEvent(m_event);
			app()->w(tr("process interrupt //TODO"));
		}
	}

	app()->i(tr("Debug finished"));
}

bool LLDBCore::init()
{
	auto err = lldb::SBDebugger::InitializeWithErrorHandling();
	if (err.Fail())
	{
		app()->e(QString("LLDBCore: init lldb failed: %1").arg(err.GetCString()));
		return false;
	}
	app()->i(QString("LLDBCore: init lldb success."));
	return true;
}

LLDBCore::~LLDBCore()
{
	if (m_process.IsValid())
	{
		auto err = m_process.Kill();
		if (err.Fail())
		{
			app()->w(tr("kill process failed:") + err.GetCString());
			terminate();
		}
	}
	wait();
	app()->getDebugger().DeleteTarget(m_target);
}

std::vector<std::pair<QString, QString>> LLDBCore::platforms()
{
	std::vector<std::pair<QString, QString>> vec;
	char buffer[1000];	// shit
	for (int i = 0; ; ++i)
	{
		auto p = app()->getDebugger().GetAvailablePlatformInfoAtIndex(i);
		if (!p) return vec;

		p.GetValueForKey("name").GetStringValue(buffer, 1000);
		QString name = buffer;
		p.GetValueForKey("description").GetStringValue(buffer, 1000);
		QString description = buffer;

		vec.emplace_back(std::pair<QString, QString>{name, description});
	}
}

bool LLDBCore::launch(
        QString const& exePath, QString const& workingDir, QString const& stdoutPath,
        QString const& stderrPath, QString const& stdinPath, QStringList const& argList,
        QStringList const& envList, uint32_t launchFlags)
{
	lldb::SBError error;
    m_target = app()->getDebugger().CreateTarget(exePath.toLocal8Bit(), nullptr, nullptr, true, error);
	if (error.Fail())
	{
		app()->w(tr("Create target failed: ") + error.GetCString());
		return false;
	}
    std::vector<const char*> args;
    for (auto const& s : argList)
        args.emplace_back(s.toLocal8Bit());
    args.emplace_back(nullptr);
    std::vector<const char*> envs;
    for (auto const& s : envList)
        envs.emplace_back(s.toLocal8Bit());
    envs.emplace_back(nullptr);
	m_listener = lldb::SBListener("lldb_local");
    m_process = m_target.Launch(m_listener, args.data(), envs.data(),
                                stdinPath.isEmpty()? nullptr : stdinPath.toLocal8Bit(),
                                stdoutPath.isEmpty()? nullptr : stdoutPath.toLocal8Bit(),
                                stderrPath.isEmpty()? nullptr : stdoutPath.toLocal8Bit(),
                                workingDir.toLocal8Bit(),
                                launchFlags, false, error);

    if (error.Fail())
    {
        app()->w(QString("LLDBCore: launch executable failed: ") + error.GetCString());
        return false;
    }

    return true;
}

bool LLDBCore::attach(uint64_t pid)
{
	m_target = app()->getDebugger().CreateTarget(nullptr);
	lldb::SBError error;
	m_listener = lldb::SBListener("lldb_local");
	m_process = m_target.AttachToProcessWithID(m_listener, pid, error);
	if (error.Fail())
	{
		app()->w(tr("LLDBCore: attach to pid failed: ") + error.GetCString());
		return false;
	}

	return true;
}

