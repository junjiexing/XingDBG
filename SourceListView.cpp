#include "SourceListView.h"
#include "LLDBCore.h"
#include "App.h"
#include <QtWidgets>


// TODO: merge.
static inline QTableWidgetItem* newItem(const QString& txt)
{
	auto item = new QTableWidgetItem(txt);
	item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	return item;
}


SourceListView::SourceListView()
	: QWidget(nullptr)
{
	auto srcFileTable = new QTableWidget(0, 3);
	srcFileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	srcFileTable->setHorizontalHeaderLabels(QStringList() << tr("Module") << tr("Path") << tr("Redirect Path"));

	connect(srcFileTable, &QTableWidget::doubleClicked, this, [=](const QModelIndex& index)
		{
			auto it = srcFileTable->item(index.row(), 1);
			if (!it) return;

			//TODO: is file exit?
			emit app()->openSourceFile(it->text());
		});

	auto refreshAction = new QAction(tr("refresh"), this);
	connect(refreshAction, &QAction::triggered, this, [=]
		{
			auto target = core()->getTarget();
			auto numModules = target.GetNumModules();
			srcFileTable->clearContents();
			int row = 0;
			for (int i = 0; i < numModules; ++i)
			{
				auto m = target.GetModuleAtIndex(i);
				auto numUnits = m.GetNumCompileUnits();
				for (int j = 0; j < numUnits; ++j)
				{
					auto unit = m.GetCompileUnitAtIndex(j);
					char pathBuf[1000];
					unit.GetFileSpec().GetPath(pathBuf, 1000);
					QString path(pathBuf);
					if (path.isEmpty()) continue;
					srcFileTable->insertRow(row);
					srcFileTable->setItem(row, 0, newItem(m.GetFileSpec().GetFilename()));
					srcFileTable->setItem(row, 1, newItem(path));
					++row;
				}
			}
		});

	addAction(refreshAction);


	auto lay = new QVBoxLayout(this);
	lay->addWidget(srcFileTable);
}

SourceListView::~SourceListView()
{

}

void SourceListView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu;
	menu.addActions(actions());
	menu.exec(event->globalPos());
}
