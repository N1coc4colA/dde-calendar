/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chenhaifeng  <chenhaifeng@uniontech.com>
*
* Maintainer: chenhaifeng  <chenhaifeng@uniontech.com>
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
#ifndef LUNARDATEINFO_H
#define LUNARDATEINFO_H

#include "lunarandfestival.h"
#include "method_interface.h"
#include "src/commondatastruct.h"

#include <QDate>
#include <QMap>

/**
 * @brief The LunarDateInfo class
 * 农历时间，用于计算农历重复日程
 */
class LunarDateInfo
{
public:
    explicit LunarDateInfo(const Job &job);
    /**
     * @brief getRRuleStartDate         获取重复农历日程开始时间(公历)集
     * @param beginDate                 查询的起始时间
     * @param endDate                   查询的截止时间
     * @param solarDate                 日程开始时间
     * @return                          日程的重复日程id和对应的开始时间
     */
    QMap<int, QDate> getRRuleStartDate(const QDate &beginDate, const QDate &endDate, const QDate &solarDate);

private:
    /**
     * @brief getAllNextMonthLunarDayBySolar        获取重复类型为每月的农历日程开始时间集
     * @param solarDate                             农历日程的开始时间
     * @return                                      重复农历日程开始时间集
     */
    QMap<int, QDate> getAllNextMonthLunarDayBySolar(const QDate &solarDate);

    /**
     * @brief getAllNextYearLunarDayBySolar        获取重复类型为每年的农历日程开始时间集
     * @param solarDate                             农历日程的开始时间
     * @return                                      重复农历日程开始时间集
     */
    QMap<int, QDate> getAllNextYearLunarDayBySolar(const QDate &solarDate);

    /**
     * @brief getNextMonthLunarDay                  获取下个月对应的农历信息
     * @param nextDate                              需要计算的公历时间
     * @param info                                  重复的农历信息
     * @return                                      计算公历时间对应的农历信息
     */
    lunarInfo getNextMonthLunarDay(QDate &nextDate, const lunarInfo &info);

    /**
     * @brief ParseRRule            解析重复规则
     * @param rule                  重复规则
     * @return
     */
    stRRuleOptions ParseRRule(const QString &rule);

    /**
     * @brief isWithinTimeFrame     是否在查询时间范围内
     * @return
     */
    bool isWithinTimeFrame(const QDate &solarDate);

    /**
     * @brief addSolarMap       将计算出来的时间添加到Map中，并更新时间和重复次数
     * @param solarMap          符合要求的时间集合
     * @param nextDate          重复农历日程对应的公历时间
     * @param count             重复次数
     * @param addDays           需要更新的天数
     * @return                  如果返回true表示计算出的次数或者时间超过范围，否则返回false
     */
    bool addSolarMap(QMap<int, QDate> &solarMap, QDate &nextDate, int &count, const int addDays);

private:
    Job m_job;
    stRRuleOptions m_options;
    QDate m_queryStartDate;
    QDate m_queryEndDate;
    qint64 m_dateInterval; //日程开始结束间隔天数
};

#endif // LUNARDATEINFO_H
