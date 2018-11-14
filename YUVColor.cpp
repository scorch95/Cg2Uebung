//
//  YUVColor.cpp
//  imageviewer-qt5
//
//  Created by Darwin Schlenk on 12.11.18.
//

#include "YUVColor.h"

YUVColor::YUVColor(QColor color) :QColor(color)
{
    
}

YUVColor::YUVColor() : QColor()
{
    
}

int YUVColor::getY() const
{
    return calcY(red(), green(), blue());
}
int YUVColor::getU() const
{
    return calcU(getY(), blue());
}
int YUVColor::getV() const
{
    return calcV(getY(), red());
}

void YUVColor::setY(int y)
{
    int v = getV();
    int u = getU();
    
    setRed(checkColor(y + 1.149*v));
    setBlue(checkColor(y + 2.04*u));
    
    setGreen(checkColor((y - 0.3*red() - 0.11*blue())*1.694));
}
/*
void YUVColor::setU(int u)
{
    setGreen(checkColor((1.0/0.59)*calcY(red(), green(), blue()) - (0.3/0.59)*red() - (0.11/0.59)*blue()));
    setRed(checkColor(calcY(red(), green(), blue()) + calcV(calcY(red(), green(), blue()), red())/0.87));
    setBlue(checkColor(calcY(red(), green(), blue()) + u/0.49));
}
void YUVColor::setV(int v)
{
    setGreen(checkColor((1.0/0.59)*calcY(red(), green(), blue()) - (0.3/0.59)*red() - (0.11/0.59)*blue()));
    setRed(checkColor(calcY(red(), green(), blue()) + v/0.87));
    setBlue(checkColor(calcY(red(), green(), blue()) + calcU(calcY(red(), green(), blue()), blue())/0.49));
}*/

double YUVColor::calcY(double r, double g, double b) const
{
    return 0.3 * r + 0.59 * g + 0.11 * b;
}
double YUVColor::calcU(double y, double b) const
{
    return 0.49 * (b-y);
}
double YUVColor::calcV(double y , double r) const
{
    return 0.87 * (r-y);
}


int YUVColor::checkColor(int value) const
{
    if(value > 255)
        value = 255;
    else if(value < 0)
        value = 0;
    return value;
}
