//
// Created by w4264 on 2020/12/21.
//

#include "MainWindow.h"
#include <kddockwidgets/DockWidget.h>
#include <QtWidgets>
#include <memory>
#include "App.h"


MainWindow::MainWindow()
		: KDDockWidgets::MainWindow("XingDBG")
{
	auto fileMenu = menuBar()->addMenu("文件");
	fileMenu->addAction("打开可执行程序", this, [this] {
		m_lldbCore = std::make_unique<LLDBCore>(R"(D:\msys64\home\w4264\a.exe)", "");
		m_lldbCore->start();
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
		KDDockWidgets::LayoutSaver saver;
		const bool result = saver.restoreFromFile("main_layout.json");
		statusBar()->showMessage(result? "加载布局成功": "加载布局失败");
	});
	auto helpMenu = menuBar()->addMenu("帮助");
	helpMenu->addAction("关于");


	auto outputDock = new KDDockWidgets::DockWidget("输出");
	auto outputEdt = new QTextEdit;
	outputEdt->setReadOnly(true);
	outputDock->setWidget(outputEdt);
	connect(App::get(), &App::outputMsg, outputEdt, [outputEdt](QString const& msg, QColor const& color){
		outputEdt->setTextColor(color);
		outputEdt->append(msg);
	});

	auto dock2 = new KDDockWidgets::DockWidget("寄存器");
	auto widget2 = new QLabel("寄存器");
	widget2->setStyleSheet("background: blue;");
	dock2->setWidget(widget2);

	auto dock3 = new KDDockWidgets::DockWidget("调用堆栈");
	auto widget3 = new QLabel("调用堆栈");
	widget3->setStyleSheet("background: green;");
	dock3->setWidget(widget3);

	auto dock4 = new KDDockWidgets::DockWidget("内存");
	auto widget4 = new QLabel("内存");
	widget4->setStyleSheet("background: yellow;");
	dock4->setWidget(widget4);

	auto dock5 = new KDDockWidgets::DockWidget("线程");
	auto widget5 = new QLabel("线程");
	widget5->setStyleSheet("background: pink;");
	dock5->setWidget(widget5);
	dock5->show();

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


	addDockWidget(outputDock, KDDockWidgets::Location_OnLeft);
	addDockWidget(dock2, KDDockWidgets::Location_OnTop);

	addDockWidget(dock3, KDDockWidgets::Location_OnRight, dock2);

	dock4->show();

	statusBar()->showMessage("Done.");


	App::get()->logError("Error");
	App::get()->logWarn("Warn");
	App::get()->logInfo("Info");

}
