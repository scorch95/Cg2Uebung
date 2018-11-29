#include "Gradient.h"
#include <iostream>

Gradient::Gradient()
{
    initGradientMagnitude();
    getEmptyVector();
}

Gradient::Gradient(QVector<QVector<int>>& iX, QVector<QVector<int>>& iY, int h, int w) : gHeight(h), gWidth(w)
{
    this->iX= iX;
    this->iY= iY;
    initGradientMagnitude();
    getEmptyVector();
}

Gradient::~Gradient()
{
}

int Gradient::getValue(int i, int j)
{
    //std::cout << "i: " << i << " j: " << j << std::endl;
    int dx = iX[i][j];
    int dy = iY[i][j];
    return std::sqrt(dx*dx+dy*dy);
}

int Gradient::getOrientationSector(int i, int j)
{
    int s;
    int x = iX[i][j];
    int y = iY[i][j];
    //QVector2D* vec = new QVector2D(COS_PI8*x-SIN_PI8*y, SIN_PI8*x+COS_PI8*y);

    int dx = COS_PI8*x-SIN_PI8*y;
    int dy = SIN_PI8*x+COS_PI8*y;

    if(dy < 0)
    {
        dx = -dx;
        dy = -dy;
    }

    if(dx > 0 && dx >= dy)
        s = 0;
    if(dx > 0 && dx < dy)
        s = 1;
    if(dx < 0 && -dx < dy)
        s = 2;
    if(dx < 0 && -dx >= dy)
        s = 3;

    return s;
}


void Gradient::initGradientMagnitude()
{
    for (int i = 0; i<gWidth; i++) {
        QVector<int> emagX;
        for(int j= 0; j < gHeight; j++){
            emagX.push_back(getValue(i, j));
        }
        emag.push_back(emagX);
    }
}

void Gradient::getEmptyVector()
{
    for (int i = 0; i<gWidth; i++) {
        QVector<int> emagX;
        for(int j= 0; j < gHeight; j++){
            emagX.push_back(0);
        }
        ebin.push_back(emagX);
        enms.push_back(emagX);

    }
}

bool Gradient::isLocalMax(int i, int j, int s, int tlo) const
{
    int mCurrent = emag[i][j];
    if(mCurrent <  tlo)
    {
        return false;
    }
    else
    {
        int mLeft = 0;
        int mRight = 0;
        if (s == 0)
        {
            mLeft = emag[i-1][j];
            mRight = emag[i+1][j];
        }
        else if (s == 1)
        {
            mLeft = emag[i-1][j-1];
            mRight = emag[i+1][j+1];
        }
        else if (s == 2)
        {
            mLeft = emag[i][j-1];
            mRight = emag[i][j+1];
        }
        else if (s == 3)
        {
            mLeft = emag[i-1][j+1];
            mRight = emag[i+1][j-1];
        }
        return (mLeft<=mCurrent)&&(mCurrent>=mRight);
    }
}

QImage* Gradient::getBinImage(int tlo, int thi)
{
    for(int i = 1; i<gWidth-1; i++)
    {
        for (int j = 1;  j<gHeight-1; j++)
        {
            int s = getOrientationSector(i, j);
            if(isLocalMax(i, j, s, tlo))
            {
                enms[i][j] = emag[i][j];
            }
        }
    }
    
    //printQVector(enms);
    
    for(int i = 1; i<gWidth-1; i++)
    {
        for (int j = 1;  j<gHeight-1; j++)
        {
            if(enms[i][j]>=thi && ebin[i][j] == 0)
            {
                traceAndTreshHold(i,j,tlo);
            }
        }
    }
    //printQVector(ebin);
    
    QImage* tempImage = new QImage(gWidth,gHeight, QImage::Format_RGB32);
    for(int i = 1; i<gWidth-1; i++)
    {
        for (int j = 1;  j<gHeight-1; j++)
        {
            tempImage->setPixelColor(i, j, QColor(ebin[i][j], ebin[i][j], ebin[i][j]));
        }
    }
               
    return tempImage;

}


void Gradient::traceAndTreshHold(int i, int j,int tlo)
{
    ebin[i][j] = 255;
    for(int u = std::max(i-1, 0); u <= std::min(i+1, gWidth); u++)
    {
        for(int v = std::max(j-1, 0); v <=std::min(j+1, gHeight);v++)
        {
            if(enms[u][v] >= tlo && ebin[u][v] == 0)
            {
                traceAndTreshHold(u, v, tlo);
            }
        }
    }
}


void Gradient::printQVector(const QVector<QVector<int>>& vector) const
{
    for(int i = 0; i<vector.size(); i++)
    {
        for (int j = 0;  j<vector[0].size(); j++)
        {
            std::cout << vector[i][j] << "\t";
        }
        std::cout << std::endl;
    }
}
