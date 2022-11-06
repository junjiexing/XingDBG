#include "AboutDlg.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QIcon>


AboutDlg::AboutDlg(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle(tr("About XingDBG"));
	auto logoLab = new QLabel;
	logoLab->setPixmap(QIcon(":/icon.ico").pixmap(128, 128));
	auto thanksLab= new QLabel;
	thanksLab->setText("Icons: <a href='https://github.com/saixmh'>LinGo</a>");
	thanksLab->setTextFormat(Qt::RichText);
	thanksLab->setTextInteractionFlags(Qt::TextBrowserInteraction);
	thanksLab->setOpenExternalLinks(true);
	auto blogLab = new QLabel;
	blogLab->setText("blog: <a href='https://xing.re'>https://xing.re</a>");
	blogLab->setTextFormat(Qt::RichText);
	blogLab->setTextInteractionFlags(Qt::TextBrowserInteraction);
	blogLab->setOpenExternalLinks(true);
	auto githubLab = new QLabel;
	githubLab->setText("github: <a href='https://github.com/junjiexing/XingDBG'>https://github.com/junjiexing/XingDBG</a>");
	githubLab->setTextFormat(Qt::RichText);
	githubLab->setTextInteractionFlags(Qt::TextBrowserInteraction);
	githubLab->setOpenExternalLinks(true);

	auto lay = new QVBoxLayout(this);
	lay->addWidget(logoLab, 0, Qt::AlignCenter);
	lay->addWidget(thanksLab, 0, Qt::AlignCenter);
	lay->addWidget(blogLab, 0, Qt::AlignCenter);
	lay->addWidget(githubLab, 0, Qt::AlignCenter);
}

AboutDlg::~AboutDlg()
{
}
