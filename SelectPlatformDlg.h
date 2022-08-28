//
// Created by spring on 2022/8/28.
//

#pragma once
#include <QDialog>

class QComboBox;
class QLineEdit;

class SelectPlatformDlg : public QDialog
{
    Q_OBJECT
public:
    explicit SelectPlatformDlg(QWidget* parent);

private:
    void setupPlatforms();

    void accept() override;

private:
    QComboBox* m_platformCombo;
    QLineEdit* m_connectUrlEdt;
};



