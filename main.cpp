#include "MainWindow.h"

#include <QStyleFactory>
#include <QApplication>
#include <kddockwidgets/Config.h>


int main(int argc, char **argv)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app(argc, argv);

    app.setOrganizationName("LaoXing");
    app.setApplicationName("XingDBG");

    qApp->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));


	auto flags = KDDockWidgets::Config::self().flags();
	flags |= KDDockWidgets::Config::Flag_AutoHideSupport;
	KDDockWidgets::Config::self().setFlags(flags);

    MainWindow mainWindow;
    mainWindow.setWindowTitle("Xing's LLDB GUI");
    mainWindow.showMaximized();

    return app.exec();
}
