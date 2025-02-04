/*
 * Copyright (C) 2017 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
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
#ifndef CSCHEDULECTRLDLG_H
#define CSCHEDULECTRLDLG_H

#include "dcalendarddialog.h"

#include <DFrame>

#include <QObject>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

class CScheduleCtrlDlg : public DCalendarDDialog
{
    Q_OBJECT
public:
    explicit CScheduleCtrlDlg(QWidget *parent = nullptr);
    QAbstractButton *addPushButton(QString btName, bool type = false);
    QAbstractButton *addsuggestButton(QString btName, bool type = false);
    QAbstractButton *addWaringButton(QString btName, bool type = false);
    void setText(QString str);
    void setInformativeText(QString str);
    int clickButton();
private:
    void initUI();
    void initConnection();
    /**
     * @brief setTheMe  根据主题type设置颜色
     * @param type  主题type
     */
    void setTheMe(const int type);
    /**
     * @brief setPaletteTextColor   设置调色板颜色
     * @param widget    需要设置的widget
     * @param textColor     显示颜色
     */
    void setPaletteTextColor(QWidget *widget, QColor textColor);
protected:
    void changeEvent(QEvent *event) override;
private slots:
    void buttonJudge(int id);
private:
    QLabel                           *m_firstLabel = nullptr;
    QLabel                           *m_seconLabel = nullptr;
    int                              m_id = -1;
    QVBoxLayout *m_mainBoxLayout = nullptr;
    DFrame *gwi = nullptr;
    QFont labelF;
    QFont labelT;
    QVector<QString> str_btName;
};

#endif // CSCHEDULECTRLDLG_H
