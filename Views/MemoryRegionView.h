#pragma once

#include <QWidget>


class QTableWidget;

class MemoryRegionView : public QWidget
{
	Q_OBJECT

public:
	MemoryRegionView(QWidget *parent = nullptr);
	~MemoryRegionView();

	void refresh();

private:
	QTableWidget* m_regionListTable;
};
