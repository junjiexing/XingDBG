//
// Created by spring on 2022/8/28.
//

#include "SelectPlatformDlg.h"
#include <QtWidgets>
#include "App.h"
#include "LLDBCore.h"


SelectPlatformDlg::SelectPlatformDlg(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Change platform"));

    m_platformCombo = new QComboBox;
    auto descriptionLab = new QLabel;
    connect(m_platformCombo, &QComboBox::currentIndexChanged, this, [=, this](int index)
    {
        auto description = m_platformCombo->itemData(index).toString();
        descriptionLab->setText(description);
    });
    setupPlatforms();

    m_connectUrlEdt = new QLineEdit;

    auto btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(btnBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto lay = new QFormLayout(this);
    lay->addRow(tr("Platform:"), m_platformCombo);
    lay->addRow(tr("Description:"), descriptionLab);
    lay->addRow(tr("Connect url:"), m_connectUrlEdt);
    lay->addRow(btnBox);
}

void SelectPlatformDlg::setupPlatforms()
{
    char buffer[1000];	// shit
    for (int i = 0; ; ++i)
    {
        auto p = app()->getDebugger().GetAvailablePlatformInfoAtIndex(i);
        if (!p) return;

        p.GetValueForKey("name").GetStringValue(buffer, 1000);
        QString name = buffer;
        p.GetValueForKey("description").GetStringValue(buffer, 1000);
        QString description = buffer;

        m_platformCombo->addItem(name, description);
    }
}

void SelectPlatformDlg::accept()
{
    // XXX: 代码位置？
    auto platformName = m_platformCombo->currentText();
    auto url = m_connectUrlEdt->text();
    lldb::SBPlatform platform(platformName.toLocal8Bit());
    if (!platform.IsValid())
    {
        QMessageBox::warning(this, tr("Error"), QString("Invalid platform name: ") + platformName);
        return;
    }

    if (!url.isEmpty())
    {
        lldb::SBPlatformConnectOptions opt(url.toLocal8Bit());
        auto err = platform.ConnectRemote(opt);
        if (err.Fail())
        {
            QMessageBox::warning(this, tr("Error"), tr("Platform connect error: ") + err.GetCString());
            return;
        }
    }

    app()->getDebugger().SetSelectedPlatform(platform);
    app()->i(tr("Change platform success"));

    QDialog::accept();
}
