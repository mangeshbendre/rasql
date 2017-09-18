#include "connect.h"
#include "settings.h"
#include "ui_connect.h"
#include "dbconnection.h"
#include "mainwindow.h"
#include <QStandardItem>
#include <QSettings>
#include <QMessageBox>


Connect::Connect(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ConnectWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton_Settings, SIGNAL(clicked()), (MainWindow *) parent, SLOT(displaySettings()));
    loadProfileList();
}


Connect::~Connect()
{
    delete ui;
}


void Connect::loadProfileList()
{
    ui->listWidget_Profiles->clear();
    ui->listWidget_Profiles->addItems(((MainWindow *) parentWidget())->settings->ConnectionsMap.keys());
}

void Connect::on_pushButton_Connect_clicked()
{
    if (ui->listWidget_Profiles->currentRow()<0)
    {
        QMessageBox::information(this,APP_NAME,tr("Select a profile to Connect."));
        return;
    }
    ConnectionProfile *profile = (((MainWindow *) parentWidget())->settings->ConnectionsMap).value(ui->listWidget_Profiles->currentItem()->text());
    if (!profile)
        return;

    bool connection_status = DBConnection::connect(profile->driver,
                                                   profile->hostName,
                                                   profile->port,
                                                   profile->databaseName,
                                                   profile->userName,
                                                   profile->password);
    if (connection_status)
    {
        QMessageBox::information(this,APP_NAME,tr("Connection Success"));
        hide();
        ((MainWindow *)parentWidget())->populateRelations();
    }
    else
        QMessageBox::warning(this,APP_NAME,tr("Unable to Connect :- ") + DBConnection::db_error);


}


