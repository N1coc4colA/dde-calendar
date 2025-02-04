/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhengxiaokang <zhengxiaokang@uniontech.com>
*
* Maintainer: zhengxiaokang <zhengxiaokang@uniontech.com>
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
#ifndef CELESTIALBODIES_H
#define CELESTIALBODIES_H

#include <QVector>
#include <QtCore/qmath.h>

//弧度
typedef struct _MoonEclipticParameter {
    double Lp;
    double D;
    double M;
    double Mp;
    double F;
    double E;
} MoonEclipticParameter;

// 月地心黄经系数
typedef struct  _MoonEclipticLongitudeCoeff {
    double D;
    double M;
    double Mp;
    double F;
    double EiA;
    double ErA;
} MoonEclipticLongitudeCoeff;
//弧度
typedef struct _EarthNutationParameter {
    double D;
    double M;
    double Mp;
    double F;
    double Omega;
} EarthNutationParameter;

//天体章动系数类型变量
typedef struct _NuationCoefficient {
    double D;
    double M;
    double Mp;
    double F;
    double Omega;
    double Sine1;
    double Sine2;
    double Cosine1;
    double Cosine2;
} NuationCoefficient;

/*
    月球黄经周期项(ΣI)及距离(Σr).
    黄经单位:0.000001度,距离单位:0.001千米.
--------------------------------------------------
  角度的组合系数  ΣI的各项振幅A  Σr的各项振幅A
  D  M  M' F        (正弦振幅)       (余弦振幅)
--------------------------------------------------
*/
static QVector<MoonEclipticLongitudeCoeff> MoonLongitude {
    {0, 0, 1, 0, 6288744, -20905355},
    {2, 0, -1, 0, 1274027, -3699111},
    {2, 0, 0, 0, 658314, -2955968},
    {0, 0, 2, 0, 213618, -569925},
    {0, 1, 0, 0, -185116, 48888},
    {0, 0, 0, 2, -114332, -3149},
    {2, 0, -2, 0, 58793, 246158},
    {2, -1, -1, 0, 57066, -152138},
    {2, 0, 1, 0, 53322, -170733},
    {2, -1, 0, 0, 45758, -204586},
    {0, 1, -1, 0, -40923, -129620},
    {1, 0, 0, 0, -34720, 108743},
    {0, 1, 1, 0, -30383, 104755},
    {2, 0, 0, -2, 15327, 10321},
    {0, 0, 1, 2, -12528, 0},
    {0, 0, 1, -2, 10980, 79661},
    {4, 0, -1, 0, 10675, -34782},
    {0, 0, 3, 0, 10034, -23210},
    {4, 0, -2, 0, 8548, -21636},
    {2, 1, -1, 0, -7888, 24208},
    {2, 1, 0, 0, -6766, 30824},
    {1, 0, -1, 0, -5163, -8379},
    {1, 1, 0, 0, 4987, -16675},
    {2, -1, 1, 0, 4036, -12831},
    {2, 0, 2, 0, 3994, -10445},
    CELESTIALBODIES_H {4, 0, 0, 0, 3861, -11650},
    {2, 0, -3, 0, 3665, 14403},
    {0, 1, -2, 0, -2689, -7003},
    {2, 0, -1, 2, -2602, 0},
    {2, -1, -2, 0, 2390, 10056},
    {1, 0, 1, 0, -2348, 6322},
    {2, -2, 0, 0, 2236, -9884},
    {0, 1, 2, 0, -2120, 5751},
    {0, 2, 0, 0, -2069, 0},
    {2, -2, -1, 0, 2048, -4950},
    {2, 0, 1, -2, -1773, 4130},
    {2, 0, 0, 2, -1595, 0},
    {4, -1, -1, 0, 1215, -3958},
    {0, 0, 2, 2, -1110, 0},
    {3, 0, -1, 0, -892, 3258},
    {2, 1, 1, 0, -810, 2616},
    {4, -1, -2, 0, 759, -1897},
    {0, 2, -1, 0, -713, -2117},
    {2, 2, -1, 0, -700, 2354},
    {2, 1, -2, 0, 691, 0},
    {2, -1, 0, -2, 596, 0},
    {4, 0, 1, 0, 549, -1423},
    {0, 0, 4, 0, 537, -1117},
    {4, -1, 0, 0, 520, -1571},
    {1, 0, -2, 0, -487, -1739},
    {2, 1, 0, -2, -399, 0},
    {0, 0, 2, -2, -381, -4421},
    {1, 1, 1, 0, 351, 0},
    {3, 0, -2, 0, -340, 0},
    {4, 0, -3, 0, 330, 0},
    {2, -1, 2, 0, 327, 0},
    {0, 2, 1, 0, -323, 1165},
    {1, 1, -1, 0, 299, 0},
    {2, 0, 3, 0, 294, 0},
    {2, 0, -1, -2, 0, 8752},
};

static QVector<NuationCoefficient> nuation {
    {0, 0, 0, 0, 1, -171996, -174.2, 92025, 8.9},
    {-2, 0, 0, 2, 2, -13187, -1.6, 5736, -3.1},
    {0, 0, 0, 2, 2, -2274, -0.2, 977, -0.5},
    {0, 0, 0, 0, 2, 2062, 0.2, -895, 0.5},
    {0, 1, 0, 0, 0, 1426, -3.4, 54, -0.1},
    {0, 0, 1, 0, 0, 712, 0.1, -7, 0},
    {-2, 1, 0, 2, 2, -517, 1.2, 224, -0.6},
    {0, 0, 0, 2, 1, -386, -0.4, 200, 0},
    {0, 0, 1, 2, 2, -301, 0, 129, -0.1},
    {-2, -1, 0, 2, 2, 217, -0.5, -95, 0.3},
    {-2, 0, 1, 0, 0, -158, 0, 0, 0},
    {-2, 0, 0, 2, 1, 129, 0.1, -70, 0},
    {0, 0, -1, 2, 2, 123, 0, -53, 0},
    {2, 0, 0, 0, 0, 63, 0, 0, 0},
    {0, 0, 1, 0, 1, 63, 0.1, -33, 0},
    {2, 0, -1, 2, 2, -59, 0, 26, 0},
    {0, 0, -1, 0, 1, -58, -0.1, 32, 0},
    {0, 0, 1, 2, 1, -51, 0, 27, 0},
    {-2, 0, 2, 0, 0, 48, 0, 0, 0},
    {0, 0, -2, 2, 1, 46, 0, -24, 0},
    {2, 0, 0, 2, 2, -38, 0, 16, 0},
    {0, 0, 2, 2, 2, -31, 0, 13, 0},
    {0, 0, 2, 0, 0, 29, 0, 0, 0},
    {-2, 0, 1, 2, 2, 29, 0, -12, 0},
    {0, 0, 0, 2, 0, 26, 0, 0, 0},
    {-2, 0, 0, 2, 0, -22, 0, 0, 0},
    {0, 0, -1, 2, 1, 21, 0, -10, 0},
    {0, 2, 0, 0, 0, 17, -0.1, 0, 0},
    {2, 0, -1, 0, 1, 16, 0, -8, 0},
    {-2, 2, 0, 2, 2, -16, 0.1, 7, 0},
    {0, 1, 0, 0, 1, -15, 0, 9, 0},
    {-2, 0, 1, 0, 1, -13, 0, 7, 0},
    {0, -1, 0, 0, 1, -12, 0, 6, 0},
    {0, 0, 2, -2, 0, 11, 0, 0, 0},
    {2, 0, -1, 2, 1, -10, 0, 5, 0},
    {2, 0, 1, 2, 2, -8, 0, 3, 0},
    {0, 1, 0, 2, 2, 7, 0, -3, 0},
    {-2, 1, 1, 0, 0, -7, 0, 0, 0},
    {0, -1, 0, 2, 2, -7, 0, 3, 0},
    {2, 0, 0, 2, 1, -7, 0, 3, 0},
    {2, 0, 1, 0, 0, 6, 0, 0, 0},
    {-2, 0, 2, 2, 2, 6, 0, -3, 0},
    {-2, 0, 1, 2, 1, 6, 0, -3, 0},
    {2, 0, -2, 0, 1, -6, 0, 3, 0},
    {2, 0, 0, 0, 1, -6, 0, 3, 0},
    {0, -1, 1, 0, 0, 5, 0, 0, 0},
    {-2, -1, 0, 2, 1, -5, 0, 3, 0},
    {-2, 0, 0, 0, 1, -5, 0, 3, 0},
    {0, 0, 2, 2, 1, -5, 0, 3, 0},
    {-2, 0, 2, 0, 1, 4, 0, 0, 0},
    {-2, 1, 0, 2, 1, 4, 0, 0, 0},
    {0, 0, 1, -2, 0, 4, 0, 0, 0},
    {-1, 0, 1, 0, 0, -4, 0, 0, 0},
    {-2, 1, 0, 0, 0, -4, 0, 0, 0},
    {1, 0, 0, 0, 0, -4, 0, 0, 0},
    {0, 0, 1, 2, 0, 3, 0, 0, 0},
    {0, 0, -2, 2, 2, -3, 0, 0, 0},
    {-1, -1, 1, 0, 0, -3, 0, 0, 0},
    {0, 1, 1, 0, 0, -3, 0, 0, 0},
    {0, -1, 1, 2, 2, -3, 0, 0, 0},
    {2, -1, -1, 2, 2, -3, 0, 0, 0},
    {0, 0, 3, 2, 2, -3, 0, 0, 0},
    {2, -1, 0, 2, 2, -3, 0, 0, 0},
};

// Gregorian历TT2000年1月1日中午12点的儒略日
const double J2000 = 2451545.0;

double GetEarthL0(double t);

double GetEarthL1(double t);

double GetEarthL2(double t);

double GetEarthL3(double t);

double GetEarthL4(double t);

double GetEarthL5(double t);

double GetEarthB0(double t);

double GetEarthB1(double t);

double GetEarthB2(double t);

double GetEarthB3(double t);

double GetEarthB4(double t);

double GetEarthR0(double t);

double GetEarthR1(double t);

double GetEarthR2(double t);

double GetEarthR3(double t);

double GetEarthR4(double t);

double GetEarthR5(double t);
#endif // ABOUTCELESTIALBODIES_H
