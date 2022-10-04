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
	void setupMenuToolBar();

private:
	KDDockWidgets::DockWidget* m_cpuDock = nullptr;
	KDDockWidgets::DockWidget* m_regDock = nullptr;
	KDDockWidgets::DockWidget* m_outputDock = nullptr;
	KDDockWidgets::DockWidget* m_callStackDock = nullptr;
	KDDockWidgets::DockWidget* m_memoryDock = nullptr;

	KDDockWidgets::DockWidget* m_sourceListDock = nullptr;
	KDDockWidgets::DockWidget* m_sourceDock = nullptr;
	KDDockWidgets::DockWidget* m_threadDock = nullptr;
	KDDockWidgets::DockWidget* m_symbolDock = nullptr;
	KDDockWidgets::DockWidget* m_bpDock = nullptr;
};


