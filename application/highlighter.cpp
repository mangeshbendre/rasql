#include <QtGui>
#include "dbconnection.h"
#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    //Rules for numbers (as in conditions)
    QTextCharFormat numberFormat;
    numberFormat.setForeground(Qt::magenta);
    rule.pattern = QRegExp("\\b\\d+\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    //Rules for strings (as in conditions)
    QTextCharFormat stringFormat;
    stringFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("\\'\\S+\\'");
    rule.format = stringFormat;
    highlightingRules.append(rule);


    //Rules for operators
    operatorFormat.setForeground(Qt::blue);
    //operatorFormat.setFontWeight(QFont::Bold);
    QStringList operatorPatterns;
    operatorPatterns << "\\x03C3" << "\\x03C0" << "\\x222A"
                    << "\\x2229" << "\\x00D7" << "\\x22C8"
                    << "\\x22C9" << "\\x27D7" << "\\x27D5"
                    << "\\x27D6" << "\\x03C1" << "\\x2212"
                    << "\\x2190";
    foreach (const QString &pattern, operatorPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = operatorFormat;
        highlightingRules.append(rule);
    }

    //Rules for SQL keywords
    sqlFormat.setForeground(Qt::darkYellow);
    //operatorFormat.setFontWeight(QFont::Bold);
    QStringList sqlPatterns;
    sqlPatterns << "\\bSELECT\\b" << "\\bFROM\\b" << "\\bWHERE\\b"
                    << "\\bAND\\b" << "\\bOR\\b" << "\\bIN\\b"
                    << "\\bUNION\\b" << "\\bMINUS\\b" << "\\bJOIN\\b"
                    << "\\bCREATE\\b" << "\\bDELETE\\b" << "\\bALTER\\b"
                    << "\\bTABLE\\b" << "\\bNATURAL JOIN\\b" << "\\bON\\b"
                    << "\\bEXCEPT\\b" <<"\\bNATURAL RIGHT JOIN\\b" << "\\bNATURAL LEFT JOIN\\b"
                    << "\\bNATURAL OUTER JOIN\\b" << "\\bINTERSECT\\b" << "\\bDISTINCT\\b"
                    << "\\bCREATE\\b" << "\\bDROP\\b" << "\\bINSERT\\b" << "\\bDELETE\\b"
                    << "\\bOUTER JOIN\\b" <<"\\bRIGHT JOIN\\b" << "\\bLEFT JOIN\\b"
                    << "\\bAS\\b" << "\\bNATURAL LEFT JOIN\\b";
    foreach (const QString &pattern, sqlPatterns) {
        rule.pattern = QRegExp(pattern, Qt::CaseInsensitive, QRegExp::RegExp);
        //rule.pattern = QRegExp(pattern);
        rule.format = sqlFormat;
        highlightingRules.append(rule);
    }

    //Rules for Relations
    QStringList tableList = DBConnection::db.tables();
    QStringList::const_iterator constIterator;
    relationFormat.setForeground(Qt::darkGreen);
    relationFormat.setFontWeight(QFont::Bold);
    attributeFormat.setForeground(Qt::darkMagenta);

   for (constIterator = tableList.constBegin(); constIterator != tableList.constEnd();
                ++constIterator)
         {
             rule.pattern = QRegExp("\\b" + *constIterator + "\\b", Qt::CaseInsensitive, QRegExp::RegExp);
             rule.format = relationFormat;
             highlightingRules.append(rule);

        }

   //Rules for attributes
   QStringList attributeList = DBConnection::fields.fieldNames();
   attributeFormat.setForeground(Qt::darkMagenta);

  for (constIterator = attributeList.constBegin(); constIterator != attributeList.constEnd();
               ++constIterator)
        {
            rule.pattern = QRegExp("\\b" + *constIterator + "\\b",Qt::CaseInsensitive, QRegExp::RegExp);
            rule.format = attributeFormat;
            highlightingRules.append(rule);

       }

  //Comments have the highest priority and should be highlighted at the end
  singleLineCommentFormat.setForeground(Qt::darkRed);
  rule.pattern = QRegExp("--[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }

    setCurrentBlockState(0);
}

