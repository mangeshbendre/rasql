#ifndef CONNECT_H
#define CONNECT_H

#include <QDialog>
#include "settings.h"


namespace Ui {
    class ConnectWindow;
}

class Connect : public QDialog {
    Q_OBJECT

private:
    QMap<QString, ConnectionProfile *> ConnectionsMap;


public:
    Connect(QWidget *parent = 0);
    ~Connect();
    void loadProfileList();
    Ui::ConnectWindow *ui;


private slots:
    void on_pushButton_Connect_clicked();
};

#endif // CONNECT_H
