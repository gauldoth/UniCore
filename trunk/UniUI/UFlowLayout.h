/*! \file UFlowLayout.h
    \brief 流布局。
    代码来自Qt例子。

    \author uni
    \date 2012-5-14
*/
#ifndef UNIUI_UFLOW_LAYOUT_H
#define UNIUI_UFLOW_LAYOUT_H

#include <QLayout>
#include <QRect>
#include <QStyle>
#include <QWidgetItem>

#define AUTO_LINK_LIB_NAME "UniUI"
#include "../UniCore/AutoLink.h"

namespace uni
{

class UFlowLayout : public QLayout
{
public:
    UFlowLayout(QWidget *parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    UFlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~UFlowLayout();

    void addItem(QLayoutItem *item);
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const;
    bool hasHeightForWidth() const;
    int heightForWidth(int) const;
    int count() const;
    QLayoutItem *itemAt(int index) const;
    QSize minimumSize() const;
    void setGeometry(const QRect &rect);
    QSize sizeHint() const;
    QLayoutItem *takeAt(int index);

private:
    int doLayout(const QRect &rect, bool testOnly) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem *> itemList;
    int m_hSpace;
    int m_vSpace;
};

}//namespace uni

#endif//UNIUI_UFLOW_LAYOUT_H
