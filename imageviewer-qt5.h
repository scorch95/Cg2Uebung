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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include "ImageObj.h"
#include "fstream"

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;
class QTabWidget;
class QPushButton;
class QSpinBox;
class QSlider;
class QDoubleSpinBox;
class QTableWidget;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

 private:

     // Beispiel für GUI Elemente
     QWidget*	m_option_panel1;
     QVBoxLayout*    m_option_layout1;

     QWidget*	m_option_panel2;
     QVBoxLayout*    m_option_layout2;
    
    QWidget*    m_option_panel3;
    QVBoxLayout*    m_option_layout3;
    
    QWidget* m_option_panel4;
    QHBoxLayout* m_option_layout4;

    

     // hier können weitere GUI Objekte hin wie Buttons Slider etc. 

     // Uebung 1
    QSlider* crossSlider;
    QPushButton* applyCross;
    
    // Uebung 2
    QLabel* varianz;
    QLabel* mittlereHelligkeit;
    //QPushButton* convertToGreyScaleBtn;
    QPushButton* resetBtn;
    QLabel* histogram;
    QLabel* cumuHistogram;
    QImage* histoImage;
    QImage* cumuHistoImage;
    QSlider* contrast;
    QSlider* brightness;
    QPushButton* adjustContrastButton;
    QVector<int>* histoVec;
    QVector<int>* cumuHistoVec;
    QDoubleSpinBox* slowSpinBox;
    QDoubleSpinBox* shighSpinBox;
    
    // Uebung3
    QPushButton* histoAdjustButton;
    QLabel* secCumuHistogram;
    QLabel* secPicCumuHistogram;
    QPushButton* linHistoAdjust;
    QPushButton* openSecButton;
    QPushButton* refHistoButton;
    QPushButton* gaussButton;
    //QPushButton* yuvButton;
    QSpinBox* sigma;
    int currentImage;
    
    //Uebung4
    QTableWidget* matrixWidget;
    QPushButton* applyFilterMatrix;
    QSpinBox* matrixRows;
    QSpinBox* matrixColumns;

 private slots:

     void drawRedCross();
     //void convertToGreyScale();
    void reset();
    
    void openSecImage();
    
    void changeBrightness(int value);
    void changeContrast(int value);
    
    void adjustContrast();
    void adjustHistoLin();
    void applyRefHisto();
    void adjustGauss();
    //void yuvConvert();

     void open();
     void print();
     void zoomIn();
     void zoomOut();
     void normalSize();
     void fitToWindow();
     void about();

    
    void rowsChanged(int rows);
    void columnsChanged(int columns);
    
    void applyMatrixFilter();





public:
     ImageViewer();
     virtual ~ImageViewer();
    bool loadFile(const QString &fileName);
     void updateImageDisplay();

protected:

    void resizeEvent(QResizeEvent * event);

 private:

    // in diesen Beiden Methoden sind Änderungen nötig bzw. sie dienen als
    // Vorlage für eigene Methoden.
    void generateControlPanels();


    // Ab hier technische Details die nicht für das Verständnis notwendig sind.
    void startLogging();
    void generateMainGui(); 
    
    void createActions();
    void createMenus();
    void updateActions();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void renewLogging();
    void calcValues();
    
    QSlider* getSlider(QLabel* valueLabel, int min, int max);
    
    int checkColor(int value);
    

    QTabWidget* tabWidget;
    QTextEdit* logBrowser;
    QWidget* centralwidget;
    QLabel* imageLabel;
    QLabel* imageLabel2;
    QScrollArea* scrollArea;
    double scaleFactor;

    ImageObj* imgObj;
    ImageObj* imgObj2;

    std::fstream logFile;

    QAction *openAct;
    QAction *printAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
};

#endif
