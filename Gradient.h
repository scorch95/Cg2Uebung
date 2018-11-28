#ifndef GRADIENT_H
#define GRADIENT_H

#include <QImage>
#include <cmath>

class Gradient
{
public:
    const double COS_PI8 = std::cos(M_PI/8.);
    const double SIN_PI8 = std::sin(M_PI/8.);

    Gradient();
    Gradient(QVector<QVector<int>>& iX, QVector<QVector<int>>& iY, int h, int w);
    ~Gradient();

    int getValue(int i, int j);
    int getOrientationSector(int i, int j);
    
    void getEmptyVector();
    bool isLocalMax(int i, int j, int s, int tlo) const;
    QImage* getBinImage(int tlo, int thi);
    
    void printQVector(const QVector<QVector<int>>& vector) const;

private:

    QVector<QVector<int>> iX;
    QVector<QVector<int>> iY;
    
    QVector<QVector<int>> emag;
    QVector<QVector<int>> enms;
    QVector<QVector<int>> ebin;
    
    int gHeight;
    int gWidth;
    
    void initGradientMagnitude();
    void traceAndTreshHold(int i, int j,int tlo);

};

#endif // GRADIENT_H
