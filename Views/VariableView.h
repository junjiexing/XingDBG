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
	const char* convertValueType(lldb::ValueType t);
	void setItemsHide();
	void removeHiddenItems();
	void addVariable(lldb::SBValue v);

private:
	QTreeWidget* m_varTree;

	lldb::SBFrame m_frame;
};
