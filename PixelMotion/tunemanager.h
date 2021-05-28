#ifndef TUNEMANAGER_H
#define TUNEMANAGER_H

#include <QWidget>
#include <QString>
#include <QImage>
#include <QPixelFormat>

#include "imageprocessor.h"

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

private:
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

    // VIDEOS
    QImage videoFrameProcessed;
    long long totalFrame;
    float frame_rate;

    int frame_width;
    int frame_height;

    ImageProcessor* workerThread;

    // PRIVATE FUNCTIONS

signals:
    /*Send image to the GUI Dock Widget to display */
    void DeliverImage(QImage frame);
    void ReturnTopLevelImage(QImage image);

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

    void exportRequested();

};

#endif // TUNEMANAGER_H
