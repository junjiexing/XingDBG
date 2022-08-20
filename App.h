//
// Created by w4264 on 2021/1/8.
//

#pragma once
#include <QObject>
#include <QColor>
#include <memory>

class LLDBCore;

class App : public QObject
{
	Q_OBJECT

public:
	~App() override;
	static App* get();

	void logError(QString const& msg);
	void logWarn(QString const& msg);
	void logInfo(QString const& msg);


	[[nodiscard]] LLDBCore* getDbgCore();

signals:
	void outputMsg(QString const& msg, QColor const& color);

	void debugeeExited(int status);
	void onStopState();
	void onThreadFrameChanged(uint64_t tid, int index);
	void gotoMemory(uint64_t address);


private:
	App();

	std::unique_ptr<LLDBCore> lldbCore;
};

#define app() App::get()
#define core() App::get()->getDbgCore()

