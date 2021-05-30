#ifndef TUNEMANAGER_H
#define TUNEMANAGER_H

#include "opencv2/opencv.hpp"
#include "opencv2/cvconfig.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/videoio.hpp"

#include <QWidget>
#include <QString>
#include <QImage>
#include <QPixelFormat>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>

#include "imageprocessor.h"
#include "videoprocessor.h"

using namespace  cv;

enum FileType{
    ImageFile = 0,
    VideoFile,
    NotAssigned
};

class TuneManager : public QWidget
{
    Q_OBJECT
public:
    explicit TuneManager(QWidget *parent = nullptr);

    bool getImageLoaded(){return this->ImageLoaded;}
    bool getVideoLoaded(){return this->VideoLoaded;}

private:

    QString exportLocation = "";

    /* Tune Parameters */
    int Pixel_Size;
    int Color_Amount;
    int Saturation_Amount;
    int Smoothing_Factor;
    int Smoothing_Iteration;
    bool InvisiblePixelCalculation;
    int QuantizerIndex;
    int SigmaValue;
    bool SmoothingActive;

    float VideoSlider;

    QString FileLocation;

    // IMAGES
    std::vector<QImage> Frames;
    QImage TopLevelImage;
    bool ImageLoaded = false;

    // VIDEOS
    VideoCapture* videoCap;
    Mat* MatFrame;
    QImage videoFrameProcessed;
    long long totalFrame;
    float frame_rate;
    int frame_width;
    int frame_height;
    long long video_duration;
    bool VideoLoaded = false;
    VideoProcessor* recorder = nullptr;


    void convertMatToQImage(QImage& image, Mat* mat_ptr);

    ImageProcessor* workerThread;

    // PRIVATE FUNCTIONS

signals:
    /*Send image to the GUI Dock Widget to display */
    void DeliverImage(QImage frame);
    void ReturnTopLevelImage(QImage image);
    void DeliverSliderTime(QString value);

    void Video_Bar_Echo(int value);
    void Video_Export_Over();

public slots:
    /* Tune parameter update slots */
    void PixelSizeUpdate(int value);
    void ColorAmountUpdate (int value);
    void SaturationUpdate(int value);
    void SmoothingFactorUpdate(int value);
    void SmoothingIterationUpdate(int value);
    void InvisiblePixelCalculationUpdate(bool value);
    void QuantizerIndexUpdate(int value);
    void SigmaValueUpdate(float value);
    void SmoothingActiveUpdate(bool value);

    void VideoSliderUpdate(float value);

    /* New file selected */
    void FileLocationUpdate(QString value);

    /* Update button pressed */
    void updateRequested();

    /* Image Processor thread emitted signal */
    void threadFinished(QImage processedImage);
    void videoThreadFinished(QImage processedFrame);

    void exportRequested();
    void videoExportRequested();
    void getFileLocation(QString value);

    void EchoVideoBar(int value);
    void EchoVideoExportOver();

};

#endif // TUNEMANAGER_H
