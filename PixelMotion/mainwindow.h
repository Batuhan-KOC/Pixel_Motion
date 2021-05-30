#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QPicture>
#include <QMessageBox>

#include "tunemanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void PixelSize_To_TuneManager(int value);
    void ColorAmount_To_TuneManager(int value);
    void Saturation_To_TuneManager(int value);
    void SmoothingFactor_To_TuneManager(int value);
    void SmoothingIteration_To_TuneManager(int value);
    void SigmaValue_To_Manager(float value);
    void InvisiblePixelCalculation_To_TuneManager(bool value);
    void QuantizerIndex_To_TuneManager(int value);
    void SmoothingActive_To_TuneManager(bool value);

    void VideoSlider_To_TuneManager(float value);

    void FileLocation_To_TuneManager(QString value);
    void UpdateRequested();
    void exportRequested();
    void videoExportRequested();
    void sendFileLocation(QString value);



private slots:
    void on_PixelSizeSlider_valueChanged(int value);

    void on_ColorAmountSlider_valueChanged(int value);

    void on_SaturationSlider_valueChanged(int value);

    void on_SmoothingFactorSlider_valueChanged(int value);

    void on_IterationSlider_valueChanged(int value);

    void Open_Image_Action();

    void Open_Video_Action();

    void Export_Action();

    void Undo_Action();

    void Redo_Action();

    void Set_Export_Location_Action();

    void About_Action();

    void How_It_Works_Action();

    void Echo_Image_On_Canvas(QImage frame);

    void on_UpdateButton_clicked();

    void on_InvisiblePixelCheckBox_toggled(bool checked);

    void on_QuantizationComboBox_currentIndexChanged(int index);

    void on_SigmaSlider_valueChanged(int value);

    void on_SmoothingActive_toggled(bool checked);

    void requestResponded(QImage image);

    void on_FileTypeBox_currentTextChanged(const QString &arg1);

    void on_QualitySlider_valueChanged(int value);

    void on_VideoSlider_valueChanged(int value);

    void VideoSlider_Update(QString value);

    void on_VideoSlider_sliderReleased();

    void videoExportBar(int value);

    void videoExportOver();

private:
    Ui::MainWindow *ui;

    void EchoImageFunction(QImage image);
    bool HideVideoContent();
    bool ShowVideoContent();

    TuneManager *_tuneManager;
    QString exportLocation = "";

    QString exportFileType = ".jpeg";
    int exportQuality = 100;

    float videoSliderValue =  0.0;

    bool UpdateInProgress = false;
};
#endif // MAINWINDOW_H
