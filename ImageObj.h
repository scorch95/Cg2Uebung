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
    static const int CONTRAST_MW = 20;
    static const int CONTRAST_MAX = 60;
    
    
    ImageObj(QImage* img);
    ~ImageObj();
    
    int getSize() const;
    double getBrightness() const;
    double getVarianz() const;
    
    //QVector<int>& getHistoVec() const;
    QVector<int>* getCumuHistoVec();
    
    QImage* getImage() const;
    QImage* getHistoImage() const;
    QImage* getCumuHistoImage() const;
    void calcValues();
    QImage* getBlankHistoimage();
    
    void resetToCopyImage();

    void drawRedCross(int crossSize);
    void convertToGreyScale();
    void adjustContrast(double slow,double shigh);
    void adjustHistoLin();
    void applyRefHisto(const QVector<int>* refHistoVec);
    void changeContrast(int contrastFactor);
    void changeBrightness(int brightnessValue);
    
    void applyGaussCumu(int sigma);

    void applyFilter(const QVector<QVector<int>>& filter, int edge, int div);
    //void yuvConvert();
    
    void cannyEdgeDectector(int sigma, int thi, int tlow);
    void applyUSM(int sigma, double a);
    void applyHoughTrans(int aSteps, int rSteps, double tHi, double tLow);
    
private:

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
    QImage* applyFilterOnImage(const QVector<QVector<int>>& filter, int div);
    QVector<QVector<int>> initGradientVector(char type) const;
    void applyGaussFilter();
    void drawLine(int x0, int y0, int x1, int y1);
    
};
#endif /* ImageObj_h */
