//
// Created by xing on 2022/8/19.
//

#include "OpenExeDlg.h"
#include <QtWidgets>
#include "App.h"
#include "LLDBCore.h"


OpenExeDlg::OpenExeDlg(QWidget *parent)
        : QDialog(parent)
{
    setWindowTitle(tr("Launch executable"));

	m_exePathEdt = new QLineEdit;
    auto chooseExeBtn = new QPushButton("...");
    m_workDirEdt = new QLineEdit;
    auto chooseWorkDirBtn = new QPushButton("...");
    m_stdoutPathEdt = new QLineEdit;
    auto chooseStdoutBtn = new QPushButton("...");
    m_stderrPathEdt = new QLineEdit;
    auto chooseStderrBtn = new QPushButton("...");
    m_stdinPathEdt = new QLineEdit;
    auto chooseStdinBtn = new QPushButton("...");
    m_argList = new QListWidget;
    auto addArgBtn = new QPushButton(tr("Add"));
    auto removeArgBtn = new QPushButton(tr("Remove"));
    auto editArgBtn = new QPushButton(tr("Edit"));
    auto moveUpArgBtn = new QPushButton(tr("Move up"));
    auto moveDownArgBtn = new QPushButton(tr("Move down"));

    m_envList = new QTableWidget(0, 2);
    m_envList->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Value"));
    auto addEnvBtn = new QPushButton(tr("Add"));
    auto removeEnvBtn = new QPushButton(tr("Remove"));

    auto launchOptionsBox = new QGroupBox;
    auto glay = new QGridLayout(launchOptionsBox);
    auto addFlagChk = [=, this](QString const &text, lldb::LaunchFlags flag, QString const &tip, int x, int y)
    {
        auto chk = new QCheckBox(text);
        chk->setToolTip(tip);
        connect(chk, &QCheckBox::stateChanged, this, [=, this](int state)
        {
            if (state != 0)
            {
                m_launchFlags |= flag;
            }
            else
            {
                m_launchFlags ^= flag;
            }
        });
        glay->addWidget(chk, x, y);
    };
    addFlagChk(tr("Exec"), lldb::eLaunchFlagExec,
               tr("Exec when launching and turn the calling process into a new process"), 0, 0);
    addFlagChk(tr("Debug"), lldb::eLaunchFlagDebug,
               tr("Stop as soon as the process launches to allow the process to be debugged"), 0, 1);
    addFlagChk(tr("Stop at entry"), lldb::eLaunchFlagStopAtEntry,
               tr("Stop at the program entry point instead of auto-continuing"
                  " when launching or attaching at entry point"), 0, 2);
    addFlagChk(tr("Disable ASLR"), lldb::eLaunchFlagDisableASLR,
               tr("Disable Address Space Layout Randomization"), 0, 3);
    addFlagChk(tr("Disable STDIO"), lldb::eLaunchFlagDisableSTDIO,
               tr("Disable stdio for inferior process (e.g. for a GUI app)"), 1, 0);
    addFlagChk(tr("Launch in TTY"), lldb::eLaunchFlagLaunchInTTY,
               tr("Launch the process in a new TTY if supported by the host"), 1, 1);
    addFlagChk(tr("Launch in shell"), lldb::eLaunchFlagLaunchInShell,
               tr("Launch the process inside a shell to get shell expansion"), 1, 2);
    addFlagChk(tr("Launch in separate process group"), lldb::eLaunchFlagLaunchInSeparateProcessGroup,
               tr("Launch the process in a separate process group"
                  " If you are going to hand the process off (e.g. to debugserver)"), 1, 3);
    addFlagChk(tr("Dont set exit status"), lldb::eLaunchFlagDontSetExitStatus,
               tr("Set this flag so lldb & the handee don't race to set its exit status"), 2, 0);
    addFlagChk(tr("Detach on error"), lldb::eLaunchFlagDetachOnError,
               tr("If set, then the client stub should detach rather than killing the debugee"
                  " if it loses connection with lldb."), 2, 1);
    addFlagChk(tr("Shell expand arguments"), lldb::eLaunchFlagShellExpandArguments,
               tr("Perform shell-style argument expansion"), 2, 2);
    addFlagChk(tr("CloseTTY on exit"), lldb::eLaunchFlagCloseTTYOnExit,
               tr("Close the open TTY on exit"), 2, 3);
    addFlagChk(tr("Inherit TCC from parent"), lldb::eLaunchFlagInheritTCCFromParent,
               tr("Don't make the inferior responsible for its own TCC"
                  " permissions but instead inherit them from its parent."), 3, 0);

    auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);


    connect(chooseExeBtn, &QPushButton::clicked, this, [=, this]
    {
        auto path = QFileDialog::getOpenFileName(this, tr("Choose an executable"), {},
                                                 "Executable files (*.exe);;All files (*)");

        if (path.isEmpty()) return;
        m_exePathEdt->setText(path);
        if (m_workDirEdt->text().isEmpty())
            m_workDirEdt->setText(QFileInfo(path).absoluteDir().absolutePath());
    });

    connect(chooseWorkDirBtn, &QPushButton::clicked, this, [=, this]
    {
        auto path = QFileDialog::getExistingDirectory(this, tr("Choose a directory"));
        if (path.isEmpty()) return;
        m_workDirEdt->setText(path);
    });

    connect(addArgBtn, &QPushButton::clicked, this, [=, this]
    {
        QDialog dlg(this);
        dlg.setWindowTitle(tr("Add arg"));
        auto argEdt = new QLineEdit;
        auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        auto lay = new QVBoxLayout(&dlg);
        lay->addWidget(argEdt);
        lay->addWidget(btnBox);
        connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() != QDialog::Accepted)
            return;

        m_argList->addItem(argEdt->text());
    });

    connect(removeArgBtn, &QPushButton::clicked, this, [=, this]
    {
        qDeleteAll(m_argList->selectedItems());
    });

    connect(editArgBtn, &QPushButton::clicked, this, [=, this]
    {
        auto currItem = m_argList->currentItem();
        if (!currItem)
        {
            QMessageBox::information(this, tr("Tip"), tr("Please select an item to edit"));
            return;
        }

        QDialog dlg(this);
        dlg.setWindowTitle(tr("Edit arg"));
        auto argEdt = new QLineEdit(currItem->text());
        auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        auto lay = new QVBoxLayout(&dlg);
        lay->addWidget(argEdt);
        lay->addWidget(btnBox);
        connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() != QDialog::Accepted)
            return;

        currItem->setText(argEdt->text());
    });

    connect(moveUpArgBtn, &QPushButton::clicked, this, [=, this]
    {
        auto currItem = m_argList->currentItem();
        if (!currItem)
        {
            QMessageBox::information(this, tr("Tip"), tr("Please select an item to move up"));
            return;
        }

        int currIndex = m_argList->row(currItem);

        auto *prev = m_argList->item(m_argList->row(currItem) - 1);
        int prevIndex = m_argList->row(prev);

        auto *temp = m_argList->takeItem(prevIndex);
        m_argList->insertItem(prevIndex, currItem);
        m_argList->insertItem(currIndex, temp);
    });

    connect(moveDownArgBtn, &QPushButton::clicked, this, [=, this]
    {
        auto currItem = m_argList->currentItem();
        if (!currItem)
        {
            QMessageBox::information(this, tr("Tip"), tr("Please select an item to move down"));
            return;
        }

        int currIndex = m_argList->row(currItem);

        auto *next = m_argList->item(m_argList->row(currItem) + 1);
        int nextIndex = m_argList->row(next);

        auto *temp = m_argList->takeItem(nextIndex);
        m_argList->insertItem(currIndex, temp);
        m_argList->insertItem(nextIndex, currItem);
    });

    connect(addEnvBtn, &QPushButton::clicked, this, [=, this]
    {
        QDialog dlg(this);
        dlg.setWindowTitle(tr("Add env"));
        auto nameEdt = new QLineEdit;
        auto valueEdt = new QLineEdit;
        auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        auto lay = new QFormLayout(&dlg);
        lay->addRow(tr("Name:"), nameEdt);
        lay->addRow(tr("Value:"), valueEdt);
        lay->addRow(btnBox);
        connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
        if (dlg.exec() != QDialog::Accepted)
            return;

        if (nameEdt->text().isEmpty())
        {
            QMessageBox::information(this, tr("Tip"), tr("name cannot be empty"));
            return;
        }

        int rowNum = m_envList->rowCount();
        m_envList->setRowCount(rowNum+ 1);
        m_envList->setItem(rowNum, 0, new QTableWidgetItem(nameEdt->text()));
        m_envList->setItem(rowNum, 1, new QTableWidgetItem(valueEdt->text()));
    });


    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);


    auto flay = new QFormLayout(this);
	auto hlay = new QHBoxLayout;
    hlay->addWidget(m_exePathEdt, 1);
    hlay->addWidget(chooseExeBtn);
    flay->addRow(tr("Executable:"), hlay);
    hlay = new QHBoxLayout;
    hlay->addWidget(m_workDirEdt, 1);
    hlay->addWidget(chooseWorkDirBtn);
    flay->addRow(tr("Working dir:"), hlay);
    hlay = new QHBoxLayout;
    hlay->addWidget(m_stdoutPathEdt, 1);
    hlay->addWidget(chooseStdoutBtn);
    flay->addRow(tr("Stdout:"), hlay);
    hlay = new QHBoxLayout;
    hlay->addWidget(m_stderrPathEdt, 1);
    hlay->addWidget(chooseStderrBtn);
    flay->addRow(tr("Stderr:"), hlay);
    hlay = new QHBoxLayout;
    hlay->addWidget(m_stdinPathEdt, 1);
    hlay->addWidget(chooseStdinBtn);
    flay->addRow(tr("Stdin:"), hlay);

    auto vlay = new QVBoxLayout;
    vlay->addWidget(m_argList, 1);
    hlay = new QHBoxLayout;
    vlay->addLayout(hlay);
    hlay->addWidget(addArgBtn);
    hlay->addWidget(removeArgBtn);
    hlay->addWidget(editArgBtn);
    hlay->addWidget(moveUpArgBtn);
    hlay->addWidget(moveDownArgBtn);
    flay->addRow(tr("Arguments:"), vlay);

    vlay = new QVBoxLayout;
    vlay->addWidget(m_envList, 1);
    hlay = new QHBoxLayout;
    vlay->addLayout(hlay);
    hlay->addWidget(addEnvBtn, 0, Qt::AlignCenter);
    hlay->addWidget(removeEnvBtn, 0, Qt::AlignCenter);
    flay->addRow(tr("Environments:"), vlay);
    flay->addRow(tr("Launch options:"), launchOptionsBox);
    flay->addRow(btnBox);
}

QString OpenExeDlg::exePath()
{
    return m_exePathEdt->text();
}

QString OpenExeDlg::workingDir()
{
    return m_workDirEdt->text();
}

void OpenExeDlg::accept()
{
    if (exePath().isEmpty())
    {
        QMessageBox::information(this, tr("Tip"), tr("Executable path cannot bo empty."));
        return;
    }
    if (workingDir().isEmpty())
    {
        QMessageBox::information(this, tr("Tip"), tr("Working directory path cannot bo empty."));
        return;
    }
    QDialog::accept();
}

QString OpenExeDlg::stdoutPath()
{
    return m_stdoutPathEdt->text();
}

QString OpenExeDlg::stderrPath()
{
    return m_stderrPathEdt->text();
}

QString OpenExeDlg::stdinPath()
{
    return m_stdinPathEdt->text();
}

QStringList OpenExeDlg::argList()
{
    QStringList l;
    for (int i = 0; i < m_argList->count(); ++i)
    {
        l.append(m_argList->item(i)->text());
    }

    return l;
}

QStringList OpenExeDlg::envList() {
    QStringList l;
    for (int i = 0; i < m_envList->rowCount(); ++i)
    {
        auto name = m_envList->item(i, 0)->text();
        auto value = m_envList->item(i, 1)->text();
        l.append(name + "=" + value);   // XXX: lldb需要 name=value 格式的字符串，如果name或value中有=应该怎么处理?
    }

    return l;
}

