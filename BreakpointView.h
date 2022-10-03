#pragma once

#include <QWidget>

class QTreeWidget;

class BreakpointView : public QWidget
{
	Q_OBJECT
public:
	BreakpointView();

	void refresh();

protected:
	void contextMenuEvent(QContextMenuEvent* event) override;
private:
	QTreeWidget* m_tree;

};