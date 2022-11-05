#pragma once

#include <QWidget>
#include "LLDBCore.h"


class QTreeWidget;
class QTreeWidgetItem;

class VariableView : public QWidget
{
	Q_OBJECT

public:
	VariableView(QWidget *parent = nullptr);
	~VariableView();


	void refresh();

private:
	QTreeWidgetItem* newParentItem(QString const& name);
	void addVariable(QTreeWidgetItem* parent, lldb::SBValue v);

private:
	QTreeWidget* m_varTree;

	lldb::SBFrame m_frame;
};
