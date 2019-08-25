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

#include "calendarwindow.h"
#include "environments.h"

#include <QFile>
#include <QDebug>
#include <QDesktopWidget>
#include <QDBusConnection>

#include <DApplication>
#include <DLog>
#include <DHiDPIHelper>
#include "dbmanager.h"
#include "calendarmainwindow.h"
//#include "monthwindow.h"
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

QString GetStyleSheetContent()
{
    QFile file(":/resources/dde-calendar.qss");
    bool result = file.open(QFile::ReadOnly);
    if (result) {
        QString content(file.readAll());
        file.close();
        return content;
    } else {
        return "";
    }
}

QRect PrimaryRect()
{
    QDesktopWidget *w = QApplication::desktop();
    return w->screenGeometry(w->primaryScreen());
}

int main(int argc, char *argv[])
{
    DApplication::loadDXcbPlugin();
    DApplication a(argc, argv);
    a.setOrganizationName("deepin");
    a.setApplicationName("dde-calendar");
    a.setApplicationVersion(DApplication::buildVersion("1.1"));
    a.loadTranslator();
    //QList<QLocale> localeFallback = QList<QLocale>() << QLocale::system();
    // meta information that necessary to create the about dialog.
    a.setProductName(QApplication::translate("CalendarWindow", "Deepin Calendar"));
    a.setProductIcon(DHiDPIHelper::loadNxPixmap(":/resources/icon/dde-calendar.svg"));
    a.setApplicationDescription(QApplication::translate("CalendarWindow", "Calendar is a date tool."));
    a.setApplicationAcknowledgementPage("https://www.deepin.org/acknowledgments/dde-calendar");
    a.setTheme("light");
    a.setStyle("chameleon");
    static const QDate buildDate = QLocale( QLocale::English ).toDate( QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy");
    QString t_date = buildDate.toString("MMdd");
    // Version Time
    a.setApplicationVersion(DApplication::buildVersion(t_date));

    if (!a.setSingleInstance("dde-calendar", DApplication::UserScope)) {
        qDebug() << "there's an dde-calendar instance running.";
        QProcess::execute("dbus-send --print-reply --dest=com.deepin.Calendar "
                          "/com/deepin/Calendar com.deepin.Calendar.RaiseWindow");

        return 0;
    }

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();


    //a.setStyleSheet(GetStyleSheetContent());
    //a.loadTranslator();
    ScheduleDbManager::initDataBase();
    Calendarmainwindow ww;
    // ww.setDate(QDate::currentDate());
    ww.move(PrimaryRect().center() - ww.geometry().center());
    ww.show();
    //test
    // CMonthWindow ww;
    //ww.setFirstWeekday(0);
    //ww.setDate(QDate::currentDate());
    //ww.move(PrimaryRect().center() - ww.geometry().center());
    //ww.show();



    //CalendarWindow cw;
    //cw.move(PrimaryRect().center() - cw.geometry().center());
    //cw.show();

    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (dbus.registerService("com.deepin.Calendar")) {
        dbus.registerObject("/com/deepin/Calendar", &ww);
    }

    return a.exec();
}
