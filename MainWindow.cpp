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


MainWindow::MainWindow()
		: KDDockWidgets::MainWindow("XingDBG")
{
	auto fileMenu = menuBar()->addMenu("文件");
	auto openAct = fileMenu->addAction("打开可执行程序", this, [] {
		App::get()->launch(R"(E:\msys64\home\xing\a.exe)", "");
		App::get()->getDbgCore()->start();
	});
	fileMenu->addAction("附加进程");
	fileMenu->addAction("远程调试");
	fileMenu->addAction("打开源文件");
	fileMenu->addSeparator();
	fileMenu->addAction("配置");
	fileMenu->addSeparator();
	fileMenu->addAction("退出", this, &MainWindow::close);
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
		statusBar()->showMessage(result? "保存布局成功": "保存布局失败");
	});
	viewMenu->addAction("加载布局", this, [this]
	{
		KDDockWidgets::LayoutSaver saver(KDDockWidgets::RestoreOption_RelativeToMainWindow);
		const bool result = saver.restoreFromFile("main_layout.json");
		statusBar()->showMessage(result? "加载布局成功": "加载布局失败");
	});
	auto helpMenu = menuBar()->addMenu("帮助");
	auto aboutAct = helpMenu->addAction("关于");

	auto helpToolBar = new QToolBar("帮助", this);
	helpToolBar->addAction(aboutAct);
	addToolBar(helpToolBar);

	auto fileToolBar = new QToolBar("文件", this);
	fileToolBar->addAction(openAct);
	addToolBar(fileToolBar);


	auto outputDock = new KDDockWidgets::DockWidget("输出");
	auto outputEdt = new QTextEdit;
	outputEdt->setReadOnly(true);
	outputDock->setWidget(outputEdt);
	connect(App::get(), &App::outputMsg, outputEdt, [outputEdt](QString const& msg, QColor const& color){
		outputEdt->setTextColor(color);
		outputEdt->append(msg);
	});

	auto dock4 = new KDDockWidgets::DockWidget("内存");
	auto widget4 = new QLabel("内存");
	widget4->setStyleSheet("background: yellow;");
	dock4->setWidget(widget4);

	auto dock6 = new KDDockWidgets::DockWidget("监视");
	auto widget6 = new QLabel("监视");
	widget6->setStyleSheet("background: cyan;");
	dock6->setWidget(widget6);
	dock6->show();

	auto dock7 = new KDDockWidgets::DockWidget("a.cpp");
	auto widget7 = new QLabel("假装有源文件");
	widget7->setStyleSheet("background: black;");
	dock7->setWidget(widget7);
	dock7->show();

	auto disasmDock = new DisassemblyView();
	disasmDock->show();

	auto registerView = new RegisterView();
	registerView->show();

	auto threadView = new ThreadView();
	threadView->show();

	auto callStackView = new CallStackView();
	callStackView->show();


	addDockWidget(outputDock, KDDockWidgets::Location_OnLeft);
//	addDockWidget(dock2, KDDockWidgets::Location_OnTop);

	dock4->show();
	statusBar()->showMessage("Done.");


	LLDBCore::init();
}
