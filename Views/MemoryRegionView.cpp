#include "MemoryRegionView.h"
#include "App.h"
#include "LLDBCore.h"
#include <QtWidgets>
#include "Utils.h"


// TODO: merge.
static inline QTableWidgetItem* newItem(const QString& txt)
{
	auto item = new QTableWidgetItem(txt);
	item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	return item;
}

MemoryRegionView::MemoryRegionView(QWidget *parent)
	: QWidget(parent)
{
	m_regionListTable = new QTableWidget(0, 8);
	m_regionListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_regionListTable->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Base") << tr("End")
		<< tr("Readable") << tr("Writable") << tr("Executable") << tr("Mapped") << tr("Page Size"));

	connect(app(), &App::onStopState, this, &MemoryRegionView::refresh);

	auto lay = new QVBoxLayout(this);
	lay->setSpacing(0);
	lay->addWidget(m_regionListTable);
}

MemoryRegionView::~MemoryRegionView()
{
}

void MemoryRegionView::refresh()
{
	auto process = core()->getProcess();
	auto regions = process.GetMemoryRegions();

	m_regionListTable->setRowCount(regions.GetSize());

	for (uint32_t i = 0; i < regions.GetSize(); ++i)
	{
		lldb::SBMemoryRegionInfo info;
		if (!regions.GetMemoryRegionAtIndex(i, info))
		{
			app()->w(tr("GetMemoryRegionAtIndex %1 failed.").arg(i));
			continue;
		}
		m_regionListTable->setItem(i, 0, newItem(info.GetName()));
		m_regionListTable->setItem(i, 1, newItem(Utils::hex(info.GetRegionBase())));
		m_regionListTable->setItem(i, 2, newItem(Utils::hex(info.GetRegionEnd())));
		m_regionListTable->setItem(i, 3, newItem(info.IsReadable()? tr("Yes") : tr("No")));
		m_regionListTable->setItem(i, 4, newItem(info.IsWritable() ? tr("Yes") : tr("No")));
		m_regionListTable->setItem(i, 5, newItem(info.IsExecutable() ? tr("Yes") : tr("No")));
		m_regionListTable->setItem(i, 6, newItem(info.IsMapped() ? tr("Yes") : tr("No")));
		m_regionListTable->setItem(i, 7, newItem(QString::number(info.GetPageSize())));
	}

}
