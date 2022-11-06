
#include "SourceView.h"
#include "App.h"
#include "LLDBCore.h"
#include <QtWidgets>
#include <QFile>
#include "Widgets/CodeEditor.h"


SourceView::SourceView()
	: QWidget(nullptr)
{
	m_tab = new QTabWidget;
	m_tab->setTabsClosable(true);
	m_tab->setMovable(true);


	auto lay = new QVBoxLayout(this);
	lay->addWidget(m_tab);

	auto bpAct = new QAction;
	bpAct->setShortcut(Qt::Key_F2);
	bpAct->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(bpAct, &QAction::triggered, this, [] 
		{
			//TODO:
		});
	addAction(bpAct);

	connect(app(), &App::onBreakpointChange, this, &SourceView::updateBreakpoints);
	connect(app(), &App::onStopState, this, &SourceView::updateCurrentLine);
}

SourceView::~SourceView() = default;

void SourceView::addSourceFile(QString const& path)
{
	for (int i = 0; i < m_tab->count(); ++i)
	{
		auto edt = m_tab->widget(i);
		if (edt && edt->property("path").toString() == path)
		{
			m_tab->setCurrentIndex(i);
			return;
		}
	}

	QFile f(path);
	if (!f.open(QFile::ReadOnly))
	{
		app()->w(tr("Open source file failed."));
		return;
	}

	auto bytes = f.readAll();
	//TODO: encoding
	auto edt = new CodeEditor;
	edt->setPlainText(QString::fromLocal8Bit(bytes));
	edt->setReadOnly(true);
	edt->setProperty("path", path);

	edt->setOnSwitchBreakpoint([path, this](int line)
	{
		auto target = core()->getTarget();
		bool found = false;
		for (int i = 0; i < target.GetNumBreakpoints(); ++i)
		{
			auto bp = target.GetBreakpointAtIndex(i);

			auto locNum = bp.GetNumLocations();
			for (int j = 0; j < bp.GetNumLocations(); ++j)
			{
				auto loc = bp.GetLocationAtIndex(j);
				auto lineEntry = loc.GetAddress().GetLineEntry();
				if (!lineEntry)
				{
					continue;
				}

				char pathBuf[1000];
				lineEntry.GetFileSpec().GetPath(pathBuf, 1000);
				if (path == pathBuf && lineEntry.GetLine() == line)
				{
					found = true;
					if (locNum > 1
						&& QMessageBox::question(this, tr("Warning"),
							tr("This breakpoint has more than one locations,"
								" the operation will delete all locations,"
								" do you want to continue?")) != QMessageBox::Yes)
					{
						break;
					}

					if (target.BreakpointDelete(bp.GetID()))
					{
						app()->i(tr("delete breakpoint success"));
					}
					else
					{
						app()->e(tr("delete breakpoint failed"));
					}
				}
			}
		}
		if (!found)
		{
			auto bp = target.BreakpointCreateByLocation(path.toLocal8Bit(), line);
			if (!bp.IsValid())
			{
				app()->e(tr("create breakpoint failed"));
				return;
			}

			app()->i(tr("create breakpoint success, id: %1").arg(bp.GetID()));
		}
	});

	auto i = m_tab->addTab(edt, QFileInfo(path).fileName());
	m_tab->setCurrentIndex(i);

	updateBreakpoints();
}

void SourceView::updateBreakpoints()
{
	for (int i = 0; i < m_tab->count(); ++i)
	{
		auto edt = static_cast<CodeEditor*>(m_tab->widget(i));
		edt->clearBreakpoint();
	}

	auto target = core()->getTarget();
	for (int i = 0; i < target.GetNumBreakpoints(); ++i)
	{
		auto bp = target.GetBreakpointAtIndex(i);

		auto locNum = bp.GetNumLocations();
		for (int j = 0; j < bp.GetNumLocations(); ++j)
		{
			auto loc = bp.GetLocationAtIndex(j);
			auto lineEntry = loc.GetAddress().GetLineEntry();
			if (!lineEntry)
			{
				continue;
			}

			char pathBuf[1000];
			lineEntry.GetFileSpec().GetPath(pathBuf, 1000);
			QString path(pathBuf);

			for (int k = 0; k < m_tab->count(); ++k)
			{
				auto edt = static_cast<CodeEditor*>(m_tab->widget(k));
				auto pathProp = edt->property("path").toString();
				if (pathProp == path)
				{
					auto line = lineEntry.GetLine();
					edt->addBreakpoint({ line, bp.IsEnabled() });
					break;
				}
			}
		}
	}
}

void SourceView::updateCurrentLine()
{
	for (int i = 0; i < m_tab->count(); ++i)
	{
		auto edt = static_cast<CodeEditor*>(m_tab->widget(i));
		edt->setCurrentLine(0);
	}

	auto process = core()->getProcess();
	auto lineEntry = process.GetSelectedThread().GetSelectedFrame().GetLineEntry();
	
	if (!lineEntry)
	{
		return;
	}

	char pathBuf[1000];
	lineEntry.GetFileSpec().GetPath(pathBuf, 1000);
	QString path(pathBuf);

	for (int i = 0; i < m_tab->count(); ++i)
	{
		auto edt = static_cast<CodeEditor*>(m_tab->widget(i));
		auto pathProp = edt->property("path").toString();
		if (pathProp == path)
		{
			auto line = lineEntry.GetLine();
			edt->setCurrentLine(int(line));
			break;
		}
	}

}

