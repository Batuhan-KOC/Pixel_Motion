#include "imageprocessor.h"
#include <QDebug>

void ImageProcessor::run()
{
    std::vector<QImage> ImagesOnProcess;

    QImage newImage = QImage(this->image.width() ,this->image.height() ,QImage::Format_RGBA64);

    ImagesOnProcess.push_back(newImage);

    /******************************************************************
     * PIXELATION STAGE
    *******************************************************************/

    int Width_Index = 0;
    int Height_Index = 0;

    bool hasAlphaChannel = this->image.hasAlphaChannel();

    while(true)
    {
        int R=0, G=0, B=0, A=0;
        int PixelsInSquare = 0;

        std::vector<std::vector<bool>> effectMask(this->Pixel_Size, std::vector<bool>(Pixel_Size,false));

        // Sum colors
        for(int x_idx = 0 ; x_idx < Pixel_Size ; x_idx++)
        {

            if(Width_Index + x_idx >= this->image.width())
                continue;

            for(int y_idx = 0 ; y_idx < Pixel_Size ; y_idx++)
            {
                if(Height_Index + y_idx >= this->image.height())
                    continue;

                QColor pixelColor = this->image.pixelColor(Width_Index + x_idx, Height_Index + y_idx);

                bool PixelValid = true;

                if(!this->InvisiblePixelCalculation)
                {
                    if(pixelColor.red()   == 0 &&
                       pixelColor.green() == 0 &&
                       pixelColor.blue()  == 0 &&
                       pixelColor.alpha() == 0)
                    {
                        PixelValid = false;
                    }
                }

                if(PixelValid)
                {
                    R += pixelColor.red();
                    G += pixelColor.green();
                    B += pixelColor.blue();

                    if (hasAlphaChannel)
                        A += pixelColor.alpha();

                    PixelsInSquare ++;
                }
                effectMask.at(x_idx).at(y_idx) = true;
            }
        }

        // Find average color
        if(PixelsInSquare > 0)
        {
            R = R / PixelsInSquare;
            G = G / PixelsInSquare;
            B = B / PixelsInSquare;
            if(hasAlphaChannel)
                A = A / PixelsInSquare;
        }
        // Apply Color to image
        int x_idx=0, y_idx=0;
        for(std::vector<bool> Line : effectMask)
        {
            y_idx = 0;

            for(bool MaskActive : Line)
            {
                if(MaskActive)
                {
                    QColor pixelColor = QColor(R,G,B,A);
                    newImage.setPixelColor(Width_Index + x_idx,
                                           Height_Index + y_idx,
                                           pixelColor);
                }

                y_idx++;
            }

            x_idx++;
        }

        bool X_overflow = false, Y_overflow = false;
        if(Width_Index + Pixel_Size >= this->image.width())
        {
            X_overflow = true;
            Width_Index = 0;
        }
        else
        {
            Width_Index += Pixel_Size;
        }

        if(X_overflow)
        {
            if(Height_Index + Pixel_Size >= this->image.height())
            {
                Y_overflow = true;
                Height_Index = 0;
            }
            else
            {
                Height_Index += Pixel_Size;
            }
        }

        if(X_overflow && Y_overflow)
        {
            break;
        }

    }

    ImagesOnProcess.push_back(newImage);

    /******************************************************************
     * COLOR DEGRADATION STAGE
    *******************************************************************/

    if(this->QuantizerIndex == NO_QUANTIZATION)
    {
        // Do Nothing
    }
    else if(this->QuantizerIndex == MEDIAN_CUT_QUANTIZER)
    {
        MedianCutQuantizer *_medianCutQuantizer = new MedianCutQuantizer(MedianCutQuantizer::QuantizerTypeSelection::MEDIAN);
        QImage quantizedImage = QImage(_medianCutQuantizer->MedianCutQuantize(ImagesOnProcess.back(),this->Color_Amount));
        ImagesOnProcess.push_back(quantizedImage);
        delete _medianCutQuantizer;
    }
    else if(this->QuantizerIndex == CENTER_CUT_QUANTIZER)
    {
        MedianCutQuantizer *_medianCutQuantizer = new MedianCutQuantizer(MedianCutQuantizer::QuantizerTypeSelection::CENTER);
        QImage quantizedImage = QImage(_medianCutQuantizer->MedianCutQuantize(ImagesOnProcess.back(),this->Color_Amount));
        ImagesOnProcess.push_back(quantizedImage);
        delete _medianCutQuantizer;
    }
    else if(this->QuantizerIndex == OCTREE_QUANTIZER)
    {
        OctreeQuantizer *_octreeQuantizer = new OctreeQuantizer();
        _octreeQuantizer->initQuantizer(ImagesOnProcess.back(), this->Color_Amount);
        QImage quantizedImage = _octreeQuantizer->OctreeQuantize();
        ImagesOnProcess.push_back(quantizedImage);
        delete _octreeQuantizer;
    }

    /******************************************************************
     * SATURATION STAGE
    *******************************************************************/

    if(this->Saturation_Amount != 0)
    {
        int m_width = ImagesOnProcess.back().width();
        int m_height = ImagesOnProcess.back().height();
        QImage SaturatedImage = QImage(ImagesOnProcess.back());

        for(int x_idx = 0 ; x_idx < m_width ; x_idx++)
        {
            for(int y_idx = 0 ; y_idx < m_height ; y_idx ++)
            {
                QColor _pixelColor = SaturatedImage.pixelColor(x_idx, y_idx);

                _pixelColor = _pixelColor.convertTo(QColor::Hsv);

                int saturation = _pixelColor.saturation();
                saturation = int(float(saturation) * (float(100 + this->Saturation_Amount) / 100.0));

                if(saturation > 255)
                    saturation = 255;

                _pixelColor.setHsv(_pixelColor.hue(), saturation, _pixelColor.value(), _pixelColor.alpha());
                SaturatedImage.setPixelColor(x_idx, y_idx, _pixelColor);
            }
        }

        ImagesOnProcess.push_back(SaturatedImage);
    }

    /******************************************************************
     * SMOOTHING STAGE
    *******************************************************************/

    if(this->SmoothingActive)
    {
        GaussianBlurEffect *gaussianObj = new GaussianBlurEffect();

        gaussianObj->initBlur(this->Smoothing_Factor,
                              this->SigmaValue,
                              this->Smoothing_Iteration);

        QImage SmoothenImage = gaussianObj->ApplyBlur(QImage(ImagesOnProcess.back()));

        ImagesOnProcess.push_back(SmoothenImage);
    }

    emit sendProcessedImage(ImagesOnProcess.back());

    this->processedImage = QImage(ImagesOnProcess.back());
}

void ImageProcessor::initializeProcessor(QImage image,
                                         int Pixel_Size,
                                         int Color_Amount,
                                         int Saturation_Amount,
                                         int Smoothing_Factor,
                                         int Smoothing_Iteration,
                                         bool InvisiblePixelCalculation,
                                         int QuantizerIndex,
                                         float SigmaValue,
                                         bool SmoothingActive)
{
    this->image = image;

    this->Pixel_Size = Pixel_Size;
    this->Color_Amount = Color_Amount;
    this->Saturation_Amount = Saturation_Amount;
    this->Smoothing_Factor = Smoothing_Factor;
    this->Smoothing_Iteration = Smoothing_Iteration;
    this->InvisiblePixelCalculation = InvisiblePixelCalculation;
    this->QuantizerIndex = QuantizerIndex;
    this->SigmaValue = SigmaValue;
    this->SmoothingActive = SmoothingActive;
}

