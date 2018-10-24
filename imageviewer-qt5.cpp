/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#ifndef QT_NO_PRINTER
#include <QPrintDialog>
#include <QPrinter>
#endif
#include<iostream>


#include "imageviewer-qt5.h"

ImageViewer::ImageViewer()
{

    image=NULL;
    backupImage=NULL;
    histoImage = NULL;
    cumuHistoImage = NULL;
    histoVec = NULL;
    cumuHistoVec=NULL;

    resize(1600, 600);

    startLogging();

    generateMainGui();

    generateControlPanels();
        createActions();
        createMenus();

        resize(QGuiApplication::primaryScreen()->availableSize() * 0.85 );

#ifdef QT_NO_PRINTER
    logFile << "Printing not available" << std::endl;
#else
    logFile << "Printing available" << std::endl;
#endif

    renewLogging();
}

ImageViewer::~ImageViewer()
{
    if(image!=NULL)
    {
        delete image;
    }
    if(backupImage != NULL)
    {
        delete backupImage;
    }
    if(histoImage!=NULL)
    {
        delete histoImage;
    }
    if(cumuHistoImage!=NULL)
    {
        delete cumuHistoImage;
    }
    if(histoVec!=NULL)
    {
        delete histoVec;
    }
    if(cumuHistoVec!=NULL)
    {
        delete cumuHistoVec;
    }
}

void ImageViewer::drawRedCross()
{
    if(image!=NULL)
    {
        delete image;
        image = new QImage(*backupImage);
        QColor crossColor = QColor(Qt::GlobalColor::red);
        int sliderValue = crossSlider->value();

        int crossWidth = image->width()*sliderValue/100;
        int crossHeight = image->height()*sliderValue/100;

        for(int i=0;i<std::min(crossWidth, crossHeight);i++)
        {
            // macht die Farbe schwarz, bitte recherchieren wie eine andere Farbe gesetzt wird ...
            image->setPixelColor(i,i,crossColor);
            image->setPixelColor(i,crossHeight-i,crossColor);
        }
    updateImageDisplay();
    logFile << "example algorithm applied " << std::endl;
    renewLogging();
    }



}

void ImageViewer::calcValues()
{
    if(histoImage!=NULL)
    {
        delete histoImage;
    }
    if(cumuHistoImage!=NULL)
    {
        delete cumuHistoImage;
    }
    if(histoVec != NULL)
    {
        delete histoVec;
    }
    if(cumuHistoVec!=NULL)
    {
        delete cumuHistoVec;
    }
    

    histoImage = getHistoimage();
    cumuHistoImage = getHistoimage();

    int n = image->width();
    int m = image->height();
    QVector<int> contrast(256);
    double brightness= 0.0;
    double var= 0.0;

    for(int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            //double light=qGray(image->pixel(i, j));

            QColor color = QColor(image->pixel(i, j));
            QRgb light = 0.299*color.red() + 0.587*color.green() + 0.144*color.blue();
            brightness += light;
            contrast[light]++;
            //std::cout<< i*n+j << std::endl;
        }
    }

    brightness /= m*n;

    for(int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            QColor color = QColor(image->pixel(i, j));
            double light = 0.299*color.red() + 0.587*color.green() + 0.144*color.blue();
            var += std::pow(light-brightness, 2);
        }
    }

    var /= m*n;

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

    int cumuHmax = m*n;
    int sum =0;
    cumuHistoVec = new QVector<int>();
    for(int i=0; i<cumuHistoImage->width(); i++)
    {
        sum +=contrast[i];
        cumuHistoVec->insert(i, sum);
        int maxHeight = sum*100/cumuHmax;
        for (int j=0; j<maxHeight; j++)
        {
            cumuHistoImage->setPixelColor(i, 99 - j, histogramColor);
        }
    }

    updateImageDisplay();
    mittlereHelligkeit->setText("Mittlere Helligkeit:  "+QString::number(brightness));
    varianz->setText("Varianz: "+QString::number(var));

    histoVec = new QVector<int>(contrast);
}

void ImageViewer::convertToGreyScale()
{
    int n = image->width();
    int m = image->height();

    for(int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            QColor color = QColor(image->pixel(i, j));
            QRgb light = 0.299*color.red() + 0.587*color.green() + 0.144*color.blue();

            QColor c = QColor(light,light,light);
            image->setPixelColor(i, j, c);

        }
    }

    updateImageDisplay();
}

/****************************************************************************************
*
*  mit dieser Methode können sie sich pro Aufgabe ein  Tab anlegen, in der die Ein-
*  stellungen per Slider, Button, Checkbox etc. gemacht werden und die zu implemen-
*  tierenden Algorithmen gestatet werden.
*
*****************************************************************************************/

void ImageViewer::generateControlPanels()
{
    // first tab

    m_option_panel1 = new QWidget(this);
    m_option_layout1 = new QVBoxLayout();
    m_option_panel1->setLayout(m_option_layout1);


    applyCross = new QPushButton(this);
    applyCross->setText("Apply cross");

    QFormLayout* crossLayout = new QFormLayout();
    QLabel* crossLabel = new QLabel();
    crossSlider = getSlider(crossLabel, 0, 100);

    crossLayout->addRow(crossLabel, crossSlider);


    QObject::connect(applyCross, SIGNAL (clicked()), this, SLOT (drawRedCross()));

    m_option_layout1->addWidget(applyCross);
    m_option_layout1->addWidget(new QLabel("Cross Size:", this));
    m_option_layout1->addLayout(crossLayout);
    tabWidget->addTab(m_option_panel1,"Uebung1");


    // second tab

    m_option_panel2 = new QWidget(this);
    m_option_layout2 = new QVBoxLayout();
    m_option_panel2->setLayout(m_option_layout2);


    convertToGreyScaleBtn = new QPushButton(this);
    convertToGreyScaleBtn->setText("convert to Greyscale");
    QObject::connect(convertToGreyScaleBtn, SIGNAL (clicked()), this, SLOT (convertToGreyScale()));

    mittlereHelligkeit = new QLabel("Mittlere Helligkeit: ", this);
    varianz = new QLabel("Varianz: ", this);

    histogram = new QLabel(this);
    histoImage = getHistoimage();
    histogram->adjustSize();
    histogram->setPixmap(QPixmap::fromImage(*histoImage));
    histogram->resize(2. * histogram->pixmap()->size());

    cumuHistogram = new QLabel(this);
    cumuHistoImage = getHistoimage();
    cumuHistogram->adjustSize();
    cumuHistogram->setPixmap(QPixmap::fromImage(*cumuHistoImage));
    cumuHistogram->resize(2. * cumuHistogram->pixmap()->size());

    m_option_layout2->addWidget(mittlereHelligkeit);
    m_option_layout2->addWidget(varianz);
    m_option_layout2->addWidget(histogram);
    m_option_layout2->addWidget(cumuHistogram);
    m_option_layout2->addWidget(convertToGreyScaleBtn);

    QLabel* contrastLabel = new QLabel(this);
    contrast = getSlider(contrastLabel, 0, CONTRAST_MAX);
    contrast->setValue(CONTRAST_MW);
    QVBoxLayout* contrastLayout = new QVBoxLayout();
    contrastLayout->addWidget(contrastLabel);
    contrastLayout->addWidget(contrast);


    QLabel* brightnessLabel = new QLabel(this);
    brightness = getSlider(brightnessLabel, -255, 255);
    QVBoxLayout* brightnessLayout = new QVBoxLayout();
    brightnessLayout->addWidget(brightnessLabel);
    brightnessLayout->addWidget(brightness);

    QFormLayout* sliderLayout = new QFormLayout();

    sliderLayout->addRow(new QLabel("Contrast: ", this), contrastLayout);
    sliderLayout->addRow(new QLabel("Brightness: ", this), brightnessLayout);

    connect(contrast, SIGNAL(valueChanged(int)), this, SLOT(changeContrast(int)));
    connect(brightness, SIGNAL(valueChanged(int)), this, SLOT(changeBrightness(int)));

    m_option_layout2->addLayout(sliderLayout);
    
    QHBoxLayout* adjustParamsLayout = new QHBoxLayout();
    
    slowSpinBox = new QDoubleSpinBox();
    slowSpinBox->setRange(0.01, 0.08);
    slowSpinBox->setSingleStep(0.005);
    
    shighSpinBox = new QDoubleSpinBox();
    shighSpinBox->setRange(0.01, 0.08);
    shighSpinBox->setSingleStep(0.005);
    
    adjustParamsLayout->addWidget(slowSpinBox);
    adjustParamsLayout->addWidget(shighSpinBox);
    
    m_option_layout2->addLayout(adjustParamsLayout);
    
    adjustContrastButton = new QPushButton("Adjust Contrast");
    connect(adjustContrastButton, SIGNAL(clicked()), this, SLOT(adjustContrast()));
    m_option_layout2->addWidget(adjustContrastButton);

    tabWidget->addTab(m_option_panel2,"Uebung2");

    tabWidget->show();


    // Hinweis: Es bietet sich an pro Aufgabe jeweils einen solchen Tab zu erstellen

}







/****************************************************************************************
*
*   ab hier kommen technische Details, die nicht notwenig für das Verständnis und die
*   Bearbeitung sind.
*
*
*****************************************************************************************/



void ImageViewer::startLogging()
{
    //LogFile
    logFile.open("log.txt", std::ios::out);
    logFile << "Logging: \n" << std::endl;
}

void ImageViewer::renewLogging()
{
    QFile file("log.txt"); // Create a file handle for the file named
    QString line;
    file.open(QIODevice::ReadOnly); // Open the file

    QTextStream stream( &file ); // Set the stream to read from myFile
    logBrowser->clear();
    while(!stream.atEnd()){

        line = stream.readLine(); // this reads a line (QString) from the file
        logBrowser->append(line);
    }
}


void ImageViewer::resizeEvent(QResizeEvent * event)
{
    QMainWindow::resizeEvent(event);
    centralwidget->setMinimumWidth(width());
        centralwidget->setMinimumHeight(height());
    centralwidget->setMaximumWidth(width());
        centralwidget->setMaximumHeight(height());
    logBrowser->setMinimumWidth(width()-40);
        logBrowser->setMaximumWidth(width()-40);
}

void ImageViewer::updateImageDisplay()
{
    imageLabel->setPixmap(QPixmap::fromImage(*image));
    histogram->setPixmap(QPixmap::fromImage(*histoImage));
    cumuHistogram->setPixmap(QPixmap::fromImage(*cumuHistoImage));

}


void ImageViewer::generateMainGui()
{
    /* Tab widget */
        tabWidget = new QTabWidget(this);
    tabWidget->setObjectName(QStringLiteral("tabWidget"));



    /* Center widget */
    centralwidget = new QWidget(this);
    centralwidget->setObjectName(QStringLiteral("centralwidget"));
    //centralwidget->setFixedSize(200,200);
    //setCentralWidget(centralwidget);

        imageLabel = new QLabel(this);
    imageLabel->setBackgroundRole(QPalette::Base);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true);


    /* Center widget */
    scrollArea = new QScrollArea(this);
    scrollArea->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidget(imageLabel);


        setCentralWidget(scrollArea);

    /* HBox layout */
    QGridLayout* gLayout = new QGridLayout(centralwidget);
    gLayout->setObjectName(QStringLiteral("hboxLayout"));
    gLayout->addWidget(new QLabel(this),1,1);
    gLayout->setVerticalSpacing(50);
    gLayout->addWidget(tabWidget,2,1);
    gLayout->addWidget(scrollArea,2,2);

    logBrowser= new QTextEdit(this);
    logBrowser->setMinimumHeight(100);
    logBrowser->setMaximumHeight(200);
    logBrowser->setMinimumWidth(width());
    logBrowser->setMaximumWidth(width());
    gLayout->addWidget(logBrowser,3,1,1,2);
    gLayout->setVerticalSpacing(50);
}


bool ImageViewer::loadFile(const QString &fileName)
{
    if(image!=NULL)
    {
    delete image;
    image=NULL;
    }

    image = new QImage(fileName);

    if(backupImage != NULL)
    {
        delete backupImage;
    }
    backupImage = new QImage(*image);

    if (image->isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        imageLabel->adjustSize();
        return false;
    }

    scaleFactor = 1.0;


    updateImageDisplay();

    printAct->setEnabled(true);
    fitToWindowAct->setEnabled(true);
    updateActions();

    if (!fitToWindowAct->isChecked())
        imageLabel->adjustSize();

    setWindowFilePath(fileName);
    logFile << "geladen: " << fileName.toStdString().c_str()  << std::endl;
    renewLogging();

    calcValues();

    return true;
}




void ImageViewer::open()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QFileDialog dialog(this, tr("Open File"),
                       picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.first());
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");


    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
}

void ImageViewer::print()
{
    Q_ASSERT(imageLabel->pixmap());
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec()) {
        QPainter painter(&printer);
        QRect rect = painter.viewport();
        QSize size = imageLabel->pixmap()->size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(imageLabel->pixmap()->rect());
        painter.drawPixmap(0, 0, *imageLabel->pixmap());
    }
#endif
}

void ImageViewer::zoomIn()
{
    scaleImage(1.25);
}

void ImageViewer::zoomOut()
{
    scaleImage(0.8);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    bool fitToWindow = fitToWindowAct->isChecked();
    scrollArea->setWidgetResizable(fitToWindow);
    if (!fitToWindow) {
        normalSize();
    }
    updateActions();
}

void ImageViewer::about()
{
    QMessageBox::about(this, tr("About Image Viewer"),
            tr("<p>The <b>Image Viewer</b> example shows how to combine QLabel "
               "and QScrollArea to display an image. QLabel is typically used "
               "for displaying a text, but it can also display an image. "
               "QScrollArea provides a scrolling view around another widget. "
               "If the child widget exceeds the size of the frame, QScrollArea "
               "automatically provides scroll bars. </p><p>The example "
               "demonstrates how QLabel's ability to scale its contents "
               "(QLabel::scaledContents), and QScrollArea's ability to "
               "automatically resize its contents "
               "(QScrollArea::widgetResizable), can be used to implement "
               "zooming and scaling features. </p><p>In addition the example "
               "shows how to use QPainter to print an image.</p>"));
}

void ImageViewer::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcut(tr("Ctrl+P"));
    printAct->setEnabled(false);
    connect(printAct, SIGNAL(triggered()), this, SLOT(print()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom &In (25%)"), this);
    zoomInAct->setShortcut(tr("Ctrl++"));
    zoomInAct->setEnabled(false);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom &Out (25%)"), this);
    zoomOutAct->setShortcut(tr("Ctrl+-"));
    zoomOutAct->setEnabled(false);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("Ctrl+S"));
    normalSizeAct->setEnabled(false);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(false);
    fitToWindowAct->setCheckable(true);
    fitToWindowAct->setShortcut(tr("Ctrl+F"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(printAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addSeparator();
    viewMenu->addAction(fitToWindowAct);

    helpMenu = new QMenu(tr("&Help"), this);
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);
    menuBar()->addMenu(helpMenu);
}

void ImageViewer::updateActions()
{
    zoomInAct->setEnabled(!fitToWindowAct->isChecked());
    zoomOutAct->setEnabled(!fitToWindowAct->isChecked());
    normalSizeAct->setEnabled(!fitToWindowAct->isChecked());
}

void ImageViewer::scaleImage(double factor)
{
    Q_ASSERT(imageLabel->pixmap());
    scaleFactor *= factor;
    imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);

    zoomInAct->setEnabled(scaleFactor < 10.0);
    zoomOutAct->setEnabled(scaleFactor > 0.05);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}

QImage* ImageViewer::getHistoimage()
{
    QImage* returnImage = new QImage(256,100, QImage::Format_RGB32);
    returnImage->fill(QColor(183, 183, 183));

    return returnImage;
}

QSlider* ImageViewer::getSlider(QLabel* valueLabel, int min, int max)
{
        QSlider* returnSlider = new QSlider(this);

        returnSlider->setRange(min, max);
        returnSlider->setValue(0);
        returnSlider->setOrientation(Qt::Horizontal);
        connect(returnSlider, SIGNAL(valueChanged(int)), valueLabel, SLOT(setNum(int)));

        valueLabel->setNum(returnSlider->value());
        return returnSlider;
}

void ImageViewer::changeContrast(int value)
{
    //value=contrast->maximum()-value;
    if(backupImage==NULL||image==NULL)
    {
        return;
    }
    for(int i = 0; i<backupImage->width(); i++)
    {
        for (int j = 0; j<backupImage->height(); j++)
        {
            QColor color = QColor(backupImage->pixel(i, j));
            //QRgb light = color.red() + color.green() + color.blue();

            color.setRed(checkColor((color.red()+brightness->value())*value/(double)CONTRAST_MW));
            color.setGreen(checkColor((color.green()+brightness->value())*value/(double)CONTRAST_MW));
            color.setBlue(checkColor((color.blue()+brightness->value())*value/(double)CONTRAST_MW));
            //QColor c = QColor(light+value,light+value,light+value);
            image->setPixelColor(i, j, color);

        }
    }
    calcValues();
}

void ImageViewer::changeBrightness(int value)
{
    changeContrast(contrast->value());
}

int ImageViewer::checkColor(int value)
{
    if(value > 255)
        value = 255;
    if(value < 0)
        value = 0;
    return value;
}

void ImageViewer::adjustContrast()
{
    
    int n = image->width();
    int m = image->height();
    int entireSize = n*m;
    int slow = static_cast<int>(slowSpinBox->value()*entireSize);
    int shigh = static_cast<int>(shighSpinBox->value()*entireSize);
    int low;
    for (int i = 0; i<cumuHistoVec->size(); i++) {
        if(cumuHistoVec->at(i) >= slow)
        {
            low = i;
            std::cout << "low: " <<low << std::endl;
            break;
        }
    }
    int high;
    std::cout << "VecSize: " <<cumuHistoVec->size() <<std::endl;
    for(int i = cumuHistoVec->size()-1; i>=0; i--)
    {
        if(cumuHistoVec->at(i) <= entireSize - shigh)
        {
            high = i;
            std::cout << "high: " << high << std::endl;
            break;
        }
    }
    
    for(int i=0; i<n; i++)
    {
        for (int j=0; j<m; j++)
        {
            QColor color = QColor(image->pixel(i, j));
            QRgb light = 0.299*color.red() + 0.587*color.green() + 0.144*color.blue();
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
                light = (light-low)*255/(high-low);
            }
            
            QColor c = QColor(light,light,light);
            image->setPixelColor(i, j, c);
            
        }
    }
    
    calcValues();
}
