//
// Created by xing on 2022/5/11.
//

#pragma once
#include <QListWidget>
#include "LLDBCore.h"


class CallStackView : public QListWidget
{
	Q_OBJECT
public:
	CallStackView();

	void setThread(lldb::SBThread thread);

private:
	uint64_t m_tid = 0;
};


