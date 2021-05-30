#include "tunemanager.h"
#include <QDebug>
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

void TuneManager::convertMatToQImage(QImage &image, Mat *mat_ptr)
{
    for(int x_idx = 0 ; x_idx < this->frame_width; x_idx++)
    {
        for(int y_idx = 0 ; y_idx < this->frame_height; y_idx++)
        {
            int R,G,B;
            B = mat_ptr->at<cv::Vec3b>(y_idx,x_idx)[0]; // Blue
            G = mat_ptr->at<cv::Vec3b>(y_idx,x_idx)[1]; // Green
            R = mat_ptr->at<cv::Vec3b>(y_idx,x_idx)[2]; // Red

            QColor _pixelColor = QColor(0,0,0,255);
            _pixelColor.setRed(R);
            _pixelColor.setGreen(G);
            _pixelColor.setBlue(B);

            image.setPixelColor(x_idx, y_idx, _pixelColor);
        }
    }
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

    if(this->VideoLoaded)
    {
        // Calculate duration in HH:MM:SS format and send it to UI

        // Total Duration in seconds is this->video_duration

        long long new_duration = float(this->video_duration)*(value/100.0);

        QString HH,MM,SS,TotDuration;
        int hours = (int) new_duration / 3600;
        int remainder = (int) new_duration - hours*3600;
        int mins = remainder / 60;
        remainder = remainder - mins*60;
        int seconds = remainder;

        SS = QString::number(seconds).rightJustified(2,'0');
        MM = QString::number(mins).rightJustified(2,'0');
        HH = QString::number(hours).rightJustified(2,'0');

        TotDuration = HH + "h : " + MM + "m : " + SS +"s";

        emit DeliverSliderTime(TotDuration);

        // Then find the corresponding image

        long long new_frameIndex = float(this->totalFrame) * (value/100.0);
        if(new_frameIndex < 0)
            new_frameIndex = 0;
        if(new_frameIndex >= totalFrame)
            new_frameIndex = totalFrame - 1;

        this->videoCap->set(CAP_PROP_POS_FRAMES, new_frameIndex);

        this->videoCap->read(*this->MatFrame);

        convertMatToQImage(videoFrameProcessed, MatFrame);

        emit DeliverImage(videoFrameProcessed);
    }
}

void TuneManager::FileLocationUpdate(QString value)
{
    this->FileLocation = value;

    QString fileFormat = value.split('.').last();

    if(fileFormat == "jpeg" || fileFormat == "png" || fileFormat == "jpg")
    {
        QImage newImage = QImage(FileLocation);

        this->ImageLoaded = false;
        this->VideoLoaded = false;

        if(!newImage.isNull())
        {
            this->ImageLoaded = true;
            Frames.push_back(newImage);

            emit DeliverImage(newImage);
        }
    }
    else if(fileFormat == "mp4" || fileFormat == "mov" || fileFormat == "wmv" || fileFormat == "avi" || fileFormat == "mkv")
    {
        this->videoCap = new VideoCapture(value.toStdString());
        MatFrame = new Mat();

        this->ImageLoaded = false;
        this->VideoLoaded = false;

        this->VideoLoaded = this->videoCap->isOpened();

        if(this->VideoLoaded)
        {

            *(videoCap) >> *(MatFrame);

            this->frame_width = videoCap->get(CAP_PROP_FRAME_WIDTH);
            this->frame_height = videoCap->get(CAP_PROP_FRAME_HEIGHT);
            this->frame_rate = videoCap->get(CAP_PROP_FPS);
            this->totalFrame = videoCap->get(CAP_PROP_FRAME_COUNT);

            this->video_duration = totalFrame / frame_rate;
            qDebug()<< "Video Duration : " << video_duration;
            qDebug()<< "Total Frame    : " << totalFrame;
            qDebug()<< "FPS            : " << frame_rate;

            videoFrameProcessed = QImage(this->frame_width, this->frame_height,QImage::Format_RGBA8888);

            convertMatToQImage(videoFrameProcessed, MatFrame);

            emit DeliverImage(videoFrameProcessed);
        }

        // TODO : QDialog Video Not Loaded Error


    }
}

void TuneManager::updateRequested()
{

    if(ImageLoaded)
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
    else if(VideoLoaded)
    {
        QImage _image = videoFrameProcessed;                  // Capture the top image from the Frames vector
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

void TuneManager::videoThreadFinished(QImage processedFrame)
{

}

void TuneManager::exportRequested()
{
    emit ReturnTopLevelImage(this->TopLevelImage);
}

void TuneManager::videoExportRequested()
{
    if(this->exportLocation.isEmpty())
    {
        QMessageBox msgBox;
        msgBox.setStandardButtons(0);

        QFont font1;
        font1.setFamily(QString::fromUtf8("8514oem"));
        font1.setPointSize(6);

        msgBox.setStyleSheet(QString::fromUtf8("background-color: rgb(239, 192, 98);"));

        QPushButton *connectButton = msgBox.addButton(tr("Ok!"), QMessageBox::ActionRole);
        connectButton->setFont(font1);

        msgBox.setFont(font1);
        msgBox.setText("Set Export Location First...");
        msgBox.exec();
    }
    else    // export Location initiated
    {

        /*
        int File_idx = 1;
        QString exportLoc = this->exportLocation + "/PixelMotionVideo" + ".mp4";
        while(true)
        {
            bool DoesExist = QFile::exists(exportLoc);
            if(DoesExist)
            {
                exportLoc = this->exportLocation + "/PixelMotionVideo" + QString::number(File_idx) + ".mp4";
                File_idx ++;
            }
            else
                break;
        }
        */

        // Start Exporting

        if(VideoLoaded)
        {
            //VideoProcessor* recorder;
            recorder = new VideoProcessor();

            connect(recorder, &QThread::finished, recorder, &QThread::deleteLater);
            connect(recorder, &VideoProcessor::sendLoadingBar, this, &TuneManager::EchoVideoBar);
            connect(recorder, &VideoProcessor::exportOver, this, &TuneManager::EchoVideoExportOver);

            recorder->initImageProcessor(this->Pixel_Size,
                                         this->Color_Amount,
                                         this->Saturation_Amount,
                                         this->Smoothing_Factor,
                                         this->Smoothing_Iteration,
                                         this->InvisiblePixelCalculation,
                                         this->QuantizerIndex,
                                         this->SigmaValue,
                                         this->SmoothingActive);

            recorder->initVideo(this->exportLocation,
                                this->totalFrame,
                                this->frame_width,
                                this->frame_height,
                                this->frame_rate,
                                this->videoCap);

            recorder->start();
        }
        else
        {
            // give error

            QMessageBox msgBox;
            msgBox.setStandardButtons(0);

            QFont font1;
            font1.setFamily(QString::fromUtf8("8514oem"));
            font1.setPointSize(6);

            msgBox.setStyleSheet(QString::fromUtf8("background-color: rgb(239, 192, 98);"));

            QPushButton *connectButton = msgBox.addButton(tr("Ok!"), QMessageBox::ActionRole);
            connectButton->setFont(font1);

            msgBox.setFont(font1);
            msgBox.setText("Error During Video Export");
            msgBox.exec();
        }
    }
}

void TuneManager::getFileLocation(QString value)
{
    this->exportLocation = value;
}

void TuneManager::EchoVideoBar(int value)
{
    qDebug()  << "Tune Manager send video bar";
    emit Video_Bar_Echo(value);
}

void TuneManager::EchoVideoExportOver()
{
    emit Video_Export_Over();
    if(recorder != nullptr)
    {
        recorder->wait();

        delete recorder;

        recorder = nullptr;
    }
}
