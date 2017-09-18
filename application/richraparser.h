#ifndef RICHRAPARSER_H
#define RICHRAPARSER_H

#include "connect.h"
#include <QHash>
#include <QTextEdit>
#include <list>
#include "../system/RAParser.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class RichRAParser
{
public:
    RichRAParser(QTextEdit *parent = 0);

    int parseRA(QString &plainText, QString &sqlQuery, QString &errorText);
    int updateSchema(wstring relation, list<wstring> attributes);
    int clearSchema();


private:
    QTextEdit *parent;
    QTextDocument *parentEditor;
    RAParser *raparser;
    struct ParseRule
    {
        QRegExp pattern;
        QString replacer;
    };
    QVector<ParseRule> parseRules;
    QRegExp suffixStartExpression;
    QRegExp suffixEndExpression;
};

#endif // RICHRAPARSER_H
