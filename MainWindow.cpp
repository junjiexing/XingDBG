//
// Created by w4264 on 2020/12/21.
//

#include "MainWindow.h"
#include <kddockwidgets/DockWidget.h>
#include <QLabel>

MainWindow::MainWindow()
	:KDDockWidgets::MainWindow("XingDBG")
{
	auto dock1 = new KDDockWidgets::DockWidget("MyDock1");
	auto widget1 = new QLabel("MyDock1");
	widget1->setStyleSheet("background: red;");
	dock1->setWidget(widget1);

	auto dock2 = new KDDockWidgets::DockWidget("MyDock2");
	auto widget2 = new QLabel("MyDock2");
	widget2->setStyleSheet("background: blue;");
	dock2->setWidget(widget2);

	auto dock3 = new KDDockWidgets::DockWidget("MyDock3");
	auto widget3 = new QLabel("MyDock3");
	widget3->setStyleSheet("background: green;");
	dock3->setWidget(widget3);

	auto dock4 = new KDDockWidgets::DockWidget("MyDock4");
	auto widget4 = new QLabel("MyDock4");
	widget4->setStyleSheet("background: yellow;");
	dock4->setWidget(widget4);


	addDockWidget(dock1, KDDockWidgets::Location_OnLeft);
	addDockWidget(dock2, KDDockWidgets::Location_OnTop);

	addDockWidget(dock3, KDDockWidgets::Location_OnRight, dock2);

	dock4->show();
}
