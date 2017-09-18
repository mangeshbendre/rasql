#include "richraparser.h"
#include <QMessageBox>
#include <QTextDocument>
#include <QtGui>
#include "dbconnection.h"
#include "highlighter.h"
#include "../system/RAParser.h"

RichRAParser::RichRAParser(QTextEdit *a)
{
    raparser = new RAParser();
    parentEditor = a->document();
    parent=a;
    ParseRule rule;

    //Rules for rich RA parsing

    QStringList parsePatterns;
    QStringList parsePatternReplacer;
    parsePatterns   << "\\x03C3" << "\\x03C0" << "\\x222A"
                    << "\\x2229" << "\\x00D7" << "\\x22C8"
                    << "\\x22C9" << "\\x27D7" << "\\x27D5"
                    << "\\x27D6" << "\\x03C1" << "\\x2212"
                    << "\\x2190" << "//[^\n]*";

    parsePatternReplacer << "SELECTION" << "PROJECTION" <<"UNION"
                    << "INTERSECTION" << "PRODUCT" << "JOIN"
                    << "SEMI_JOIN" << "OUTER_JOIN" << "LEFT_JOIN"
                    << "RIGHT_JOIN" << "RENAME" << "MINUS"
                    << "=" <<"" ;

    for (int i = 0; i < parsePatterns.count(); i++) {
        rule.pattern = QRegExp(parsePatterns.at(i));
        rule.replacer = parsePatternReplacer.at(i);
        parseRules.append(rule);
    }

    //suffixStartExpression = QRegExp("vertical-align:sub;\">");
    suffixStartExpression = QRegExp("vertical-align:sub;\">");
    suffixEndExpression = QRegExp("</span>");
}

int RichRAParser::parseRA(QString &plainText, QString &sqlQuery, QString &errorText)
{
    QTextDocument *temp = new QTextDocument();
    QString initText;

    //Get the text portion to be parsed
    QTextCursor a = parent->textCursor();
    if(a.hasSelection())
    {
       initText  = a.selection().toHtml();
    }
    else
    {
        temp = parentEditor->clone(0);
        initText = temp->toHtml();
    }


    //Convert to plain text
    int startIndex = suffixStartExpression.indexIn(initText);
    while(startIndex >=0) {
        int endIndex = suffixEndExpression.indexIn(initText, startIndex);
        int suffixStartPosition = startIndex+QString("vertical-align:sub;\">").length();
        QString suffix = initText.mid(suffixStartPosition, endIndex-suffixStartPosition);
        if (suffix.trimmed().length())
            initText.replace(suffixStartPosition, suffix.length(), "{"+suffix+"}" );
        startIndex = suffixStartExpression.indexIn(initText, endIndex);
    }

    temp->setHtml(initText);
    initText = temp->toPlainText();


    //Replace the symbols with plain text operators
    plainText = initText;

    foreach (const ParseRule &rule, parseRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(plainText);
        while (index >= 0) {
            int length = expression.matchedLength();
            plainText.replace(index, length, rule.replacer);

            index = expression.indexIn(plainText, index + rule.replacer.length());
        }
    }

    if ( plainText.trimmed().length() < 1)
    {
        errorText ="No valid string to parse!";
        return -1;
    }

    //Call backend parser here
   // int ret = 0;
    int ret=raparser->parseRA(plainText.toStdString());

    if (ret)
    {
        errorText = QString::fromStdString(raparser->ErrorMessage);
        sqlQuery = plainText;
    }
    else
    {
        sqlQuery = QString::fromStdString(raparser->getResult());

    }
    return ret;
}

int  RichRAParser::updateSchema(wstring relation, list<wstring> attributes)
{
    return raparser->addRelationAttributes(relation, attributes);

}

int RichRAParser::clearSchema()
{
    return raparser->clearRelationAttributes();
}
