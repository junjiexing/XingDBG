//
// Created by w4264 on 2021/1/8.
//

#pragma once
#include <QThread>
#include <memory>
#include <lldb/API/LLDB.h>
#include <vector>

struct LaunchInfo
{
	QString exePath;
	QString workingDir;
	QString stdoutPath;
	QString stderrPath;
	QString stdinPath;
	QStringList argList;
	QStringList envList;
	uint32_t launchFlags;
};


class LLDBCore: public QThread
{
Q_OBJECT

public:
	explicit LLDBCore(const lldb::SBListener& listener);
	~LLDBCore() override;

	static bool init();

	std::vector<std::pair<QString, QString>> platforms();

	bool launch(QString const &exePath, QString const &workingDir, QString const &stdoutPath,
				QString const &stderrPath, QString const &stdinPath, QStringList const &argList,
				QStringList const &envList, uint32_t launchFlags);

	bool launch(const LaunchInfo& launchInfo);

	bool attach(uint64_t pid);

	LaunchInfo const& getLaunchInfo() const { return m_launchInfo; }
	lldb::SBProcess const &getProcess() const { return m_process; }
	lldb::SBProcess &getProcess() { return m_process; }
	lldb::SBTarget const &getTarget() const { return m_target; }
	lldb::SBTarget &getTarget() { return m_target; }


protected:
	void run() override;

private:
	static std::unique_ptr<LLDBCore> instance;

	LaunchInfo m_launchInfo;

	lldb::SBListener m_listener;
	lldb::SBTarget m_target;
	lldb::SBProcess m_process;
	lldb::SBEvent m_event;
};


