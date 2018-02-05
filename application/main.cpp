#include "mainwindow.h"
#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(ra);

    int id = QFontDatabase::addApplicationFont(":/Apple Symbols Regular.ttf");

    app.setOrganizationName(COMPANY_NAME);
    app.setApplicationName(APP_NAME);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
