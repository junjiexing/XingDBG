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


signals:
	void outputMsg(QString const &msg, QColor const &color);

	void debugeeExited(int status);
	void onStopState();
	void onThreadFrameChanged(uint64_t tid, int index);
	void gotoMemory(uint64_t address);

	void openSourceFile(QString const& path);


private:
	App();

	lldb::SBDebugger m_debugger;
	std::unique_ptr<LLDBCore> lldbCore;
};

inline App *app() { return App::get(); }
inline LLDBCore *core() { return app()->getDbgCore(); }

