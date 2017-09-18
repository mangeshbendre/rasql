#ifndef DBCONNECTION_H
#define DBCONNECTION_H

#include <QString>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlDatabase>
#include <QStringList>


class DBFields{
public:
    QStringList tables;
    QStringList fields;
    DBFields();
    void append(QString tableName, QString fieldName){
        tables.append(tableName);
        fields.append(fieldName);
    }
    void clear(){
        tables.clear()   ;
        fields.clear();
    }
    QStringList fieldNames(QString curTableName){
        QStringList curFieldNames;
        for (int i =0; i < tables.count(); i ++)
        {
            if (tables.at(i) == curTableName){
                curFieldNames.append(fields.at(i));
            }

        }
        return curFieldNames;
    }
    QStringList fieldNames(){
        return fields;
    }
};

class DBConnection
{


private:

public:
    static bool connected;
    static QString db_error;

    static QSqlQuery *query;
    static QSqlQueryModel *model;
    static bool querySizeFeature;
    static QSqlDatabase db;
    static DBFields fields;
    DBConnection();

    static bool connect(QString driver, QString hostName, int port, QString dbName, QString userName, QString password);
    static bool disconnect();
    static bool retrieveAttrs();

};

#endif // DBCONNECTION_H
