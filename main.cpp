#include "MainWindow.h"

#include <QStyleFactory>
#include <QApplication>
#include <kddockwidgets/Config.h>


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    app.setOrganizationName("LaoXing");
    app.setApplicationName("XingDBG");

    qApp->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));


	auto flags = KDDockWidgets::Config::self().flags();
	flags |= KDDockWidgets::Config::Flag_AutoHideSupport;
	flags |= KDDockWidgets::Config::Flag_AllowReorderTabs;
	flags |= KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible;
	flags |= KDDockWidgets::Config::Flag_TabsHaveCloseButton;
	KDDockWidgets::Config::self().setFlags(flags);

    MainWindow mainWindow;
    mainWindow.setWindowTitle("Xing's LLDB GUI");
    mainWindow.showMaximized();

    return app.exec();
}
