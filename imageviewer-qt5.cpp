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
#include "ImageObj.h"

#include "imageviewer-qt5.h"

ImageViewer::ImageViewer()
{
    histoImage = nullptr;
    cumuHistoImage = nullptr;
    histoVec = nullptr;
    cumuHistoVec=nullptr;
    imgObj=nullptr;
    resize(1600, 600);
    currentImage = 1;
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
    if(imgObj!=nullptr)
    {
        delete imgObj;
    }
    
}

void ImageViewer::drawRedCross()
{
    imgObj->drawRedCross(crossSlider->value());
    updateImageDisplay();
}

void ImageViewer::calcValues()
{
    imgObj->calcValues();

    updateImageDisplay();
    mittlereHelligkeit->setText("Mittlere Helligkeit:  "+QString::number(imgObj->getBrightness()));
    varianz->setText("Varianz: "+QString::number(imgObj->getVarianz()));

}

/*void ImageViewer::convertToGreyScale()
{

}*/

void ImageViewer::changeContrast(int value)
{
    imgObj->changeContrast(value);
    updateImageDisplay();
}

void ImageViewer::changeBrightness(int value)
{
    imgObj->changeBrightness(value);
    updateImageDisplay();
}

void ImageViewer::adjustContrast()
{
    imgObj->adjustContrast(slowSpinBox->value(), shighSpinBox->value());
    updateImageDisplay();
}

void ImageViewer::adjustHistoLin()
{
    imgObj->adjustHistoLin();
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


    //convertToGreyScaleBtn = new QPushButton(this);
    //convertToGreyScaleBtn->setText("convert to Greyscale");
    //QObject::connect(convertToGreyScaleBtn, SIGNAL (clicked()), this, SLOT (convertToGreyScale()));

    mittlereHelligkeit = new QLabel("Mittlere Helligkeit: ", this);
    varianz = new QLabel("Varianz: ", this);

    histogram = new QLabel(this);
    histoImage = imgObj->getBlankHistoimage();
    histogram->adjustSize();
    histogram->setPixmap(QPixmap::fromImage(*histoImage));
    histogram->resize(2. * histogram->pixmap()->size());

    cumuHistogram = new QLabel(this);
    cumuHistoImage = imgObj->getBlankHistoimage();
    cumuHistogram->adjustSize();
    cumuHistogram->setPixmap(QPixmap::fromImage(*cumuHistoImage));
    cumuHistogram->resize(2. * cumuHistogram->pixmap()->size());

    m_option_layout2->addWidget(mittlereHelligkeit);
    m_option_layout2->addWidget(varianz);
    m_option_layout2->addWidget(histogram);
    m_option_layout2->addWidget(cumuHistogram);
    //m_option_layout2->addWidget(convertToGreyScaleBtn);

    QLabel* contrastLabel = new QLabel(this);
    contrast = getSlider(contrastLabel, 0, ImageObj::CONTRAST_MAX);
    contrast->setValue(ImageObj::CONTRAST_MW);
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
    //m_option_layout2->addWidget(resetBtn);
    
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
    
    m_option_panel3 = new QWidget(this);
    m_option_layout3 = new QVBoxLayout();
    m_option_panel3->setLayout(m_option_layout3);
    
    imageLabel2 = new QLabel();
    m_option_layout3->addWidget(imageLabel2);
    imageLabel2->setMaximumWidth(m_option_panel3->width());
    
    openSecButton = new QPushButton("Open");
    connect(openSecButton, SIGNAL(clicked()), this, SLOT(openSecImage()));
    m_option_layout3->addWidget(openSecButton);
    
    secPicCumuHistogram = new QLabel();
    secCumuHistogram = new QLabel();
    secPicCumuHistogram->setPixmap(QPixmap::fromImage(*cumuHistoImage));
    secCumuHistogram->setPixmap(QPixmap::fromImage(*cumuHistoImage));
    
    QHBoxLayout* histoLayout = new QHBoxLayout();
    histoLayout->addWidget(secPicCumuHistogram);
    histoLayout->addWidget(secCumuHistogram);
    
    m_option_layout3->addLayout(histoLayout);
    histoAdjustButton = new QPushButton("Linearer Histogramausgleich");
    connect(histoAdjustButton, SIGNAL(clicked()), this, SLOT(adjustHistoLin()));
    
    refHistoButton = new QPushButton("Referenzausgleich");
    connect(refHistoButton, SIGNAL(clicked()), this, SLOT(applyRefHisto()));
    refHistoButton->setEnabled(false);
    
    sigma = new QSpinBox();
    sigma->setRange(0, 150);
    sigma->setValue(50);
    gaussButton = new QPushButton("Gauss ausgleich");
    connect(gaussButton, SIGNAL(clicked()), this, SLOT(adjustGauss()));
    
    //yuvButton = new QPushButton("YUV Convert");
    //connect(yuvButton, SIGNAL(clicked()), this, SLOT(yuvConvert()));

    m_option_layout3->addWidget(histoAdjustButton);
    m_option_layout3->addWidget(refHistoButton);
    m_option_layout3->addWidget(sigma);
    m_option_layout3->addWidget(gaussButton);
    //m_option_layout3->addWidget(yuvButton);
    
    tabWidget->addTab(m_option_panel3, "Uebung3");
    
    m_option_panel4 = new QWidget(this);
    m_option_layout4 = new QHBoxLayout();
    m_option_panel4->setLayout(m_option_layout4);
    
    matrixWidget = new QTableWidget(3,3);
    matrixWidget->horizontalHeader()->setDefaultSectionSize(28);
    //matrixWidget->horizontalHeader()->setDefaultSectionSize(10);
    m_option_layout4->addWidget(matrixWidget);
    
    QWidget* optionWidgetTab4 = new QWidget(m_option_panel4);
    QVBoxLayout* optionLayoutTab4 = new QVBoxLayout();
    optionWidgetTab4->setLayout(optionLayoutTab4);
    
    QLabel* rowsLabel = new QLabel("Rows");
    matrixRows = new QSpinBox();
    matrixRows->setMinimum(3);
    matrixRows->setMaximum(11);
    matrixRows->setSingleStep(2);
    matrixRows->setValue(3);
    connect(matrixRows, SIGNAL(valueChanged(int)), this, SLOT(rowsChanged(int)));
    
    QLabel* columnsLabel = new QLabel("Columns");
    matrixColumns = new QSpinBox();
    matrixColumns->setMinimum(3);
    matrixColumns->setMaximum(11);
    matrixColumns->setSingleStep(2);
    matrixColumns->setValue(3);
    connect(matrixColumns, SIGNAL(valueChanged(int)), this, SLOT(columnsChanged(int)));
    
    QFormLayout* rowClmLayout = new QFormLayout();
    rowClmLayout->addRow(rowsLabel, matrixRows);
    rowClmLayout->addRow(columnsLabel, matrixColumns);
    
    optionLayoutTab4->addLayout(rowClmLayout);
    
    
    applyFilterMatrix = new QPushButton("Apply Filtermatrix");
    optionLayoutTab4->addWidget(applyFilterMatrix);
    
    
    m_option_layout4->addWidget(optionWidgetTab4);
    
    tabWidget->addTab(m_option_panel4, "Uebung4");
    

    tabWidget->show();

    
    //vor dem laden eines Bilds sollte nichts ausgewählt werden können
    crossSlider->setEnabled(false);
    applyCross->setEnabled(false);
    resetBtn->setEnabled(false);
    contrast->setEnabled(false);
    brightness->setEnabled(false);
    adjustContrastButton->setEnabled(false);
    slowSpinBox->setEnabled(false);
    shighSpinBox->setEnabled(false);
    histoAdjustButton->setEnabled(false);
    gaussButton->setEnabled(false);
    sigma->setEnabled(false);


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
    if(imgObj != nullptr)
    {
        histoImage = imgObj->getHistoImage();
        imageLabel->setPixmap(QPixmap::fromImage(*imgObj->getImage()));
        cumuHistoImage = imgObj->getCumuHistoImage();

        histogram->setPixmap(QPixmap::fromImage(*histoImage));
        cumuHistogram->setPixmap(QPixmap::fromImage(*cumuHistoImage));
        secCumuHistogram->setPixmap(QPixmap::fromImage(*cumuHistoImage));
    }
    
    if(imgObj2 != nullptr)
    {
        imageLabel2->setPixmap(QPixmap::fromImage(*imgObj2->getImage()));
        secPicCumuHistogram->setPixmap(QPixmap::fromImage(*(imgObj2->getCumuHistoImage())));
    }

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

    resetBtn = new QPushButton("Reset");
    connect(resetBtn, SIGNAL(clicked()), this, SLOT(reset()));
    
    /* HBox layout */
    QGridLayout* gLayout = new QGridLayout(centralwidget);
    gLayout->setObjectName(QStringLiteral("hboxLayout"));
    gLayout->addWidget(new QLabel(this),1,1);
    gLayout->setVerticalSpacing(50);
    gLayout->addWidget(tabWidget,2,1);
    gLayout->addWidget(resetBtn,3,1);
    gLayout->addWidget(scrollArea,2,2);

    logBrowser= new QTextEdit(this);
    logBrowser->setMinimumHeight(100);
    logBrowser->setMaximumHeight(200);
    logBrowser->setMinimumWidth(width());
    logBrowser->setMaximumWidth(width());
    gLayout->addWidget(logBrowser,4,1,1,2);
    gLayout->setVerticalSpacing(15);
}


bool ImageViewer::loadFile(const QString &fileName)
{
    if (currentImage == 1)
    {
        if(imgObj != nullptr)
        {
            delete imgObj;
        }
        
        imgObj = new ImageObj(new QImage(fileName));
        //image = imgObj->getImage();
        
        
        if (imgObj->getImage()->isNull())
        {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
            setWindowFilePath(QString());
            imageLabel->setPixmap(QPixmap());
            imageLabel->adjustSize();
            return false;
        }
    }
    
    if (currentImage == 2)
    {
        if(imgObj2 != nullptr)
        {
            delete imgObj2;
        }
        
        imgObj2 = new ImageObj(new QImage(fileName));
        //image = imgObj->getImage();
        
        
        if (imgObj2->getImage()->isNull())
        {
            QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                     tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
            setWindowFilePath(QString());
            imageLabel->setPixmap(QPixmap());
            imageLabel->adjustSize();
            return false;
        }
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

void ImageViewer::openSecImage()
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

    currentImage = 2;
    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())) {}
    currentImage = 1;
    
    refHistoButton->setEnabled(true);
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
    
    crossSlider->setEnabled(true);
    applyCross->setEnabled(true);
    resetBtn->setEnabled(true);
    contrast->setEnabled(true);
    brightness->setEnabled(true);
    adjustContrastButton->setEnabled(true);
    slowSpinBox->setEnabled(true);
    shighSpinBox->setEnabled(true);
    histoAdjustButton->setEnabled(true);
    gaussButton->setEnabled(true);
    sigma->setEnabled(true);

}

void ImageViewer::applyRefHisto()
{
    if(imgObj != nullptr && imgObj2 != nullptr)
    {
        imgObj->applyRefHisto(imgObj2->getCumuHistoVec());
        updateImageDisplay();
    }
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

void ImageViewer::adjustGauss()
{
    imgObj->getGaussCumu(sigma->value());
    updateImageDisplay();
}

/*void ImageViewer:: yuvConvert()
{
    imgObj->yuvConvert();
    updateImageDisplay();
}*/


void ImageViewer::reset()
{
    contrast->setValue(ImageObj::CONTRAST_MW);
    brightness->setValue(0);
    
    imgObj->resetToCopyImage();
    
    updateImageDisplay();
}


void ImageViewer::rowsChanged(int rows)
{
    matrixWidget->setRowCount(rows);
}

void ImageViewer::columnsChanged(int columns)
{
    matrixWidget->setColumnCount(columns);
}
