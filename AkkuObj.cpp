#include "AkkuObj.h"

AkkuObj::AkkuObj()
{

}

AkkuObj::~AkkuObj()
{

}

void AkkuObj::setPoints(int& x, int& y)
{
    if(count == 0)
    {
        startX = x;
        startY = y;
    }

    endX = x;
    endY = y;
    count++;
}

int AkkuObj::count()
{
    return count;
}
