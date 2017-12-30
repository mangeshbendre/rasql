#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName(COMPANY_NAME);
    app.setApplicationName(APP_NAME);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
