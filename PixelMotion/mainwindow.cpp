#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _tuneManager = new TuneManager(this);


    /*Connections between MainWindow and Tune Manager */
    connect(this,&MainWindow::PixelSize_To_TuneManager,_tuneManager,&TuneManager::PixelSizeUpdate);
    connect(this,&MainWindow::ColorAmount_To_TuneManager,_tuneManager,&TuneManager::ColorAmountUpdate);
    connect(this,&MainWindow::Saturation_To_TuneManager,_tuneManager,&TuneManager::SaturationUpdate);
    connect(this,&MainWindow::SmoothingFactor_To_TuneManager,_tuneManager,&TuneManager::SmoothingFactorUpdate);
    connect(this,&MainWindow::SmoothingIteration_To_TuneManager,_tuneManager,&TuneManager::SmoothingIterationUpdate);
    connect(this,&MainWindow::FileLocation_To_TuneManager,_tuneManager,&TuneManager::FileLocationUpdate);
    connect(this,&MainWindow::UpdateRequested,_tuneManager,&TuneManager::updateRequested);
    connect(this,&MainWindow::InvisiblePixelCalculation_To_TuneManager,_tuneManager,&TuneManager::InvisiblePixelCalculationUpdate);
    connect(this,&MainWindow::SigmaValue_To_Manager,_tuneManager,&TuneManager::SigmaValueUpdate);
    connect(_tuneManager,&TuneManager::DeliverImage,this,&MainWindow::Echo_Image_On_Canvas);
    connect(this,&MainWindow::QuantizerIndex_To_TuneManager,_tuneManager,&TuneManager::QuantizerIndexUpdate);
    connect(this,&MainWindow::SmoothingActive_To_TuneManager,_tuneManager,&TuneManager::SmoothingActiveUpdate);
    connect(this,&MainWindow::exportRequested,_tuneManager,&TuneManager::exportRequested);
    connect(_tuneManager,&TuneManager::ReturnTopLevelImage,this,&MainWindow::requestResponded);

    /*Connections between Action Tabs and corresponding Slots */
    connect(this->ui->OpenImageAction,&QAction::triggered,this,&MainWindow::Open_Image_Action);
    connect(this->ui->OpenVideoAction,&QAction::triggered,this,&MainWindow::Open_Video_Action);
    connect(this->ui->actionExport,&QAction::triggered,this,&MainWindow::Export_Action);
    connect(this->ui->UndoAction,&QAction::triggered,this,&MainWindow::Undo_Action);
    connect(this->ui->RedoActiopn,&QAction::triggered,this,&MainWindow::Redo_Action);
    connect(this->ui->SetLocationFolderAction,&QAction::triggered,this,&MainWindow::Set_Export_Location_Action);
    connect(this->ui->AboutAction,&QAction::triggered,this,&MainWindow::About_Action);
    connect(this->ui->HowItWorksAction,&QAction::triggered,this,&MainWindow::How_It_Works_Action);

    QImage bannerImage = QImage(":/images/bannerTransparent.png","PNG");
    bannerImage = bannerImage.convertToFormat(QImage::Format_RGBA8888);
    EchoImageFunction(bannerImage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_PixelSizeSlider_valueChanged(int value)
{
    this->ui->PixelSizeValue->setText(QString::number(value));
    emit PixelSize_To_TuneManager(value);
    this->update();
}

void MainWindow::on_ColorAmountSlider_valueChanged(int value)
{
    this->ui->ColorAmountValue->setText(QString::number(value));
    emit ColorAmount_To_TuneManager(value);
    this->update();
}

void MainWindow::on_SaturationSlider_valueChanged(int value)
{
    this->ui->SaturationValue->setText(QString("%") + QString::number(value));

    if(value < 0)
        this->ui->SaturationValue->setText(QString("- %") + QString::number(std::abs(value)));
    else
        this->ui->SaturationValue->setText(QString("+ %") + QString::number(value));

    emit Saturation_To_TuneManager(value);
    this->update();
}

void MainWindow::on_SmoothingFactorSlider_valueChanged(int value)
{
    this->ui->SmoothingFactorValue->setText(QString::number(value));
    emit SmoothingFactor_To_TuneManager(value);
    this->update();
}

void MainWindow::on_IterationSlider_valueChanged(int value)
{
    this->ui->IterationValue->setText(QString::number(value));
    emit SmoothingIteration_To_TuneManager(value);
    this->update();
}

void MainWindow::Open_Image_Action()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"),
                                                     "/home",
                                                     tr("Images (*.png *.jpeg *.jpg)"));
    HideVideoContent();

    if(!fileName.isEmpty())
        emit FileLocation_To_TuneManager(fileName);
}

void MainWindow::Open_Video_Action()
{
    ShowVideoContent();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Video File"),
                                                     "/home",
                                                     tr("Videos (*.mp4 *.mov *.wmv *.avi *.mkv)"));

    if(!fileName.isEmpty())
        emit FileLocation_To_TuneManager(fileName);
}

void MainWindow::Export_Action()
{
    emit exportRequested();
}

void MainWindow::Undo_Action()
{

}

void MainWindow::Redo_Action()
{

}

void MainWindow::Set_Export_Location_Action()
{
    QString dir = "";
    dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if(dir != "")
    {
        this->exportLocation = dir;
        QMessageBox msgBox;
        msgBox.setStandardButtons(0);

        QFont font1;
        font1.setFamily(QString::fromUtf8("8514oem"));
        font1.setPointSize(6);

        msgBox.setStyleSheet(QString::fromUtf8("background-color: rgb(217, 217, 108);"));

        QPushButton *connectButton = msgBox.addButton(tr("Yeap!"), QMessageBox::ActionRole);
        connectButton->setFont(font1);

        msgBox.setFont(font1);
        msgBox.setText("Export Location succesfully initiated!");
        msgBox.exec();
    }
}

void MainWindow::About_Action()
{

}

void MainWindow::How_It_Works_Action()
{

}

void MainWindow::on_UpdateButton_clicked()
{
    if(!this->UpdateInProgress){
        this->ui->UpdateButton->setText("IN PROCESS");
        this->UpdateInProgress = true;
        emit UpdateRequested();
    }
}

void MainWindow::Echo_Image_On_Canvas(QImage frame)
{
    this->ui->UpdateButton->setText("UPDATE");
    this->UpdateInProgress = false;
    EchoImageFunction(frame);
}



void MainWindow::on_InvisiblePixelCheckBox_toggled(bool checked)
{
    emit InvisiblePixelCalculation_To_TuneManager(checked);
}

void MainWindow::on_QuantizationComboBox_currentIndexChanged(int index)
{
    emit QuantizerIndex_To_TuneManager(index);
}

void MainWindow::on_SigmaSlider_valueChanged(int value)
{
    this->ui->SigmaValue->setText(QString::number(float(value/10.0)));
    emit SigmaValue_To_Manager(value);
    this->update();
}

void MainWindow::on_SmoothingActive_toggled(bool checked)
{
    emit SmoothingActive_To_TuneManager(checked);
    this->update();
}

void MainWindow::requestResponded(QImage image)
{
    int File_idx = 1;
    QString exportLoc = this->exportLocation + "/PixelMotionImage" + this->exportFileType;
    while(true)
    {
        bool DoesExist = QFile::exists(exportLoc);
        if(DoesExist)
        {
            exportLoc = this->exportLocation + "/PixelMotionImage" + QString::number(File_idx) + this->exportFileType;
            File_idx ++;
        }
        else
            break;
    }
    bool exported = false;

    if(exportFileType == ".jpeg")
        exported = image.save(exportLoc,"JPEG",this->exportQuality);
    else if(exportFileType == ".png")
        exported = image.save(exportLoc,"PNG",this->exportQuality);
    else if(exportFileType == ".bmp")
        exported = image.save(exportLoc,"BMP",this->exportQuality);

    if(exported)
    {
        QMessageBox msgBox;
        msgBox.setStandardButtons(0);

        QFont font1;
        font1.setFamily(QString::fromUtf8("8514oem"));
        font1.setPointSize(6);

        msgBox.setStyleSheet(QString::fromUtf8("background-color: rgb(217, 217, 108);"));

        QPushButton *connectButton = msgBox.addButton(tr("OK!"), QMessageBox::ActionRole);
        connectButton->setFont(font1);

        msgBox.setFont(font1);
        msgBox.setText("Succesfully exported!");
        msgBox.exec();
    }
}

void MainWindow::on_FileTypeBox_currentTextChanged(const QString &arg1)
{
    this->exportFileType=QString(".") + QString(arg1.toLower());
}

void MainWindow::on_QualitySlider_valueChanged(int value)
{
    this->ui->QualityValue->setText(QString::number(value));
    this->exportQuality = value;
}

void MainWindow::EchoImageFunction(QImage image)
{
    QPixmap _pixmap;

    QString tempFile = "temp.png";

    image.save(tempFile,"PNG",100);

    _pixmap = QPixmap(tempFile,"PNG");

    QDir dir;
    dir.remove(tempFile);

    this->ui->ImageLabel->setPixmap(_pixmap.scaled(this->ui->ImageLabel->width(),
                                                   this->ui->ImageLabel->height(),
                                                   Qt::KeepAspectRatio));

    this->update();
}

bool MainWindow::HideVideoContent()
{
    this->ui->VideoSlider->setVisible(false);
}

bool MainWindow::ShowVideoContent()
{
    this->ui->VideoTimeLabel->setVisible(true);
}

void MainWindow::on_VideoSlider_valueChanged(int value)
{
    this->videoSliderValue = (float(value)/100.0);

    emit VideoSlider_To_TuneManager(this->videoSliderValue);
}
