//
// Created by w4264 on 2020/12/21.
//

#include "MainWindow.h"
#include <kddockwidgets/DockWidget.h>
#include <QtWidgets>
#include "App.h"
#include "Views/DisassemblyView.h"
#include "Views/RegisterView.h"
#include "Views/ThreadView.h"
#include "Views/CallStackView.h"
#include "Views/OutputView.h"
#include "Views/MemoryView.h"
#include "Views/SymbolView.h"
#include "Views/SourceView.h"
#include "Views/SourceListView.h"
#include "Views/BreakpointView.h"
#include "Dialogs/SelectPlatformDlg.h"
#include "Dialogs/OpenExeDlg.h"
#include "Dialogs/AttachDlg.h"
#include "Dialogs/AboutDlg.h"


MainWindow::MainWindow()
	: KDDockWidgets::MainWindow("XingDBG")
{
	LLDBCore::init();

	setupDockWidgets();
	setupMenuToolBar();
	statusBar()->showMessage("Done.");


	connect(app(), &App::openSourceFile, this, [this](QString const& path)
	{
		if (!m_sourceDock->isVisible())
		{
			m_sourceDock->show();
		}
		auto srcView = static_cast<SourceView*>(m_sourceDock->widget());
		srcView->addSourceFile(path);
	});
}

void MainWindow::setupDockWidgets()
{
	// TODO:
	QFont f{"Sarasa Fixed SC", 14};
	f.setStyleHint(QFont::Monospace);



	m_cpuDock = new KDDockWidgets::DockWidget(tr("CPU"));
	auto disasmView = new DisassemblyView();
	disasmView->init();
	disasmView->updateFont(f);
	m_cpuDock->setWidget(disasmView);
	m_cpuDock->setIcon(QIcon(":/img/cpu.png"));
	addDockWidget(m_cpuDock, KDDockWidgets::Location_OnLeft);

	m_regDock = new KDDockWidgets::DockWidget(tr("Register"));
	m_regDock->setWidget(new RegisterView());
	addDockWidget(m_regDock, KDDockWidgets::Location_OnRight, m_cpuDock);

	m_outputDock = new KDDockWidgets::DockWidget(tr("Output"));
	m_outputDock->setWidget(new OutputView());
	m_outputDock->setIcon(QIcon(":/img/output.png"));
	addDockWidget(m_outputDock, KDDockWidgets::Location_OnBottom);

	m_callStackDock = new KDDockWidgets::DockWidget(tr("Call Stack"));
	m_callStackDock->setWidget(new CallStackView());
	addDockWidget(m_callStackDock, KDDockWidgets::Location_OnRight, m_outputDock);

	m_memoryDock = new KDDockWidgets::DockWidget(tr("Memory"));
	auto memoryView = new MemoryView();
	memoryView->init();
	memoryView->updateFont(f);
	m_memoryDock->setWidget(memoryView);
	addDockWidget(m_memoryDock, KDDockWidgets::Location_OnRight, m_outputDock);




	m_sourceListDock = new KDDockWidgets::DockWidget("Source List");
	m_sourceListDock->setWidget(new SourceListView());
	m_sourceListDock->setIcon(QIcon(":/img/source-list.png"));

	m_sourceDock = new KDDockWidgets::DockWidget("Source");
	m_sourceDock->setWidget(new SourceView());
	m_sourceDock->setIcon(QIcon(":/img/source-code.png"));

	m_threadDock = new KDDockWidgets::DockWidget(tr("Thread"));
	m_threadDock->setWidget(new ThreadView());

	m_symbolDock = new KDDockWidgets::DockWidget(tr("Symbol"));
	m_symbolDock->setWidget(new SymbolView());

	m_bpDock = new KDDockWidgets::DockWidget(tr("Breakpoint"));
	m_bpDock->setWidget(new BreakpointView());
}

void MainWindow::setupMenuToolBar()
{
	auto fileMenu = menuBar()->addMenu("File");
	fileMenu->addAction(tr("Change platform"), this, [this]
	{
		SelectPlatformDlg dlg(this);
		dlg.exec();
	});
	auto openAct = fileMenu->addAction(QIcon(":/img/open.png"), "Open executable", this, [this]
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
	auto attachAct = fileMenu->addAction(QIcon(":/img/attach-to.png"), "Attach", this, [this]
	{
		AttachDlg dlg(this);
		if (dlg.exec() != QDialog::Accepted)
			return;

		app()->resetCore();
		core()->attach(dlg.pid());
		core()->start();
	});

	fileMenu->addSeparator();
	fileMenu->addAction("Config");
	fileMenu->addSeparator();
	fileMenu->addAction(QIcon(":/img/exit.png"), "Exit", this, &MainWindow::close);
	auto fileToolBar = new QToolBar("File", this);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(attachAct);
	addToolBar(fileToolBar);


	auto viewMenu = menuBar()->addMenu("View");
	viewMenu->addAction(m_outputDock->icon(), "Output", [this] {m_outputDock->show(); });
	viewMenu->addAction(m_regDock->icon(), "Register", [this] {m_regDock->show(); });
	viewMenu->addAction(m_callStackDock->icon(), "Call stack", [this] {m_callStackDock->show(); });
	viewMenu->addAction(m_memoryDock->icon(), "Memory", [this] {m_memoryDock->show(); });
	viewMenu->addAction(m_symbolDock->icon(), "Symbol", [this] {m_symbolDock->show(); });
	viewMenu->addAction(m_threadDock->icon(), "Thread", [this] {m_threadDock->show(); });
	viewMenu->addAction(m_bpDock->icon(), "Breakpoint", [this] {m_bpDock->show(); });
	viewMenu->addSeparator();
	viewMenu->addAction("Save layout", this, [this]
	{
		KDDockWidgets::LayoutSaver saver;
		const bool result = saver.saveToFile("main_layout.json");
		statusBar()->showMessage(result ? "保存布局成功" : "保存布局失败");
	});
	viewMenu->addAction("Restore layout", this, [this]
	{
		KDDockWidgets::LayoutSaver saver(KDDockWidgets::RestoreOption_RelativeToMainWindow);
		const bool result = saver.restoreFromFile("main_layout.json");
		statusBar()->showMessage(result ? "加载布局成功" : "加载布局失败");
	});

	auto debugMenu = menuBar()->addMenu(tr("Debug"));
	auto runAct = debugMenu->addAction(QIcon(":/img/run.png"), tr("Run"), this, []
	{
		core()->getProcess().Continue();
	}, QKeySequence(Qt::Key_F9));
	debugMenu->addAction(QIcon(":/img/pause.png"), tr("Pause"));
	debugMenu->addAction(QIcon(":/img/restart.png"), tr("Restart"));
	debugMenu->addAction(QIcon(":/img/stop.png"), tr("Stop"));
	debugMenu->addSeparator();
	auto stepOverAct = debugMenu->addAction(QIcon(":/img/step-over.png"), tr("Step over"), this, []
	{
		//		core()->getProcess().GetSelectedThread().StepOver();
		lldb::SBError err;
		core()->getProcess().GetSelectedThread().StepInstruction(true, err);
		if (err.Fail())
			app()->e(QString("Step over instruction failed: ").append(err.GetCString()));
	}, QKeySequence(Qt::Key_F8));
	auto stepIntoAct = debugMenu->addAction(QIcon(":/img/step-into.png"), tr("Step into"), this, []
	{
		//		core()->getProcess().GetSelectedThread().StepInto();
		lldb::SBError err;
		core()->getProcess().GetSelectedThread().StepInstruction(false, err);
		if (err.Fail())
			app()->e(QString("Step into instruction failed: ") + err.GetCString());
	}, QKeySequence(Qt::Key_F7));
	debugMenu->addAction(tr("Switch breakpoint"), this, []
	{
		//TODO:
	});
	auto debugToolBar = new QToolBar("Debug", this);
	debugToolBar->addAction(runAct);
	debugToolBar->addAction(stepOverAct);
	debugToolBar->addAction(stepIntoAct);
	addToolBar(debugToolBar);


	auto helpMenu = menuBar()->addMenu(tr("Help"));
	auto aboutQtAct = helpMenu->addAction(QIcon(":/img/about-qt.png"), tr("About Qt"), this, [this]
	{
		QMessageBox::aboutQt(this);
	});
	auto aboutAct = helpMenu->addAction(QIcon(":/img/about.png"), tr("About"), this, [this]
	{
		AboutDlg dlg(this);
		dlg.exec();
	});
	auto helpToolBar = new QToolBar(tr("Help"), this);
	helpToolBar->addAction(aboutAct);
	addToolBar(helpToolBar);
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
