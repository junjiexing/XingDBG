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

	m_platformCombo = new QComboBox;
	auto platforms = core()->platforms();
	auto descriptionLab = new QLabel;
	connect(m_platformCombo, &QComboBox::currentIndexChanged, this, [=, this](int index)
	{
		auto description = m_platformCombo->itemData(index).toString();
		descriptionLab->setText(description);
	});
	for (auto const& platform : platforms)
	{
		m_platformCombo->addItem(platform.first, platform.second);
	}
	m_connectUrlEdt = new QLineEdit;

	m_pidEdt = new QLineEdit;
	auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	auto lay = new QFormLayout(this);
	lay->addRow(tr("Platform:"), m_platformCombo);
	lay->addRow(tr("Description:"), descriptionLab);
	lay->addRow(tr("Connect url:"), m_connectUrlEdt);
	lay->addRow(tr("PID:"), m_pidEdt);
	lay->addRow(btnBox);
}

uint64_t AttachDlg::pid()
{
	return m_pidEdt->text().toULongLong();
}

QString AttachDlg::platformName()
{
	return m_platformCombo->currentText();
}

QString AttachDlg::connectUrl()
{
	return m_connectUrlEdt->text();
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
