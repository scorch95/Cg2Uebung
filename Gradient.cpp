#include "Gradient.h"

Gradient::Gradient()
{

}

Gradient::Gradient(QImage* iX, QImage* iY)
{
    this->iX= iX;
    this->iY= iY;
}

Gradient::~Gradient()
{
    delete iX;
    delete iY;
}

int Gradient::getValue(int i, int j)
{
    int dx = iX->getPixel(i,j);
    int dy = iY->getPixel(i,j);
    return std::sqrt(dx*dx+dy*dy);
}

int Gradient::getOrientationSector(int i, int j)
{
    int s;
    int x = iX->getPixel(i,j);
    int y = iY->getPixel(i,j);
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
