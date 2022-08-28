//
// Created by xing on 2022-08-21.
//

#include "AttachDlg.h"
#include <QtWidgets>
#include "App.h"
#include "LLDBCore.h"


AttachDlg::AttachDlg(QWidget* parent)
	: QDialog(parent)
{
	setWindowTitle(tr("Attach"));

	m_pidEdt = new QLineEdit;
	auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	auto lay = new QFormLayout(this);
	lay->addRow(tr("PID:"), m_pidEdt);
	lay->addRow(btnBox);
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
