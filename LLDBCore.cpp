//
// Created by w4264 on 2021/1/8.
//

#include "LLDBCore.h"
#include "App.h"
#include <vector>


LLDBCore::LLDBCore()
{
}

static void log(const char *msg, void *)
{
	App::get()->logInfo(QString("LLDBCore: log: %1").arg(msg));
}

void LLDBCore::run()
{
	for (;;)
	{
		if (!m_listener.WaitForEvent(std::numeric_limits<uint32_t>::max(), m_event))
		{
			//TODO: log
			return;
		}

//		std::cout << "event: " << m_event.GetType() << std::endl;

		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitStateChanged) > 0)
		{
			auto state = lldb::SBProcess::GetStateFromEvent(m_event);
//			std::cout << "state: " << state <<std::endl;

			if(state == lldb::StateType::eStateExited || state == lldb::StateType::eStateCrashed)
			{
				auto exit_status = state == lldb::StateType::eStateCrashed ? -1 : m_process.GetExitStatus();
				App::get()->logInfo(QString("LLDBCore: 被调试进程已退出，代码：%1").arg(exit_status));
//				std::cout << "exit status: " << exit_status << std::endl;
				return;
			}
			if (state == lldb::StateType::eStateStopped)	//断点或异常
			{
				emit App::get()->onStopState();

				auto thread = m_process.GetSelectedThread();
//				std::cout << "selected thread:" << thread.GetThreadID() << std::endl;
//				std::cout << "num frames: " << thread.GetNumFrames() << std::endl;
				//process.Continue();
				App::get()->logInfo(QString("LLDBCore: 目标中断，线程id：%1").arg(thread.GetThreadID()));
//				process.Continue();
//				thread.StepOver();

				continue;
			}
		}

		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitSTDOUT) > 0)
		{
			char buffer[100];
			size_t n;
			std::string str = "stdout: ";
			while((n = m_process.GetSTDOUT(buffer, 100)) != 0)
				str += std::string(buffer, n);

//			std::cout << str << std::endl;
		}
		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitSTDERR) > 0)
		{
			char buffer[100];
			size_t n;
			std::string str = "stderr: ";
			while((n = m_process.GetSTDERR(buffer, 100)) != 0)
				str += std::string(buffer, n);

//			std::cout << str << std::endl;
		}
		if((m_event.GetType() & lldb::SBProcess::eBroadcastBitInterrupt) > 0)
		{
			auto state = lldb::SBProcess::GetStateFromEvent(m_event);
//			std::cout << "eBroadcastBitInterrupt:" << state << std::endl;
		}
	}

//	App::get()->logInfo(QString("LLDB: run '%1', args: '%2'").arg(m_path).arg(m_args));
//	emit debugeeExited(-1);
}

bool LLDBCore::init()
{
	auto err = lldb::SBDebugger::InitializeWithErrorHandling();
	if (err.Fail())
	{
		App::get()->logError(QString("LLDBCore: init lldb failed: %1").arg(err.GetCString()));
		return false;
	}
	App::get()->logInfo(QString("LLDBCore: init lldb success."));
	return true;
}
LLDBCore::~LLDBCore()
{
	m_process.Kill();

	wait();
	m_process.Destroy();
	lldb::SBDebugger::Destroy(m_debugger);
}

bool LLDBCore::launch(
        QString const& exePath, QString const& workingDir, QString const& stdoutPath,
        QString const& stderrPath, QString const& stdinPath, QStringList const& argList,
        QStringList const& envList, uint32_t launchFlags)
{
    m_debugger = lldb::SBDebugger::Create(true, &log, nullptr);
    m_listener = lldb::SBListener("lldb_local");
    m_target = m_debugger.CreateTarget(exePath.toLocal8Bit());
//    auto bp = m_target.BreakpointCreateByName("main");
//    bp.SetEnabled(true);
    lldb::SBError error;
    std::vector<const char*> args;
    for (auto const& s : argList)
    {
        args.emplace_back(s.toLocal8Bit());
    }
    args.emplace_back(nullptr);
    std::vector<const char*> envs;
    for (auto const& s : envList)
    {
        envs.emplace_back(s.toLocal8Bit());
    }
    envs.emplace_back(nullptr);
    m_process = m_target.Launch(m_listener, args.data(), envs.data(),
                                stdinPath.isEmpty()? nullptr : stdinPath.toLocal8Bit(),
                                stdoutPath.isEmpty()? nullptr : stdoutPath.toLocal8Bit(),
                                stderrPath.isEmpty()? nullptr : stdoutPath.toLocal8Bit(),
                                workingDir.toLocal8Bit(),
                                launchFlags, false, error);

    if (error.Fail())
    {
        App::get()->logWarn(QString("LLDBCore: launch executable failed: %1").arg(error.GetCString()));
        return false;
    }

    return true;
}

