#ifndef GRADIENT_H
#define GRADIENT_H

#include <QImage>
#include <Math.h>

class Gradient
{
public:
    static const double COS_PI8 = std::cos(M_PI/8.);
    static const double SIN_PI8 = std::sin(M_PI/8.);

    Gradient();
    Gradient(QImage* iX, QImage* iY);
    ~Gradient();

    int getValue(int i, int j);
    int getOrientationSector(int i, int j);

private:

    QImage* iX;
    QImage* iY;

};

#endif // GRADIENT_H
