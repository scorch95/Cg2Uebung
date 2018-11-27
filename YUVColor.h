//
//  YUVColor.h
//  imageviewer-qt5
//
//  Created by Darwin Schlenk on 12.11.18.
//

#ifndef YUVColor_h
#define YUVColor_h

#include <QColor>

class YUVColor : public QColor
{
public:
    YUVColor();
    YUVColor(QColor color);
    
    int getY() const;
    int getU() const;
    int getV() const;
    
    void setY(int y);
    //void setU(int u);
    //void setV(int v);
    
    void convertToGrey();
    
private:
    double calcY(double r, double g, double b) const;
    double calcU(double y, double b) const;
    double calcV(double y , double r) const;
    
    int checkColor(int value) const;
    
};

#endif /* YUVColor_h */
