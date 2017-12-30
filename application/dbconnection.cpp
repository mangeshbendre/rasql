#include "dbconnection.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlDriver>
#include <QMessageBox>

DBFields::DBFields()
{

}

DBConnection::DBConnection()
{

}

bool DBConnection::connected = false;
QString DBConnection::db_error;

QSqlDatabase DBConnection::db;

QSqlQuery *DBConnection::query=NULL;
QSqlQueryModel *DBConnection::model=NULL;
bool DBConnection::querySizeFeature=false;
DBFields DBConnection::fields;


/* Connect using the settings */
bool DBConnection::connect(QString driver, QString hostName, int port, QString dbName, QString userName, QString password)
{
    if (connected)
        disconnect();

    //DBFields = new struct fields
    db = QSqlDatabase::addDatabase(driver);
    db.setHostName(hostName);
    db.setPort(port);
    db.setDatabaseName(dbName);
    db.setUserName(userName);
    db.setPassword(password);
    bool ok = db.open();
    if (ok)
    {
        connected=true;
        query = new QSqlQuery();
        model = new QSqlQueryModel();
        querySizeFeature = db.driver()->hasFeature(QSqlDriver::QuerySize);

    }
    else
        db_error= db.lastError().text();

    retrieveAttrs();
    return ok;

}

bool DBConnection::disconnect()
{

    QSqlDatabase db = QSqlDatabase::database();
    model->clear();
    delete model;
    query->finish();
    delete query;
    db.close();
    QSqlDatabase::removeDatabase(db.connectionName());
    return true;
}

bool DBConnection::retrieveAttrs()
{

    QString SQL1 = "select * from ";
    QString SQL2 = " LIMIT 0, 0";
    QString finalSQL;
    QStringList tableList = db.tables();

    QStringList::const_iterator constIterator;

    fields.clear();
    for (constIterator = tableList.constBegin(); constIterator != tableList.constEnd();
                ++constIterator)
         {
            finalSQL = SQL1 + *constIterator + SQL2;
            QSqlQuery q(finalSQL);
            QSqlRecord rec = q.record();

            for (int i=0; i<rec.count(); i++){
              fields.append(*constIterator, rec.fieldName(i));
              }


         }

    return true;
}
