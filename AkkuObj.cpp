#include "AkkuObj.h"

AkkuObj::AkkuObj() : count(0), startX(-1), startY(-1), endX(-1), endY(-1)
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

int AkkuObj::getCount()
{
    return count;
}
