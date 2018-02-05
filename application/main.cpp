#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(ra);
    app.setOrganizationName(COMPANY_NAME);
    app.setApplicationName(APP_NAME);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
