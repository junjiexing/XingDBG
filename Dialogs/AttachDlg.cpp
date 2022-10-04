//
// Created by xing on 2022-08-21.
//

#include "AttachDlg.h"
#include <QtWidgets>
#include "App.h"
#include "LLDBCore.h"
#include "Utils.h"
#include <lldb/Target/Platform.h>
#include <lldb/Utility/ProcessInfo.h>

// TODO: merge.
static inline QTableWidgetItem *newItem(const QString &txt)
{
	auto item = new QTableWidgetItem(txt);
	item->setFlags(item->flags() ^ Qt::ItemIsEditable);
	return item;
}


AttachDlg::AttachDlg(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Attach"));

	m_pidEdt = new QLineEdit;
	auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	auto processListTable = new QTableWidget(0, 7);
	processListTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	processListTable->setHorizontalHeaderLabels(QStringList() << tr("PID") << tr("Name") << tr("File") << tr("UID") << tr("GID") << tr("Arch") << tr("Args"));

	auto platform = app()->getDebugger().GetSelectedPlatform();
	auto platformSp = Utils::GetSp<lldb::SBPlatform>(platform).sp();

	lldb_private::ProcessInstanceInfoList plist;
	platformSp->FindProcesses({}, plist);

	processListTable->setRowCount(plist.size());
	for (int i = 0; i < plist.size(); ++i)
	{
		auto& pi = plist.at(i);
		// TODO: win上显示的似乎有问题，其他平台还不清楚.
		std::string cmd;
		pi.GetArguments().GetCommandString(cmd);
		processListTable->setItem(i, 0, newItem(QString::number(pi.GetProcessID())));
		processListTable->setItem(i, 1, newItem(pi.GetName()));
		processListTable->setItem(i, 2, newItem(pi.GetExecutableFile().GetPath().c_str()));
		processListTable->setItem(i, 3, newItem(QString::number(pi.GetUserID())));
		processListTable->setItem(i, 4, newItem(QString::number(pi.GetGroupID())));
		processListTable->setItem(i, 5, newItem(pi.GetArchitecture().GetArchitectureName()));
		processListTable->setItem(i, 6, newItem(cmd.c_str()));
	}

	connect(processListTable->selectionModel(), &QItemSelectionModel::currentRowChanged, this,
			[=, this](const QModelIndex &current, const QModelIndex &)
			{
				auto row = current.row();
				m_pidEdt->setText(processListTable->item(row, 0)->text());
			});

	auto vlay = new QVBoxLayout(this);
	vlay->addWidget(processListTable);
	auto flay = new QFormLayout;
	vlay->addLayout(flay);
	flay->addRow(tr("PID:"), m_pidEdt);
	flay->addRow(btnBox);
}

uint64_t AttachDlg::pid()
{
	return m_pidEdt->text().toULongLong();
}

void AttachDlg::accept()
{
	if (pid() == 0)
	{
		QMessageBox::information(this, tr("Tip"), tr("pid is invalid"));
		return;
	}
	QDialog::accept();
}
