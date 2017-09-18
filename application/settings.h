#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QMap>
#include <QStandardItemModel>

class ConnectionProfile : public QObject {
public:
    QString driver;
    QString hostName;
    int port;
    QString databaseName;
    QString userName;
    QString password;
};

namespace Ui {
    class SettingsWindow;
}

class Settings : public QDialog {
    Q_OBJECT


private:
    QMap<QString, QString> DBDrivers;
    void populateDrivers();



private slots:
    void on_lineEdit_ProfileName_textChanged(QString );
    void connectionUpdate();
    void connectionTest();
    void connectionDelete();
    void getFileName();
    void saveSettings();
    void loadSettings();
    void profileSelected(int);

public:
    Settings(QWidget *parent = 0);
    ~Settings();
    QMap<QString, ConnectionProfile *> ConnectionsMap;
    bool ValidateSchema;

    Ui::SettingsWindow *ui;



};

#endif // SETTINGS_H
