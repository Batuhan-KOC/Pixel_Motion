#include "videoprocessor.h"
#include <QDebug>
void VideoProcessor::run()
{
    // Do same procedure for every frame

    int File_idx = 1;
    QString exportLoc = this->folderToSave + "/PixelMotionVideo" + ".avi";
    while(true)
    {
        bool DoesExist = QFile::exists(exportLoc);
        if(DoesExist)
        {
            exportLoc = this->folderToSave + "/PixelMotionVideo" + QString::number(File_idx) + ".avi";
            File_idx ++;
        }
        else
            break;
    }

    int codec = cv::VideoWriter::fourcc('M','J','P','G');

    this->VideoWriter = new cv::VideoWriter(exportLoc.toStdString(),
                                            codec,
                                            this->frame_rate,
                                            cv::Size(this->frame_width, this->frame_height),
                                            true);

    for(long long frame_idx = 0 ; frame_idx < this->Total_Frame_Count ; frame_idx++)
    {
        // read frame to a QImage
        QImage frameQImage = QImage(this->frame_width, this->frame_height, QImage::Format_RGBA8888);
        QImage frameQImage2 = QImage(this->frame_width, this->frame_height, QImage::Format_RGBA8888);
        QImage frameQImage3 = QImage(this->frame_width, this->frame_height, QImage::Format_RGBA8888);
        QImage frameQImage4 = QImage(this->frame_width, this->frame_height, QImage::Format_RGBA8888);

        Mat* MatFrame = new Mat();
        Mat* MatFrame_2 = new Mat();
        Mat* MatFrame_3 = new Mat();
        Mat* MatFrame_4 = new Mat();

        bool FR_Valid[4] = {false,false,false,false};

        if(frame_idx < Total_Frame_Count)
        {
            this->VideoCap->set(CAP_PROP_POS_FRAMES, frame_idx);
            frame_idx++;
            this->VideoCap->read(*MatFrame);
            FR_Valid[0] = true;
            convertMatToQImage(frameQImage, MatFrame);
        }

        if(frame_idx < Total_Frame_Count)
        {
            this->VideoCap->set(CAP_PROP_POS_FRAMES, frame_idx);
            frame_idx++;
            this->VideoCap->read(*MatFrame_2);
            FR_Valid[1] = true;
            convertMatToQImage(frameQImage2, MatFrame_2);
        }

        if(frame_idx < Total_Frame_Count)
        {
            this->VideoCap->set(CAP_PROP_POS_FRAMES, frame_idx);
            frame_idx++;
            this->VideoCap->read(*MatFrame_3);
            FR_Valid[2] = true;
            convertMatToQImage(frameQImage3, MatFrame_3);
        }

        if(frame_idx < Total_Frame_Count)
        {
            this->VideoCap->set(CAP_PROP_POS_FRAMES, frame_idx);
            this->VideoCap->read(*MatFrame_4);
            FR_Valid[3] = true;
            convertMatToQImage(frameQImage4, MatFrame_4);
        }


        // create imageprocessor thread to process single frame
        ImageProcessor* workerThread,*workerThread_2,*workerThread_3,*workerThread_4;
        workerThread = new ImageProcessor();
        workerThread_2 = new ImageProcessor();
        workerThread_3 = new ImageProcessor();
        workerThread_4 = new ImageProcessor();

        if(FR_Valid[0])
        {
            workerThread->initializeProcessor(frameQImage,
                                              this->Pixel_Size,
                                              this->Color_Amount,
                                              this->Saturation_Amount,
                                              this->Smoothing_Factor,
                                              this->Smoothing_Iteration,
                                              this->InvisiblePixelCalculation,
                                              this->QuantizerIndex,
                                              this->SigmaValue,
                                              this->SmoothingActive);

            connect(workerThread, &QThread::finished,workerThread,&QThread::deleteLater);   // Auto call delete when thread is finished
            connect(workerThread,&ImageProcessor::sendProcessedImage,this,&VideoProcessor::overwriteImage);  // Get processed image when thread response

            workerThread->start();
        }
        if(FR_Valid[1])
        {
            workerThread_2->initializeProcessor(frameQImage2,
                                              this->Pixel_Size,
                                              this->Color_Amount,
                                              this->Saturation_Amount,
                                              this->Smoothing_Factor,
                                              this->Smoothing_Iteration,
                                              this->InvisiblePixelCalculation,
                                              this->QuantizerIndex,
                                              this->SigmaValue,
                                              this->SmoothingActive);

            connect(workerThread_2, &QThread::finished,workerThread_2,&QThread::deleteLater);   // Auto call delete when thread is finished
            connect(workerThread_2,&ImageProcessor::sendProcessedImage,this,&VideoProcessor::overwriteImage2);  // Get processed image when thread response

            workerThread_2->start();
        }
        if(FR_Valid[2])
        {
            workerThread_3->initializeProcessor(frameQImage3,
                                              this->Pixel_Size,
                                              this->Color_Amount,
                                              this->Saturation_Amount,
                                              this->Smoothing_Factor,
                                              this->Smoothing_Iteration,
                                              this->InvisiblePixelCalculation,
                                              this->QuantizerIndex,
                                              this->SigmaValue,
                                              this->SmoothingActive);

            connect(workerThread_3, &QThread::finished,workerThread_3,&QThread::deleteLater);   // Auto call delete when thread is finished
            connect(workerThread_3,&ImageProcessor::sendProcessedImage,this,&VideoProcessor::overwriteImage3);  // Get processed image when thread response

            workerThread_3->start();
        }
        if(FR_Valid[3])
        {
            workerThread_4->initializeProcessor(frameQImage4,
                                              this->Pixel_Size,
                                              this->Color_Amount,
                                              this->Saturation_Amount,
                                              this->Smoothing_Factor,
                                              this->Smoothing_Iteration,
                                              this->InvisiblePixelCalculation,
                                              this->QuantizerIndex,
                                              this->SigmaValue,
                                              this->SmoothingActive);

            connect(workerThread_4, &QThread::finished,workerThread_4,&QThread::deleteLater);   // Auto call delete when thread is finished
            connect(workerThread_4,&ImageProcessor::sendProcessedImage,this,&VideoProcessor::overwriteImage4);  // Get processed image when thread response

            workerThread_4->start();
        }

        //wait until thread to finish its job
        if(FR_Valid[0])
            workerThread->wait();
        if(FR_Valid[1])
            workerThread_2->wait();
        if(FR_Valid[2])
            workerThread_3->wait();
        if(FR_Valid[3])
            workerThread_4->wait();

        if(FR_Valid[0])
        {
            convertQImageToMat(processedFrame, MatFrame);
            VideoWriter->write(*MatFrame);
        }
        if(FR_Valid[1])
        {
            convertQImageToMat(processedFrame2, MatFrame_2);
            VideoWriter->write(*MatFrame_2);
        }
        if(FR_Valid[2])
        {
            convertQImageToMat(processedFrame3, MatFrame_3);
            VideoWriter->write(*MatFrame_3);
        }
        if(FR_Valid[3])
        {
            convertQImageToMat(processedFrame4, MatFrame_4);
            VideoWriter->write(*MatFrame_4);
        }


        delete workerThread;
        delete workerThread_2;
        delete workerThread_3;
        delete workerThread_4;

        delete MatFrame;
        delete MatFrame_2;
        delete MatFrame_3;
        delete MatFrame_4;

        // can be removed
        qDebug()<< "Frame : " << frame_idx << " / " << Total_Frame_Count;

        emit sendLoadingBar(int(float(float(frame_idx) / float(Total_Frame_Count))));
    }

    delete VideoWriter;

    emit exportOver();

}

void VideoProcessor::initImageProcessor(int Pixel_Size, int Color_Amount, int Saturation_Amount, int Smoothing_Factor, int Smoothing_Iteration, bool InvisiblePixelCalculation, int QuantizerIndex, float SigmaValue, bool SmoothingActive)
{
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

void VideoProcessor::initVideo(QString folderToSave,
                               long long Total_Frame_Count,
                               int frame_width,
                               int frame_height,
                               int frame_rate,
                               cv::VideoCapture *VideoCap)
{
    this->folderToSave = folderToSave;
    this->Total_Frame_Count = Total_Frame_Count;
    this->VideoCap = VideoCap;
    this->frame_width = frame_width;
    this->frame_height = frame_height;
    this->frame_rate = frame_rate;
}

void VideoProcessor::convertMatToQImage(QImage &image, Mat *mat_ptr)
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

void VideoProcessor::convertQImageToMat(QImage &image, Mat *mat_ptr)
{
    for(int x_idx = 0 ; x_idx < this->frame_width; x_idx++)
    {
        for(int y_idx = 0 ; y_idx < this->frame_height; y_idx++)
        {
            int R,G,B;
            R = (image.pixelColor(x_idx,y_idx).red());
            G = (image.pixelColor(x_idx,y_idx).green());
            B = (image.pixelColor(x_idx,y_idx).blue());
            mat_ptr->at<cv::Vec3b>(y_idx,x_idx)[0] = B; // Blue
            mat_ptr->at<cv::Vec3b>(y_idx,x_idx)[1] = G; // Green
            mat_ptr->at<cv::Vec3b>(y_idx,x_idx)[2] = R; // Red
        }
    }
}

void VideoProcessor::overwriteImage(QImage image)
{
    processedFrame = QImage(image.width(), image.height(), QImage::Format_RGBA8888);
    processedFrame = image;
}

void VideoProcessor::overwriteImage2(QImage image)
{
    processedFrame2 = QImage(image.width(), image.height(), QImage::Format_RGBA8888);
    processedFrame2 = image;
}

void VideoProcessor::overwriteImage3(QImage image)
{
    processedFrame3 = QImage(image.width(), image.height(), QImage::Format_RGBA8888);
    processedFrame3 = image;
}

void VideoProcessor::overwriteImage4(QImage image)
{
    processedFrame4 = QImage(image.width(), image.height(), QImage::Format_RGBA8888);
    processedFrame4 = image;
}
