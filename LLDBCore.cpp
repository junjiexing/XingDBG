//
// Created by w4264 on 2021/1/8.
//

#include "LLDBCore.h"
#include "App.h"
#include <vector>

LLDBCore::LLDBCore(const lldb::SBListener& listener)
	: m_listener(listener)
{
}

void LLDBCore::run()
{
	auto exec = m_target.GetExecutable();
	app()->setExecutable(exec.GetFilename());
	app()->setProcessId(m_process.GetProcessID());
	emit app()->updateTitle();
	for (;;)
	{
		if (!m_listener.WaitForEvent(std::numeric_limits<uint32_t>::max(), m_event))
		{
			app()->w(tr("WaitForEvent failed, debug will stop."));
			break;
		}

		app()->setThreadId(m_process.GetSelectedThread().GetThreadID());

		const auto eventMask = m_event.GetType();

		if (lldb::SBProcess::EventIsProcessEvent(m_event))
		{
			app()->i(tr("on process debug event: %1").arg(eventMask));
			auto process = lldb::SBProcess::GetProcessFromEvent(m_event);

			if (eventMask & lldb::SBProcess::eBroadcastBitStateChanged)
			{
				auto state = lldb::SBProcess::GetStateFromEvent(m_event);
				if (state == lldb::StateType::eStateExited || state == lldb::StateType::eStateCrashed)
				{
					auto exitStatus = state == lldb::StateType::eStateCrashed ? -1 : m_process.GetExitStatus();
					app()->i(QString("LLDBCore: debugee exited：%1").arg(exitStatus));
					emit app()->debugeeExited(exitStatus);
					break;
				}
				if (state == lldb::StateType::eStateStopped)	//断点或异常
				{
					emit app()->onStopState();

					auto thread = m_process.GetSelectedThread();
					app()->i(tr("LLDBCore: paused，thread id：%1").arg(thread.GetThreadID()));
					continue;
				}
			}
			if (eventMask & lldb::SBProcess::eBroadcastBitSTDOUT)
			{
				char buffer[100];
				size_t n;
				QString str = "stdout: ";
				while ((n = m_process.GetSTDOUT(buffer, 100)) != 0)
					str += QString::fromLocal8Bit(buffer, qsizetype(n));
				app()->i(str);
			}
			if (eventMask & lldb::SBProcess::eBroadcastBitSTDERR)
			{
				char buffer[100];
				size_t n;
				QString str = "stderr: ";
				while ((n = m_process.GetSTDERR(buffer, 100)) != 0)
					str += QString::fromLocal8Bit(buffer, qsizetype(n));
				app()->i(str);
			}
			if (eventMask & lldb::SBProcess::eBroadcastBitInterrupt)
			{
				//			auto state = lldb::SBProcess::GetStateFromEvent(m_event);
				app()->w(tr("process interrupt //TODO"));
			}
		}
		else if (lldb::SBProcess::EventIsStructuredDataEvent(m_event))
		{
			app()->i(tr("on structured data event: %1").arg(eventMask));
		}
		else if (lldb::SBTarget::EventIsTargetEvent(m_event))
		{
			app()->i(tr("on target debug event: %1").arg(eventMask));
			// TODO: 相同的dll会走两遍，可能是windows版lldb的bug.
			if (eventMask & lldb::SBTarget::eBroadcastBitModulesLoaded)
			{
				QStringList moduleNames;
				for (int i = 0; i < lldb::SBTarget::GetNumModulesFromEvent(m_event); ++i)
				{
					auto m = lldb::SBTarget::GetModuleAtIndexFromEvent(i, m_event);
					moduleNames.append(m.GetFileSpec().GetFilename());
				}
				emit app()->onModulesChange();
				app()->i(tr("on module load: ") + moduleNames.join(";"));
			}
			if (eventMask & lldb::SBTarget::eBroadcastBitModulesUnloaded)
			{
				QStringList moduleNames;
				for (int i = 0; i < lldb::SBTarget::GetNumModulesFromEvent(m_event); ++i)
				{
					auto m = lldb::SBTarget::GetModuleAtIndexFromEvent(i, m_event);
					moduleNames.append(m.GetFileSpec().GetFilename());
				}
				emit app()->onModulesChange();
				app()->i(tr("on module unload: ") + moduleNames.join(";"));
			}
			if (eventMask & lldb::SBTarget::eBroadcastBitSymbolsLoaded)
			{
				app()->i(tr("on symbol loaded"));
			}
		}
		else if (lldb::SBBreakpoint::EventIsBreakpointEvent(m_event))
		{
			app()->i(tr("on breakpoint event: %1").arg(eventMask));
			//lldb::SBBreakpoint::GetBreakpointFromEvent(m_event)
			emit app()->onBreakpointChange();

		}
		else if (lldb::SBCommandInterpreter::EventIsCommandInterpreterEvent(m_event))
		{
			app()->i(tr("on command interpreter event: %1").arg(eventMask));
		}
		else if (lldb::SBThread::EventIsThreadEvent(m_event))
		{
			app()->i(tr("on thread event: %1").arg(eventMask));
		}
		else if (lldb::SBWatchpoint::EventIsWatchpointEvent(m_event))
		{
			app()->i(tr("on watchpoint event: %1").arg(eventMask));
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
	m_launchInfo.exePath = exePath;
	m_launchInfo.workingDir = workingDir;
	m_launchInfo.stdoutPath = stdoutPath;
	m_launchInfo.stderrPath = stderrPath;
	m_launchInfo.stdinPath = stdinPath;
	m_launchInfo.argList = argList;
	m_launchInfo.envList = envList;
	m_launchInfo.launchFlags = launchFlags;


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
	auto listener = lldb::SBListener();
	m_process = m_target.Launch(listener, args.data(), envs.data(),
		stdinPath.isEmpty() ? nullptr : stdinPath.toLocal8Bit(),
		stdoutPath.isEmpty() ? nullptr : stdoutPath.toLocal8Bit(),
		stderrPath.isEmpty() ? nullptr : stdoutPath.toLocal8Bit(),
		workingDir.toLocal8Bit(),
		launchFlags, false, error);

    if (error.Fail())
    {
        app()->w(QString("LLDBCore: launch executable failed: ") + error.GetCString());
        return false;
    }

    return true;
}

bool LLDBCore::launch(const LaunchInfo& launchInfo)
{
	return launch(
		launchInfo.exePath,
		launchInfo.workingDir,
		launchInfo.stdoutPath,
		launchInfo.stderrPath,
		launchInfo.stdinPath,
		launchInfo.argList,
		launchInfo.envList,
		launchInfo.launchFlags
	);
}

bool LLDBCore::attach(uint64_t pid)
{
	m_target = app()->getDebugger().CreateTarget(nullptr);
	lldb::SBError error;
	auto listener = lldb::SBListener();
	m_process = m_target.AttachToProcessWithID(listener, pid, error);
	if (error.Fail())
	{
		app()->w(tr("LLDBCore: attach to pid failed: ") + error.GetCString());
		return false;
	}

	return true;
}

