//
// Created by Xing on 2020/12/21.
//

#pragma once

#include <kddockwidgets/MainWindow.h>
#include <kddockwidgets/DockWidget.h>

#include "LLDBCore.h"
#include <memory>


class SourceView;

class MainWindow : public KDDockWidgets::MainWindow
{
	Q_OBJECT
public:
	MainWindow();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	void setupDockWidgets();

private:
	KDDockWidgets::DockWidget* m_sourceDock = nullptr;
};


