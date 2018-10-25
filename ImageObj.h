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
    Q_OBJECT
    
public:
    ImageObj(QImage& img);
    ~ImageObj();
    
    int getSize() const;
    double getBrightness() const;
    double getVarianz() const;
    
    QVector<int>& getHistoVec() const;
    QVector<int>& getCumuHistoVec() const;
    
    QImage& getImage() const;
    void calcValues();
    
public slots:
    void convertToGreyScale();
    void adjustContrast(double slow,double shigh);
    
    void changeContrast(int contrastFactor);
    void changeBrightness(int brightnessValue);
    
private:
    QImage copyImage;
    
    double brightness;
    double varianz;
    
    int size;
    int contrastFactor;
    int brightnessValue;
    
    QVector<int> histoVec;
    QVector<int> cumuHistoVec;
    
    void drawNewImage();
    
    
};
#endif /* ImageObj_h */
