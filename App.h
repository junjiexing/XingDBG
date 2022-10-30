//
// Created by w4264 on 2021/1/8.
//

#pragma once
#include <QObject>
#include <QColor>
#include <memory>
#include <lldb/API/LLDB.h>


class LLDBCore;

class App: public QObject
{
	Q_OBJECT

public:
	~App() override;
	static App *get();

	void logError(QString const &msg);
	void logWarn(QString const &msg);
	void logInfo(QString const &msg);
	void i(QString const &msg) { logInfo(msg); }
	void w(QString const &msg) { logWarn(msg); }
	void e(QString const &msg) { logError(msg); }

	void resetCore();

	lldb::SBDebugger const& getDebugger() const {return m_debugger;}
	lldb::SBDebugger& getDebugger() {return m_debugger;}
	LLDBCore *getDbgCore();

	void setExecutable(QString const& fileName) { m_executable = fileName; }
	QString const& getExecutable() const { return m_executable; }
	void setModule(QString const& fileName) { m_module = fileName; }
	QString const& getModule() const { return m_module; }
	void setProcessId(uint64_t tid) { m_processId = tid; }
	uint64_t getProcessId() const { return m_processId; }
	void setThreadId(uint64_t tid) { m_threadId = tid; }
	uint64_t getThreadId() const { return m_threadId; }

signals:
	void outputMsg(QString const &msg, QColor const &color);

	void debugeeExited(int status);
	void onStopState();
	void onThreadFrameChanged(uint64_t tid, int index);
	void gotoMemory(uint64_t address);
	void onModulesChange();
	void onBreakpointChange();

	void openSourceFile(QString const& path);
	void updateTitle();

private:
	App();

	QString m_executable;
	QString m_module;
	uint64_t m_processId = 0;
	uint64_t m_threadId = 0;

	lldb::SBDebugger m_debugger;
	std::unique_ptr<LLDBCore> lldbCore;
};

inline App *app() { return App::get(); }
inline LLDBCore *core() { return app()->getDbgCore(); }

