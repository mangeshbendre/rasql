#include "settings.h"
#include "ui_settings.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QLineEdit>
#include "dbconnection.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QStandardItem>
#include <QSettings>
#include <QStandardPaths>

Settings::Settings(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    connect(ui->listWidget_Profiles, SIGNAL(currentRowChanged(int)),
            this,SLOT(profileSelected(int)));
    populateDrivers();
    loadSettings();
}

void Settings::profileSelected(int row)
{
    if (row>=0 && ConnectionsMap.contains(ui->listWidget_Profiles->currentItem()->text()))
    {
        ConnectionProfile *profile = ConnectionsMap.value(ui->listWidget_Profiles->currentItem()->text());

        ui->lineEdit_ProfileName->setText(ui->listWidget_Profiles->currentItem()->text());
        ui->lineEdit_HostName->setText(profile->hostName);

        ui->lineEdit_HostName->setText(profile->hostName);
        ui->spinBox_port->setValue(profile->port);
        ui->lineEdit_DBName->setText(profile->databaseName);
        ui->lineEdit_UserName->setText(profile->userName);
        ui->lineEdit_Password->setText(profile->password);

        if (DBDrivers.contains(profile->driver))
        {
            int i=ui->comboBox_Drivers->findText(DBDrivers[profile->driver]);
            ui->comboBox_Drivers->setCurrentIndex(i);
        }
        else
        {
            int i=ui->comboBox_Drivers->findText(profile->driver);
            ui->comboBox_Drivers->setCurrentIndex(i);
        }
        ui->pushButton_ConnectionUpdate->setText(tr("&Update"));
    }
}

void Settings::populateDrivers()
{
    DBDrivers["QDB2"]   = "IBM DB2";
    DBDrivers["QIBASE"] = "Borland InterBase Driver";
    DBDrivers["QMYSQL"] = "MySQL Driver";
    DBDrivers["QOCI"]   = "Oracle Call Interface Driver";
    DBDrivers["QODBC"]  = "ODBC Driver (includes Microsoft SQL Server)";

    DBDrivers["QPSQL"]    = "PostgreSQL Driver";
    DBDrivers["QSQLITE"]  = "SQLite version 3 or above";
    DBDrivers["QSQLITE2"] = "SQLite version 2";
    DBDrivers["QTDS"]     = "Sybase Adaptive Server";

    for (int i = 0; i < QSqlDatabase::drivers().size(); ++i)
    {
        if (DBDrivers.contains(QSqlDatabase::drivers().at(i)))
            ui->comboBox_Drivers->addItem(DBDrivers[QSqlDatabase::drivers().at(i)]);
        else
            ui->comboBox_Drivers->addItem( QSqlDatabase::drivers().at(i));
    }
}

Settings::~Settings()
{
    delete ui;
}


void Settings::getFileName()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Select DB file"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)+
                                                    ui->lineEdit_DBName->text(),
                                                    tr("SQLite DB (*.db);;All Files(*.*)"),
                                                    0,
                                                    QFileDialog::DontConfirmOverwrite);
    if (!fileName.isEmpty())
        ui->lineEdit_DBName->setText(fileName);
}


void Settings::connectionTest()
{

    QString db_driver="";

    for (int i = 0; i < DBDrivers.size(); ++i)
    {
        if (DBDrivers.values().at(i).compare(ui->comboBox_Drivers->currentText())==0)
            db_driver = DBDrivers.keys().at(i);
    }

    if (db_driver.size()==0)
        db_driver = ui->comboBox_Drivers->currentText();

    bool connection_status = DBConnection::connect(db_driver,
                                                   ui->lineEdit_HostName->text(),
                                                   ui->spinBox_port->value(),
                                                   ui->lineEdit_DBName->text(),
                                                   ui->lineEdit_UserName->text(),
                                                   ui->lineEdit_Password->text());
    if (connection_status)
    {
        QMessageBox::information(this,APP_NAME,tr("Connection Success"));
        DBConnection::disconnect();
    }
    else
        QMessageBox::warning(this,APP_NAME,tr("Unable to Connect :- ") + DBConnection::db_error);


}


void Settings::connectionDelete()
{
    if (ui->listWidget_Profiles->currentRow()>=0)
    {
        free(ConnectionsMap.take(ui->listWidget_Profiles->currentItem()->text()));
        ui->listWidget_Profiles->takeItem(ui->listWidget_Profiles->currentRow());
        ((MainWindow *) parentWidget())->connectWindow->loadProfileList();
    }

}

void Settings::connectionUpdate()
{

    if (ui->lineEdit_ProfileName->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this,APP_NAME,tr("Kindly specify a Profile Name"));
        return;
    }

    QString db_driver="";

    for (int i = 0; i < DBDrivers.size(); ++i)
    {
        if (DBDrivers.values().at(i).compare(ui->comboBox_Drivers->currentText())==0)
            db_driver = DBDrivers.keys().at(i);
    }

    if (db_driver.size()==0)
        db_driver = ui->comboBox_Drivers->currentText();


    ConnectionProfile *profile = new ConnectionProfile;

    profile->driver = db_driver;
    profile->hostName = ui->lineEdit_HostName->text();
    profile->port = ui->spinBox_port->value();
    profile->databaseName = ui->lineEdit_DBName->text();
    profile->userName = ui->lineEdit_UserName->text();
    profile->password = ui->lineEdit_Password->text();

    if (ConnectionsMap.contains(ui->lineEdit_ProfileName->text().trimmed()))
    {
        free(ConnectionsMap.take(ui->lineEdit_ProfileName->text().trimmed()));
        ConnectionsMap.insert(ui->lineEdit_ProfileName->text().trimmed(),profile);
    }
    else
    {
        ConnectionsMap.insert(ui->lineEdit_ProfileName->text().trimmed(),profile);

        ui->listWidget_Profiles->clear();
        ui->listWidget_Profiles->addItems(ConnectionsMap.keys());
        ((MainWindow *) parentWidget())->connectWindow->loadProfileList();

    }
    ui->pushButton_ConnectionUpdate->setText(tr("&Update"));
}


void Settings::saveSettings()
{
    QSettings settings(COMPANY_NAME, APP_NAME);
    settings.beginWriteArray("Profiles");
    int count=0;
    settings.clear();

    QMessageBox::information(this,APP_NAME,tr("Saving settings"));


    ValidateSchema = ui->checkBoxValidateSchema->checkState();
    settings.setValue("ValidateSchema", ValidateSchema);

    QMap<QString, ConnectionProfile *>::const_iterator i = ConnectionsMap.constBegin();
    while (i != ConnectionsMap.constEnd()) {
        settings.setArrayIndex(count++);
        settings.setValue("ProfileName",i.key());
        ConnectionProfile *profile = i.value();
        settings.setValue("Driver",profile->driver);
        settings.setValue("HostName",profile->hostName);
        settings.setValue("Port",profile->port);
        settings.setValue("DBName",profile->databaseName);
        settings.setValue("User",profile->userName);
        settings.setValue("Password",profile->password);
        ++i;
    }
    settings.endArray();
    hide();
}

void Settings::loadSettings()
{
    ConnectionsMap.clear();

    QSettings settings(COMPANY_NAME, APP_NAME);
    ValidateSchema = settings.value("ValidateSchema",false).toBool();
    ui->checkBoxValidateSchema->setChecked(ValidateSchema);

    int size = settings.beginReadArray("Profiles");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ConnectionProfile *profile = new ConnectionProfile();
        profile->driver = settings.value("Driver").toString();
        profile->hostName = settings.value("HostName").toString();
        profile->port = settings.value("Port").toInt();
        profile->databaseName = settings.value("DBName").toString();
        profile->userName = settings.value("User").toString();
        profile->password = settings.value("Password").toString();

        QString ProfileName = settings.value("ProfileName").toString();
        ConnectionsMap.insert(ProfileName, profile);

    }
    settings.endArray();

    ui->listWidget_Profiles->clear();
    ui->listWidget_Profiles->addItems(ConnectionsMap.keys());
}

void Settings::on_lineEdit_ProfileName_textChanged(QString a)
{
    if (ConnectionsMap.contains(ui->lineEdit_ProfileName->text().trimmed()))
    {
        ui->pushButton_ConnectionUpdate->setText(tr("&Update"));
    }
    else
    {
        ui->pushButton_ConnectionUpdate->setText(tr("&Insert"));
    }
}
