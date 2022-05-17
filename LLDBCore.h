//
// Created by w4264 on 2021/1/8.
//

#pragma once
#include <QThread>
#include <memory>
#include <lldb/API/LLDB.h>

class LLDBCore : public QThread
{
	Q_OBJECT

public:
	explicit LLDBCore(QString path, QString args);
	~LLDBCore() override;

	static bool init();

	[[nodiscard]] lldb::SBProcess const&  getProcess() const {return m_process;}
	[[nodiscard]] lldb::SBProcess&  getProcess() {return m_process;}
	[[nodiscard]] lldb::SBTarget const& getTarget() const {return m_target;}
	[[nodiscard]] lldb::SBTarget& getTarget() {return m_target;}


protected:
	void run() override;

private:
	static std::unique_ptr<LLDBCore> instance;


	QString m_path;
	QString m_args;

	lldb::SBDebugger m_debugger;
	lldb::SBListener m_listener;
	lldb::SBTarget m_target;
	lldb::SBProcess m_process;
	lldb::SBEvent m_event;
};


