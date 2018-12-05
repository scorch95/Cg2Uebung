//
//  ImageObj.cpp
//  imageviewer-qt5
//
//  Created by Darwin Schlenk on 25.10.18.
//

#include "ImageObj.h"

#include "YUVColor.h"
#include "Gradient.h"

#include <iostream>
#include <cmath>

ImageObj::ImageObj(QImage* img){
    image = new QImage(*img);
    copyImage = new QImage(*img);

    height = image->height();
    width = image->width();
    image2DSize = height * width;

    histoImage = getBlankHistoimage();
    cumuHistoImage = getBlankHistoimage();

    brightness = 0;
    varianz = 0;

    contrastFactor = 1;
    brightnessValue = 0;
    
    histoVec = nullptr;
    cumuHistoVec = nullptr;
    
    calcValues();
}

ImageObj::~ImageObj(){
    if(image!=nullptr)
    {
        delete image;
    }
    if(copyImage!=nullptr)
    {
        delete copyImage;
    }
}

void ImageObj::calcValues(){
    setPointersToNull();
    histoImage = getBlankHistoimage();
    cumuHistoImage= getBlankHistoimage();
    QVector<int> contrast(256);
    for(int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            //double light=qGray(image->pixel(i, j));

            YUVColor color = YUVColor(image->pixelColor(i, j));
            int light = color.getY();
            brightness += light;
            if(light >= 0 && light < 256)
                contrast[light]++;
            //std::cout<< i*n+j << std::endl;
        }
    }

    brightness /= image2DSize;

    for(int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            YUVColor color = YUVColor(image->pixelColor(i, j));
            double light = static_cast<double>(color.getY());
            varianz += std::pow(light-brightness, 2);
        }
    }

    varianz /= image2DSize;

    QColor histogramColor = QColor(Qt::GlobalColor::black);
    int max = *std::max_element(contrast.begin(), contrast.end());
    for(int i=0; i<histoImage->width(); i++)
    {
        if( contrast[i] != 0)
        {
            //std::cout<< contrast[i] << " pro "<< max<< " MN "<<m*n<< " at "<< i<< std::endl;
            int maxHeight = contrast[i]*100/max;
            for (int j=0; j<maxHeight; j++)
            {
                histoImage->setPixelColor(i, 99 - j, histogramColor);
            }
        }
    }

    int sum =0;
    cumuHistoVec = new QVector<int>();
    for(int i=0; i<cumuHistoImage->width(); i++)
    {
        sum +=contrast[i];
        cumuHistoVec->insert(i, sum);
        int maxHeight = sum*100/image2DSize;
        for (int j=0; j<maxHeight; j++)
        {
            cumuHistoImage->setPixelColor(i, 99 - j, histogramColor);
        }
    }

    histoVec = new QVector<int>(contrast);
    
    
}

void ImageObj::overrideImage(){
    if( copyImage ==nullptr||image==nullptr)
    {
        return;
    }
    for(int i = 0; i<copyImage->width(); i++)
    {
        for (int j = 0; j<copyImage->height(); j++)
        {
            YUVColor color = YUVColor(copyImage->pixelColor(i, j));
            color.setY((color.getY()+brightnessValue)*contrastFactor/(double)CONTRAST_MW);

            image->setPixelColor(i, j, color);

        }
    }
    calcValues();
}

int ImageObj::getSize() const{
    return image2DSize;
}

double ImageObj::getBrightness() const{
    return brightness;
}

double ImageObj::getVarianz() const{
    return varianz;
}

QImage* ImageObj::getHistoImage() const{
    return histoImage;
}

QImage* ImageObj::getCumuHistoImage() const{
    return cumuHistoImage;
}

QImage* ImageObj::getImage() const{
    return image;
}

void ImageObj::drawRedCross(int crossSize){
    if(image!=nullptr)
    {
        delete image;
        image = new QImage(*copyImage);
        QColor crossColor = QColor(Qt::GlobalColor::red);

        int crossWidth = height*crossSize/100;
        int crossHeight = width*crossSize/100;

        for(int i=0;i<std::min(crossWidth, crossHeight);i++)
        {
            image->setPixelColor(i,i,crossColor);
            image->setPixelColor(i,crossHeight-i,crossColor);
        }
    }
}

void ImageObj::convertToGreyScale(){

}

void ImageObj::adjustContrast(double slow,double shigh){
    int low;
    int high;
    for (int i = 0; i<cumuHistoVec->size(); i++) {
        if(cumuHistoVec->at(i) >= slow)
        {
            low = i;
            std::cout << "low: " <<low << std::endl;
            break;
        }
    }

    std::cout << "VecSize: " <<cumuHistoVec->size() <<std::endl;
    for(int i = cumuHistoVec->size()-1; i>=0; i--)
    {
        if(cumuHistoVec->at(i) <= image2DSize - shigh)
        {
            high = i;
            std::cout << "high: " << high << std::endl;
            break;
        }
    }

    for(int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            YUVColor color = YUVColor(copyImage->pixelColor(i, j));
            int light = color.getY();
            if(light<=low)
            {
                light = 0;
            }
            else if(light>=high)
            {
                light = 255;
            }
            else
            {
                light = ceil((light-low)*255/(double)(high-low));
            }

            color.setY(light);
            image->setPixelColor(i, j, color);

        }
    }

    calcValues();
}

void ImageObj::adjustHistoLin(){
    for(int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            YUVColor color = YUVColor(copyImage->pixelColor(i, j));
            int light = color.getY();
            light = cumuHistoVec->at(light)*255/(image2DSize);
            color.setY(light);
            image->setPixelColor(i, j, color);

        }
    }

    calcValues();
}

void ImageObj::applyRefHisto(const QVector<int>* refHistoVec)
{
    int refSize = refHistoVec->at(refHistoVec->size()-1);
    
    std::cout << "histo "<<histoVec->size() << " cumu "<< cumuHistoVec->size() << "refHistoVec "<<refHistoVec->size()<<std::endl;
    for(int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            YUVColor color = YUVColor(copyImage->pixelColor(i, j));
            int light = color.getY();
            int pixelNum = (1.0*cumuHistoVec->at(light)/image2DSize)*refSize;
            //std::cout << cumuHistoVec->at(light) << " : "<< refSize << std::endl;
            for(int k = 0; k<refHistoVec->size(); k++)
            {
                if(refHistoVec->at(k)>= pixelNum)
                {
                    light=k;
                    //std::cout << " k  "<<k << " refHistoVec->at(k)   "<< refHistoVec->at(k) << " pixelNum   "<<pixelNum << " refHistoVec "<<refHistoVec->size()<<std::endl;
                     break;
                }
            }
            color.setY(light);
            image->setPixelColor(i, j, color);
            
        }
    }
    
    calcValues();
}

void ImageObj::changeContrast(int contrastFactor){
    this->contrastFactor = contrastFactor;
    overrideImage();
}

void ImageObj::changeBrightness(int brightnessValue){
    this->brightnessValue = brightnessValue;
    overrideImage();
}

QImage* ImageObj::getBlankHistoimage()
{
    QImage* returnImage = new QImage(256,100, QImage::Format_RGB32);
    returnImage->fill(QColor(183, 183, 183));

    return returnImage;
}

int ImageObj::checkColor(int value)
{
    if(value > 255)
        value = 255;
    else if(value < 0)
        value = 0;
    return value;
}

void ImageObj::setPointersToNull(){
    if(histoImage!=nullptr)
    {
        delete histoImage;
    }
    if(cumuHistoImage!=nullptr)
    {
        delete cumuHistoImage;
    }
    if(histoVec!=nullptr)
    {
        delete histoVec;
    }
    if(cumuHistoVec!=nullptr)
    {
        delete cumuHistoVec;
    }
}

QVector<int>* ImageObj::getCumuHistoVec()
{
    return cumuHistoVec;
}


void ImageObj::applyGaussCumu(int sigma)
{
    QVector<int>* returnVec = new QVector<int>();
    int mu = 128;
    returnVec->insert(0, 100.*((1.0/sigma*sqrt(2*M_PI))*exp(-0.5*((0-mu)/sigma)*((0-mu)/sigma))));
    for (int i = 1; i<256 ; i++)
    {
        returnVec->insert(i, 100.*((1.0/sigma*sqrt(2*M_PI))*exp(-0.5*((i-mu)/sigma)*((i-mu)/sigma)))+ returnVec->at(i-1));
    }
    //for(auto k : *returnVec)
        //std::cout<< k << std::endl;
    applyRefHisto(returnVec);
    delete returnVec;
}

void ImageObj::resetToCopyImage()
{
    if(image != nullptr)
    {
        delete image;
        image = new QImage(*copyImage);
        calcValues();
    }
}

void ImageObj::applyFilter(const QVector<QVector<int>>& filter, int edge, int div){

    int filterWidth = filter[0].size();
    int filterHeight = filter.size();

    int filterWidthHalf = filterWidth/2;
    int filterHeightHalf= filterHeight/2;

    //double div = filterWidth*filterHeight;
    std::cout << "width: " << filterWidthHalf << " height: " << filterHeightHalf << std::endl;
    for(int u= filterWidthHalf; u < width-filterWidthHalf; u++){
         for(int v= filterHeightHalf; v < height-filterHeightHalf; v++){
            int sum = 0;
            YUVColor color = YUVColor();
            for(int i = -filterWidthHalf; i <= filterWidthHalf; i++){
                for(int j = -filterHeightHalf; j <= filterHeightHalf; j++){
                    color = YUVColor(copyImage->pixelColor(u+i, v+j));
                    int c = filter.at(i+filterWidthHalf).at(j+filterHeightHalf);
                    sum += c * color.getY();
                }
            }
             //std::cout << "div: "<<div << std::endl;

             int  y = (int) std::round((1.0 * sum)/ (1.0*div));
             color.setY(y);
             //color.convertToGrey();
            image->setPixelColor(u, v, color);
         }
    }

    if(edge > 0)
    {
        QColor borderColor;
        if(edge == 1)
        {
            borderColor = QColor(Qt::GlobalColor::black);
        }
        for(int i=0; i < width; i++){
            if(edge == 2)
            {
                borderColor= image->pixel(i,filterHeightHalf);
            }
             for(int j= 0; j < filterHeightHalf; j++)
             {
                if(edge == 3)
                {
                    borderColor= image->pixel(i,2*filterHeightHalf-j);
                }
                image->setPixelColor(i, j, borderColor);
             }
            if(edge == 2)
            {
                borderColor= image->pixel(i,height-filterHeightHalf-1);
            }
            for(int j= height-filterHeightHalf; j < height; j++)
            {
                if(edge == 3)
                {
                    borderColor= image->pixel(i,height-(2*filterHeightHalf)-1-j);
                }
                image->setPixelColor(i, j, borderColor);
            }
        }
        for(int j= filterHeightHalf; j < height-filterHeightHalf; j++){
            if(edge == 2)
            {
                borderColor= image->pixel(filterHeightHalf,j);
            }
            for(int i = 0 ; i < filterWidthHalf ; i++)
            {
                if(edge == 3)
                {
                    borderColor= image->pixel(2*filterHeightHalf-i,j);
                }
                image->setPixelColor(i, j, borderColor);
            }
            if(edge == 2)
            {
                borderColor= image->pixel(width-filterWidthHalf-1,j);
            }
            for(int i = width-filterWidthHalf ; i < width ; i++)
            {
                if(edge == 3)
                {
                    borderColor= image->pixel(width-(2*filterWidthHalf)-1-    i,j);
                }
                image->setPixelColor(i, j, borderColor);
            }
        }
    }

    calcValues();
}

QImage* ImageObj::applyFilterOnImage(const QVector<QVector<int>>& filter, int div)
{
    QImage* tempImage = new QImage(width, height, QImage::Format_RGB32);

    int filterWidth = filter[0].size();
    int filterHeight = filter.size();

    int filterWidthHalf = filterWidth/2;
    int filterHeightHalf= filterHeight/2;

    std::cout << "width: " << filterWidthHalf << " height: " << filterHeightHalf << std::endl;
    for(int u= filterWidthHalf; u < width-filterWidthHalf; u++){
         for(int v= filterHeightHalf; v < height-filterHeightHalf; v++){
            int sum = 0;
            YUVColor color = YUVColor();
            for(int i = -filterWidthHalf; i <= filterWidthHalf; i++){
                for(int j = -filterHeightHalf; j <= filterHeightHalf; j++){
                    color = YUVColor(copyImage->pixelColor(u+i, v+j));
                    int c = filter.at(i+filterWidthHalf).at(j+filterHeightHalf);
                    sum += c * color.getY();
                }
            }
             //std::cout << "sum: " <<  (int) std::round(s* sum) << std::endl;

             int  y = (int) std::round((1.0 * sum)/ (1.0*div));
             /*if(y == 0)
             {
                 y = 0;
             }
             else
             {
                 y = 255;
             }*/
             color.setY(y);
             //color.convertToGrey();
            tempImage->setPixelColor(u, v, color);
         }
    }
    return tempImage;
}

void ImageObj::cannyEdgeDectector(int sigma, int thi, int tlow)
{
    applyGaussFilter();
    QImage* gradientX = applyFilterOnImage(initGradientVector('x'), 2);
    QImage* gradientY = applyFilterOnImage(initGradientVector('y'), 2);
    
    QVector<QVector<int>> iX;
    QVector<QVector<int>> iY;
    
    for (int i = 0; i<gradientX->width(); i++)
    {
        QVector<int> subX;
        QVector<int> subY;
        
        for(int j= 0; j < gradientX->height(); j++){
            YUVColor xColor = YUVColor(gradientX->pixelColor(i, j));
            YUVColor yColor = YUVColor(gradientY->pixelColor(i, j));
            /*if(xColor.getY()>0)
            {
                std::cout << xColor.getY() << std::endl;
            }*/
            subX.push_back(xColor.getY());
            subY.push_back(yColor.getY());

        }
        iX.push_back(subX);
        iY.push_back(subY);

    }
    
    Gradient* gradient = new Gradient(iX, iY, width, height);
    
    
    
    if(image != nullptr)
    {
        delete image;
    }
    image = gradient->getBinImage(tlow, thi);
    calcValues();
    
    delete gradient;
    delete gradientX;
    delete gradientY;
    
}


QVector<QVector<int>> ImageObj::initGradientVector(char type) const
{
    QVector<QVector<int>> filter;
    for(int i=0; i < 3; i++){
        QVector<int> filterX;
        for(int j= 0; j < 3; j++){
            filterX.push_back(0);
        }
        filter.push_back(filterX);
    }
    if(type == 'x')
    {
        filter[1][0] = -1;
        filter[1][2] = 1;
    }
    if(type == 'y')
    {
        filter[0][1] = -1;
        filter[2][1] = 1;
    }
    return filter;
}

void ImageObj::applyUSM(int sigma, double a)
{
    //applyGaussCumu(sigma);
    applyGaussFilter();
    
    QImage mask = QImage(*copyImage);
    
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            YUVColor color = YUVColor(copyImage->pixelColor(i, j));
            int yOrig = color.getY();
            color = YUVColor(image->pixelColor(i, j));
            int ySmooth = color.getY();
            color.setY(yOrig-ySmooth);
            
            mask.setPixelColor(i, j, color);
            
        }
    }
    
    for(int i = 0; i < width; i++)
    {
        for(int j = 0; j < height; j++)
        {
            YUVColor color = YUVColor(mask.pixelColor(i, j));
            int yMask = color.getY();
            color = YUVColor(copyImage->pixelColor(i, j));
            int yOrig = color.getY();
            
            color.setY(yOrig+a*yMask);
            
            image->setPixelColor(i, j, color);
        }
    }
    calcValues();
}

void ImageObj::applyGaussFilter()
{
    const QVector<int> l1 = { 1, 2, 1 };
    const QVector<int> l2 = { 2, 4, 2 };
    
    const QVector<QVector<int>> gaussVec = {l1,l2,l1};
    
    applyFilter(gaussVec, 2, 16);
    calcValues();
}
/*void ImageObj::yuvConvert()
{
    for(int i=0; i<width; i++)
    {
        for (int j=0; j<height; j++)
        {
            QColor color = QColor(copyImage->pixel(i, j));
            QRgb y = 0.299*color.red() + 0.587*color.green() + 0.144*color.blue();
            y = cumuHistoVec->at(y)*255/(image2DSize);
            int u = checkColor(0.49*(color.blue()-y));
            int v = checkColor(0.87*(color.red()-y));
            QColor c = QColor(y,u,v);
            image->setPixelColor(i, j, c);

        }
    }

    calcValues();
}*/
