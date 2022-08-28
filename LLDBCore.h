//
// Created by w4264 on 2021/1/8.
//

#pragma once
#include <QThread>
#include <memory>
#include <lldb/API/LLDB.h>
#include <vector>

class LLDBCore: public QThread
{
Q_OBJECT

public:
	explicit LLDBCore();
	~LLDBCore() override;

	static bool init();

	std::vector<std::pair<QString, QString>> platforms();

	bool launch(QString const &exePath, QString const &workingDir, QString const &stdoutPath,
				QString const &stderrPath, QString const &stdinPath, QStringList const &argList,
				QStringList const &envList, uint32_t launchFlags);

	bool attach(uint64_t pid);

	bool stop();

	lldb::SBProcess const &getProcess() const { return m_process; }
	lldb::SBProcess &getProcess() { return m_process; }
	lldb::SBTarget const &getTarget() const { return m_target; }
	lldb::SBTarget &getTarget() { return m_target; }


protected:
	void run() override;

private:
	static std::unique_ptr<LLDBCore> instance;

	lldb::SBListener m_listener;
	lldb::SBTarget m_target;
	lldb::SBProcess m_process;
	lldb::SBEvent m_event;
};


