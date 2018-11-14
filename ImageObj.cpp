//
//  ImageObj.cpp
//  imageviewer-qt5
//
//  Created by Darwin Schlenk on 25.10.18.
//

#include "ImageObj.h"

#include "YUVColor.h"

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


void ImageObj::getGaussCumu(int sigma)
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

void ImageObj::applyFilter(int** filter, int weight){
    double s = 1.0/weight;

    int filterWidth = filter[0].length/2;
    int filterHeight= filter.length/2;

    for(int u= filterWidth; u < width-filterWidth; u++){
         for(int v= filterHeight; v < height-filterHeight; v++){
            int sum = 0;
            YUVColor color;
            for(int i = -filterWidth; i <= filterWidth; i++){
                for(int j = -filterHeight; j <= filterHeight; j++){
                    color = YUVColor(copyImage->pixelColor(i, j));
                    int c = filter[i+filterWidth][j+filterHeight];
                    sum += c * color.getY();
                }
            }
            color.setY((int) Math.round(s* sum));
            image->setPixelColor(u, v, color);
         }
    }
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
