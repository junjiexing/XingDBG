//
// Created by xing on 2022/5/11.
//

#pragma once
#include <QTreeWidget>
#include "LLDBCore.h"



class RegisterView final : public QTreeWidget
{
	Q_OBJECT

public:
	RegisterView();

public slots:
	void setThreadFrame(lldb::SBThread thread, int index);

private:
	void addValue(lldb::SBValue value, QTreeWidgetItem* parent);
protected:
	void contextMenuEvent(QContextMenuEvent *event) override;
};


