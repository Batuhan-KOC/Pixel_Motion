#include "tunemanager.h"

TuneManager::TuneManager(QWidget *parent) : QWidget(parent)
{
    // Default UI Parameters
    Pixel_Size = 1;
    Color_Amount = 256;
    Saturation_Amount = 0;
    Smoothing_Factor = 1;
    Smoothing_Iteration = 1;
    InvisiblePixelCalculation = false;
    QuantizerIndex = 0;
    SigmaValue = 1;
    SmoothingActive = false;

    VideoSlider = 0.0;
}


void TuneManager::PixelSizeUpdate(int value)
{
    this->Pixel_Size = value;
}

void TuneManager::ColorAmountUpdate(int value)
{
    this->Color_Amount = value;
}

void TuneManager::SaturationUpdate(int value)
{
   this->Saturation_Amount = value;
}

void TuneManager::SmoothingFactorUpdate(int value)
{
    this->Smoothing_Factor = value;
}

void TuneManager::SmoothingIterationUpdate(int value)
{
    this->Smoothing_Iteration = value;
}

void TuneManager::InvisiblePixelCalculationUpdate(bool value)
{
    this->InvisiblePixelCalculation = value;
}

void TuneManager::QuantizerIndexUpdate(int value)
{
    this->QuantizerIndex = value;
}

void TuneManager::SigmaValueUpdate(float value)
{
    this->SigmaValue = value;
}

void TuneManager::SmoothingActiveUpdate(bool value)
{
    this->SmoothingActive = value;
}

void TuneManager::VideoSliderUpdate(float value)
{
    this->VideoSlider = value;
}

void TuneManager::FileLocationUpdate(QString value)
{
    this->FileLocation = value;

    QString fileFormat = value.split('.').last();

    if(fileFormat == "jpeg" || fileFormat == "png" || fileFormat == "jpg")
    {
        QImage newImage = QImage(FileLocation);

        if(!newImage.isNull())
        {
            Frames.push_back(newImage);

            emit DeliverImage(newImage);
        }
    }
    else if(fileFormat == "mp4" || fileFormat == "mov" || fileFormat == "wmv" || fileFormat == "avi" || fileFormat == "mkv")
    {
    }
}

void TuneManager::updateRequested()
{
    if(Frames.size() > 0)
    {
        QImage _image = Frames.back();                  // Capture the top image from the Frames vector
        this->workerThread = new ImageProcessor();      // Create an instance of thread class
        this->workerThread->initializeProcessor(_image, // Initialize thread with tune parameters
                                                this->Pixel_Size,
                                                this->Color_Amount,
                                                this->Saturation_Amount,
                                                this->Smoothing_Factor,
                                                this->Smoothing_Iteration,
                                                this->InvisiblePixelCalculation,
                                                this->QuantizerIndex,
                                                this->SigmaValue,
                                                this->SmoothingActive);

        connect(this->workerThread, &QThread::finished,this->workerThread,&QThread::deleteLater);   // Auto call delete when thread is finished
        connect(this->workerThread,&ImageProcessor::sendProcessedImage,this,&TuneManager::threadFinished);  // Get processed image when thread response

        this->workerThread->start();
    }
}

void TuneManager::threadFinished(QImage processedImage)
{
    // Thread responded with a new image

    // add this image to the frames stack
    //this->Frames.push_back(processedImage);

    // Send image to the GUI
    this->TopLevelImage = QImage(processedImage);
    emit DeliverImage(processedImage);
}

void TuneManager::exportRequested()
{
    emit ReturnTopLevelImage(this->TopLevelImage);
}
