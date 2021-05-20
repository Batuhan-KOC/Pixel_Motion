#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QThread>
#include <QImage>
#include <QImageWriter>
#include "Quantizers/mediancutquantizer.h"
#include "Quantizers/octreequantizer.h"
#include "gaussianblur.h"

enum QuantizerIndex{
    NO_QUANTIZATION = 0,
    MEDIAN_CUT_QUANTIZER,
    CENTER_CUT_QUANTIZER,
    OCTREE_QUANTIZER
};

class ImageProcessor : public QThread
{
    Q_OBJECT
public:
    void run() override;

    void initializeProcessor(QImage image,
                             int Pixel_Size,
                             int Color_Amount,
                             int Saturation_Amount,
                             int Smoothing_Factor,
                             int Smoothing_Iteration,
                             bool InvisiblePixelCalculation,
                             int QuantizerIndex,
                             float SigmaValue,
                             bool SmoothingActive);
private:
    QImage image;
    QImage processedImage;
    int Pixel_Size;
    int Color_Amount;
    int Saturation_Amount;
    int Smoothing_Factor;
    int Smoothing_Iteration;
    bool InvisiblePixelCalculation;
    int QuantizerIndex;
    float SigmaValue;
    bool SmoothingActive;

signals:
    void sendProcessedImage(QImage frame);
};

#endif // IMAGEPROCESSOR_H
