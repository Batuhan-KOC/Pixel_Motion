#ifndef MEDIANCUTQUANTIZER_H
#define MEDIANCUTQUANTIZER_H
#include <QImage>
#include <vector>

struct PIXEL_RGB{
    int RGB[3] = {0,0,0};
    int x = 0;
    int y = 0;
};

using namespace std;

class MedianCutQuantizer
{
public:

    enum QuantizerTypeSelection{
        MEDIAN = 1,
        CENTER
    };

    explicit MedianCutQuantizer(unsigned short QuantizerTypeSelect);

    QImage MedianCutQuantize(QImage image, int ColorNumber);

private:
    std::vector<PIXEL_RGB> pixels;
    std::vector<std::vector<PIXEL_RGB>> color_boxes;
    bool IsMedianCutQuantizer = true;
};

#endif // MEDIANCUTQUANTIZER_H
