//
// Created by xing on 2022-08-21.
//

#pragma once
#include <QDialog>


class QLineEdit;
class QComboBox;

class AttachDlg : public QDialog
{
	Q_OBJECT

public:
	explicit AttachDlg(QWidget* parent);

	uint64_t pid();

	void accept() override;

private:
	QLineEdit* m_pidEdt;
};





