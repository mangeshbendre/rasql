#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include "connect.h"
#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE


class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat operatorFormat;
    QTextCharFormat sqlFormat;
    QTextCharFormat relationFormat;
    QTextCharFormat attributeFormat;
    QTextCharFormat singleLineCommentFormat;

};


#endif
