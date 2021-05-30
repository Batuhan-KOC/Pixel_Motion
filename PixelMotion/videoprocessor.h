#ifndef VIDEOPROCESSOR_H
#define VIDEOPROCESSOR_H

#include <QThread>
#include <QFile>
#include "opencv2/opencv.hpp"
#include "opencv2/cvconfig.h"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/videoio.hpp"
#include "imageprocessor.h"

using namespace cv;

class VideoProcessor: public QThread
{
    Q_OBJECT
public:

    void run() override;

    void initImageProcessor( int Pixel_Size,
                             int Color_Amount,
                             int Saturation_Amount,
                             int Smoothing_Factor,
                             int Smoothing_Iteration,
                             bool InvisiblePixelCalculation,
                             int QuantizerIndex,
                             float SigmaValue,
                             bool SmoothingActive);

    void initVideo(QString folderToSave,
                   long long Total_Frame_Count,
                   int frame_width,
                   int frame_height,
                   int frame_rate,
                   cv::VideoCapture *VideoCap);

private:

    QString folderToSave;
    long long Total_Frame_Count;
    int frame_width;
    int frame_height;
    int frame_rate;
    cv::VideoCapture* VideoCap;
    cv::VideoWriter* VideoWriter;
    QImage processedFrame,processedFrame2,processedFrame3,processedFrame4;

    int Pixel_Size;
    int Color_Amount;
    int Saturation_Amount;
    int Smoothing_Factor;
    int Smoothing_Iteration;
    bool InvisiblePixelCalculation;
    int QuantizerIndex;
    float SigmaValue;
    bool SmoothingActive;

    void convertMatToQImage(QImage& image, cv::Mat* mat_ptr);
    void convertQImageToMat(QImage& image, cv::Mat* mat_ptr);

signals:
    void sendLoadingBar(int value);
    void exportOver();

private slots:
    void overwriteImage(QImage image);
    void overwriteImage2(QImage image);
    void overwriteImage3(QImage image);
    void overwriteImage4(QImage image);
};

#endif // VIDEOPROCESSOR_H
