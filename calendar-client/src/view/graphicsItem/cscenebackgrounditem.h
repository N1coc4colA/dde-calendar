/*
   * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     chenhaifeng <chenhaifeng@uniontech.com>
   *
   * Maintainer: chenhaifeng <chenhaifeng@uniontech.com>
   *
   * This program is free software: you can redistribute it and/or modify
   * it under the terms of the GNU General Public License as published by
   * the Free Software Foundation, either version 3 of the License, or
   * any later version.
   *
   * This program is distributed in the hope that it will be useful,
   * but WITHOUT ANY WARRANTY; without even the implied warranty of
   * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   * GNU General Public License for more details.
   *
   * You should have received a copy of the GNU General Public License
   * along with this program.  If not, see <http://www.gnu.org/licenses/>.
   */
#ifndef CSCENEBACKGROUNDITEM_H
#define CSCENEBACKGROUNDITEM_H

#include "cfocusitem.h"

#include <QDate>

class CSceneBackgroundItem : public CFocusItem
{
    Q_OBJECT
public:
    //属于哪个视图的背景
    enum ItemOnView {
        OnMonthView,
        OnWeekView,
        OnDayView
    };

public:
    explicit CSceneBackgroundItem(ItemOnView view, QGraphicsItem *parent = nullptr);
    //设置下一个item focus状态并获取下一个Item
    CFocusItem *setNextItemFocusAndGetNextItem() override;
    //更新在此背景上显示的item
    void updateShowItem();
    //获取在此背景上显示item的数目
    int getShowItemCount();
    //设置背景编号
    void setBackgroundNum(int num);
    //获取背景编号
    int getBackgroundNum() const;
    //设置item是否获取focus
    void setItemFocus(bool isFocus) override;
    //恢复初始状态
    void initState();
    //获取当前焦点的item
    CFocusItem *getFocusItem();

    CSceneBackgroundItem *getLeftItem() const;
    void setLeftItem(CSceneBackgroundItem *leftItem);

    CSceneBackgroundItem *getRightItem() const;
    void setRightItem(CSceneBackgroundItem *rightItem);

    CSceneBackgroundItem *getUpItem() const;
    void setUpItem(CSceneBackgroundItem *upItem);

    CSceneBackgroundItem *getDownItem() const;
    void setDownItem(CSceneBackgroundItem *downItem);
    //获取该背景属于哪个视图
    ItemOnView getItemOfView() const;

protected:
    virtual void updateCurrentItemShow();
signals:
    void setChangeFocus();

private:
    int m_backgroundNum;
    CSceneBackgroundItem *m_leftItem;
    CSceneBackgroundItem *m_rightItem;
    CSceneBackgroundItem *m_upItem;
    CSceneBackgroundItem *m_downItem;
protected:
    QVector<CFocusItem *> m_item {};
    int m_showItemIndex;
    ItemOnView m_itemOfView;
};

#endif // CSCENEBACKGROUNDITEM_H
