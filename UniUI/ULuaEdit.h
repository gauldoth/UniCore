/*! \file ULuaEdit.h
    \brief 能识别Lua语法的编辑框。
    
    \author uni
    \date 2012-4-8
*/
#ifndef UNIUI_ULUA_EDIT_H
#define UNIUI_ULUA_EDIT_H

#include <QSyntaxHighlighter>
#include <QTextEdit>

namespace uni
{

//! Lua编辑框使用的Highlighter。
class ULuaSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    ULuaSyntaxHighlighter(QTextDocument *parent = 0);

protected:
    void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules_;

    QRegExp commentStartExpression_;
    QRegExp commentEndExpression_;

    QTextCharFormat keywordFormat_;
    QTextCharFormat classFormat_;
    QTextCharFormat singleLineCommentFormat_;
    QTextCharFormat multiLineCommentFormat_;
    QTextCharFormat quotationFormat_;
    QTextCharFormat functionFormat_;
};

//! Lua编辑框。
/*!
    该编辑框用于编辑，显示Lua脚本，功能如下，
    - 能高亮显示语法。
    - 自动补全。
    - 自动缩进。
*/
class ULuaEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit ULuaEdit(QWidget *parent = 0);
    ~ULuaEdit();
};

}//namespace uni

#endif//UNIUI_ULUA_EDIT_H

