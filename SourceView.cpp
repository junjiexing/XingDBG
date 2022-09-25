
#include "SourceView.h"
#include "App.h"
#include "LLDBCore.h"
#include <QtWidgets>
#include <QFile>


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
	auto edt = new QTextEdit;
	edt->setText(QString::fromLocal8Bit(bytes));
	edt->setProperty("path", path);

	auto i = m_tab->addTab(edt, QFileInfo(path).fileName());
	m_tab->setCurrentIndex(i);
}

