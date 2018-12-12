#ifndef AKKUOBJ_H
#define AKKUOBJ_H


class AkkuObj
{
public:
    AkkuObj();
    //AkkuObj(int count, int startX, int startY, int endX, int endY);
    ~AkkuObj();

    void setPoints(int& x, int& y);
    int count();
private:
    int count, startX, startY, endX, endY;
};

#endif // AKKUOBJ_H
