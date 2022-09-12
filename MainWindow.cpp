//
// Created by w4264 on 2020/12/21.
//

#include "MainWindow.h"
#include <kddockwidgets/DockWidget.h>
#include <QtWidgets>
#include <memory>
#include "App.h"
#include "DisassemblyView.h"
#include "RegisterView.h"
#include "ThreadView.h"
#include "CallStackView.h"
#include "OutputView.h"
#include "MemoryView.h"
#include "OpenExeDlg.h"
#include "AttachDlg.h"
#include "SymbolView.h"
#include "SelectPlatformDlg.h"


MainWindow::MainWindow()
	: KDDockWidgets::MainWindow("XingDBG")
{
	auto fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(tr("Change platform"), this, [this]
    {
        SelectPlatformDlg dlg(this);
        dlg.exec();
    });
	auto openAct = fileMenu->addAction("Open executable", this, [this]
	{
		OpenExeDlg dlg(this);
		if (dlg.exec() != QDialog::Accepted)
			return;

		app()->resetCore();
		auto success = core()->launch(dlg.exePath(), dlg.workingDir(), dlg.stdoutPath(),
									  dlg.stderrPath(), dlg.stdinPath(), dlg.argList(),
									  dlg.envList(), dlg.launchFlags());
		if (!success)
		{
			QMessageBox::warning(this, tr("Error"), tr("Open executable failed"));
			return;
		}

		core()->start();
	});
	auto attachAct = fileMenu->addAction("Attach", this, [this]
	{
		AttachDlg dlg(this);
		if (dlg.exec() != QDialog::Accepted)
			return;

		app()->resetCore();
		core()->attach(dlg.pid());
		core()->start();
	});

//	fileMenu->addAction("远程调试");
	fileMenu->addAction("打开源文件");
	fileMenu->addSeparator();
	fileMenu->addAction("配置");
	fileMenu->addSeparator();
	fileMenu->addAction("退出", this, &MainWindow::close);
	auto fileToolBar = new QToolBar("文件", this);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(attachAct);
	addToolBar(fileToolBar);


	auto viewMenu = menuBar()->addMenu("视图");
	viewMenu->addAction("输出");
	viewMenu->addAction("寄存器");
	viewMenu->addAction("调用堆栈");
	viewMenu->addAction("内存");
	viewMenu->addAction("模块");
	viewMenu->addAction("线程");
	viewMenu->addAction("监视");
	viewMenu->addSeparator();
	viewMenu->addAction("保存布局", this, [this]
	{
		KDDockWidgets::LayoutSaver saver;
		const bool result = saver.saveToFile("main_layout.json");
		statusBar()->showMessage(result ? "保存布局成功" : "保存布局失败");
	});
	viewMenu->addAction("加载布局", this, [this]
	{
		KDDockWidgets::LayoutSaver saver(KDDockWidgets::RestoreOption_RelativeToMainWindow);
		const bool result = saver.restoreFromFile("main_layout.json");
		statusBar()->showMessage(result ? "加载布局成功" : "加载布局失败");
	});

	auto debugMenu = menuBar()->addMenu(tr("Debug"));
	auto runAct = debugMenu->addAction(QIcon(), tr("Run"), this, []
	{
		core()->getProcess().Continue();
	}, QKeySequence(Qt::Key_F9));
	debugMenu->addAction(tr("Pause"));
	debugMenu->addAction(tr("Restart"));
	debugMenu->addAction(tr("Stop"));
	debugMenu->addSeparator();
	auto stepOverAct = debugMenu->addAction(QIcon(), tr("Step over"), this, []
	{
//		core()->getProcess().GetSelectedThread().StepOver();
		lldb::SBError err;
		core()->getProcess().GetSelectedThread().StepInstruction(true, err);
		if (err.Fail())
			app()->e(QString("Step over instruction failed: ").append(err.GetCString()));
	}, QKeySequence(Qt::Key_F8));
	auto stepIntoAct = debugMenu->addAction(QIcon(), tr("Step into"), this, []
	{
//		core()->getProcess().GetSelectedThread().StepInto();
		lldb::SBError err;
		core()->getProcess().GetSelectedThread().StepInstruction(false, err);
		if (err.Fail())
			app()->e(QString("Step into instruction failed: ") + err.GetCString());
	}, QKeySequence(Qt::Key_F7));
	auto debugToolBar = new QToolBar("Debug", this);
	debugToolBar->addAction(runAct);
	debugToolBar->addAction(stepOverAct);
	debugToolBar->addAction(stepIntoAct);
	addToolBar(debugToolBar);


	auto helpMenu = menuBar()->addMenu(tr("Help"));
	auto aboutQtAct = helpMenu->addAction(tr("About Qt"), this, [this]
	{
		QMessageBox::aboutQt(this);
	});
	auto aboutAct = helpMenu->addAction(tr("About"));
	auto helpToolBar = new QToolBar(tr("Help"), this);
	helpToolBar->addAction(aboutAct);
	addToolBar(helpToolBar);


	LLDBCore::init();

	setupDockWidgets();

	statusBar()->showMessage("Done.");
}

void MainWindow::setupDockWidgets()
{
	// TODO:
	QFont f{"Sarasa Fixed SC", 14};
	f.setStyleHint(QFont::Monospace);

	auto outputDock = new KDDockWidgets::DockWidget(tr("Output"));
	outputDock->setWidget(new OutputView());


	auto cpuDock = new KDDockWidgets::DockWidget(tr("CPU"));
	auto disasmView = new DisassemblyView();
	disasmView->init();
	disasmView->updateFont(f);
	cpuDock->setWidget(disasmView);
	addDockWidget(cpuDock, KDDockWidgets::Location_OnLeft);

	auto sourceDock = new KDDockWidgets::DockWidget("Source");
	auto widget7 = new QLabel("假装有源文件");
	widget7->setStyleSheet("background: black;");
	sourceDock->setWidget(widget7);
	addDockWidget(sourceDock, KDDockWidgets::Location_None, cpuDock);


	auto regDock = new KDDockWidgets::DockWidget(tr("Register"));
	regDock->setWidget(new RegisterView());
	addDockWidget(regDock, KDDockWidgets::Location_OnRight, cpuDock);

	auto threadDock = new KDDockWidgets::DockWidget(tr("Thread"));
	threadDock->setWidget(new ThreadView());
	addDockWidget(threadDock, KDDockWidgets::Location_None, regDock);


	addDockWidget(outputDock, KDDockWidgets::Location_OnBottom);

	auto callStackDock = new KDDockWidgets::DockWidget(tr("Call Stack"));
	callStackDock->setWidget(new CallStackView());
	addDockWidget(callStackDock, KDDockWidgets::Location_OnRight, outputDock);

	auto memoryDock = new KDDockWidgets::DockWidget(tr("Memory"));
	auto memoryView = new MemoryView();
	memoryView->init();
	memoryView->updateFont(f);
	memoryDock->setWidget(memoryView);
	addDockWidget(memoryDock, KDDockWidgets::Location_OnRight, outputDock);

	auto symbolDock = new KDDockWidgets::DockWidget(tr("Symbol"));
	symbolDock->setWidget(new SymbolView());
	addDockWidget(symbolDock, KDDockWidgets::Location_OnLeft, regDock);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (core() && core()->isRunning())
	{
		if (QMessageBox::question(this, tr("Debugger is still running"),
								  tr("Do you want to exit debugger?")) != QMessageBox::Yes)
			return;
		app()->resetCore();
	}
	KDDockWidgets::MainWindow::closeEvent(event);
}
