//
// Created by xing on 2022/8/19.
//

#pragma once

#include <QDialog>

class QLineEdit;
class QListWidget;
class QTableWidget;
class QComboBox;

class OpenExeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit OpenExeDlg(QWidget* parent);

	QString exePath();
    QString workingDir();
    QString stdoutPath();
    QString stderrPath();
    QString stdinPath();
    QStringList argList();
    QStringList envList();
    uint32_t launchFlags() {return m_launchFlags;}

    void accept() override;

private:
    QLineEdit* m_exePathEdt;
    QLineEdit* m_workDirEdt;
    QLineEdit* m_stdoutPathEdt;
    QLineEdit* m_stderrPathEdt;
    QLineEdit* m_stdinPathEdt;
    QListWidget* m_argList;
    QTableWidget* m_envList;
    uint32_t m_launchFlags = 0;
};



