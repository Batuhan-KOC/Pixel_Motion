#ifndef GAUSSIANBLUR_H
#define GAUSSIANBLUR_H

/*
 * Copyright owner of this structure : Vitaly Rybnikov
 *
 * Github : https://github.com/Frodox
 *
 * Repo : https://github.com/Frodox/gaussian-blur-qt-example/blob/master/src/gaussianblur.cpp
*/

#include <QImage>
#include <qmath.h>

class GaussianBlur
{
private:
    int radius_;
    int size_;
    double diviation_;
    bool values_is_set_;
    double **matrix_;

private:
    double GetNumberOnNormalDistribution(int x, int y, const int center, double sigma) const;
    void GetPixelMatrix(const QPoint &center, const QImage &image, QRgb **matrix);
    QRgb GetNewPixelValue(QRgb** matrix);
    QPoint GetCoordinate(const QPoint &point, const QSize &image_size);
    void NormilizeElementsBySum();

public:
    explicit GaussianBlur(int radius = 1, float sigma = 1.0);
    GaussianBlur(const GaussianBlur &original);
    ~GaussianBlur();

    double GetSumElements() const;
    QImage ApplyGaussianFilterToImage(const QImage input);

    const GaussianBlur &operator = (const GaussianBlur &blur);
};

class GaussianBlurEffect
{
public:
    GaussianBlurEffect();

    void initBlur(int radius, float sigma,int iteration);
    QImage ApplyBlur(QImage input);

private:
    int radius = 1;
    double sigma = 1;
    int iteration = 1;

};

#endif // GAUSSIANBLUR_H
