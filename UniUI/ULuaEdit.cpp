#include "ULuaEdit.h"

namespace uni
{

ULuaSyntaxHighlighter::ULuaSyntaxHighlighter( QTextDocument *parent /*= 0*/ )
:QSyntaxHighlighter(parent)
{
    HighlightingRule rule;
    keywordFormat_.setForeground(Qt::darkBlue);
    keywordFormat_.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns<<"\\bfunction\\b"<<"\\bend\\b";
    foreach(const QString &pattern,keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat_;
        highlightingRules_.append(rule);
    }

    quotationFormat_.setForeground(Qt::darkGreen);
    rule.pattern = QRegExp("\".*\"");
    rule.format = quotationFormat_;
    highlightingRules_.append(rule);

    functionFormat_.setFontItalic(true);
    functionFormat_.setForeground(Qt::blue);
    rule.pattern = QRegExp("\\b\\w+(?=\\()");
    rule.format = functionFormat_;
    highlightingRules_.append(rule);

    singleLineCommentFormat_.setForeground(Qt::red);
    rule.pattern = QRegExp("--[^\n]*");
    rule.format = singleLineCommentFormat_;
    highlightingRules_.append(rule);

    multiLineCommentFormat_.setForeground(Qt::red);

    commentStartExpression_ = QRegExp("--\\[\\[");
    commentEndExpression_ = QRegExp("\\]\\]");
    
}

void ULuaSyntaxHighlighter::highlightBlock( const QString &text )
{
    foreach(const HighlightingRule &rule,highlightingRules_)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while(index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index,length,rule.format);
            index = expression.indexIn(text,index+length);
        }
    }
}

ULuaEdit::ULuaEdit( QWidget *parent /*= 0*/ )
:QTextEdit(parent)
{
    ULuaSyntaxHighlighter *highlighter = new ULuaSyntaxHighlighter(document());
}

ULuaEdit::~ULuaEdit()
{

}

}//namespace uni