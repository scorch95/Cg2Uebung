//
//  ImageObj.h
//  imageviewer-qt5
//
//  Created by Darwin Schlenk on 25.10.18.
//

#ifndef ImageObj_h
#define ImageObj_h

#include <stdio.h>
#include <QImage>

class ImageObj
{
    
public:
    ImageObj(QImage* img);
    ~ImageObj();
    
    int getSize() const;
    double getBrightness() const;
    double getVarianz() const;
    
    //QVector<int>& getHistoVec() const;
    //QVector<int>& getCumuHistoVec() const;
    
    QImage* getImage() const;
    QImage* getHistoImage() const;
    QImage* getCumuHistoImage() const;
    void calcValues();
    QImage* getBlankHistoimage();

    void drawRedCross(int crossSize);
    void convertToGreyScale();
    void adjustContrast(double slow,double shigh);
    void adjustHistoLin();
    void changeContrast(int contrastFactor);
    void changeBrightness(int brightnessValue);
    
private:
    const int CONTRAST_MW = 20;
    const int CONTRAST_MAX = 60;

    QImage* image;
    QImage* copyImage;
    
    double brightness;
    double varianz;
    
    int image2DSize;
    int width;
    int height;
    int contrastFactor;
    int brightnessValue;
    
    QVector<int>* histoVec;
    QVector<int>* cumuHistoVec;

    QImage* histoImage;
    QImage* cumuHistoImage;
    
    void overrideImage();
    int checkColor(int value);
    void setPointersToNull();
    
};
#endif /* ImageObj_h */
