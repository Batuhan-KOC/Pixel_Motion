#include "mediancutquantizer.h"
MedianCutQuantizer::MedianCutQuantizer(unsigned short QuantizerTypeSelect)
{
    if(QuantizerTypeSelect == MEDIAN)
        this->IsMedianCutQuantizer = true;
    else
        this->IsMedianCutQuantizer = false;
}

bool R_Comp(const PIXEL_RGB& a, const PIXEL_RGB& b)
{
    return (a.RGB[0] < b.RGB[0]);
}
bool G_Comp(const PIXEL_RGB& a, const PIXEL_RGB& b)
{
    return (a.RGB[1] < b.RGB[1]);
}
bool B_Comp(const PIXEL_RGB& a, const PIXEL_RGB& b)
{
    return (a.RGB[2] < b.RGB[2]);
}

double findMedian(const std::vector<PIXEL_RGB>& a, int n, int Channel_Index)
{
    if(n>0)
    {
        if (n % 2 != 0)
            return (double)a[n / 2].RGB[Channel_Index];

        return (double)(a[(n - 1) / 2].RGB[Channel_Index] + a[n / 2].RGB[Channel_Index]) / 2.0;
    }
    else
        return 0;
}

QImage MedianCutQuantizer::MedianCutQuantize(QImage image, int ColorNumber)
{

    // Collect all the pixels in "pixels" vector

    for(int x_idx = 0 ; x_idx < image.width() ; x_idx ++)
    {
        for(int y_idx = 0 ; y_idx < image.height(); y_idx++)
        {
            QColor pixelColor = image.pixelColor(x_idx, y_idx);

            if(pixelColor != Qt::black)
            {
                PIXEL_RGB Pixel_RGB;
                Pixel_RGB.RGB[0] = pixelColor.red();
                Pixel_RGB.RGB[1] = pixelColor.green();
                Pixel_RGB.RGB[2] = pixelColor.blue();

                Pixel_RGB.x = x_idx;
                Pixel_RGB.y = y_idx;

                this->pixels.push_back(Pixel_RGB);
            }
        }
    }

    // Find largest color channel in pixels
    int R_MAX = std::max_element(this->pixels.begin(),this->pixels.end(),R_Comp)->RGB[0];
    int G_MAX = std::max_element(this->pixels.begin(),this->pixels.end(),G_Comp)->RGB[1];
    int B_MAX = std::max_element(this->pixels.begin(),this->pixels.end(),B_Comp)->RGB[2];

    int R_MIN = std::min_element(this->pixels.begin(),this->pixels.end(),R_Comp)->RGB[0];
    int G_MIN = std::min_element(this->pixels.begin(),this->pixels.end(),G_Comp)->RGB[1];
    int B_MIN = std::min_element(this->pixels.begin(),this->pixels.end(),B_Comp)->RGB[2];

    int R_RANGE = std::abs(R_MAX - R_MIN);
    int G_RANGE = std::abs(G_MAX - G_MIN);
    int B_RANGE = std::abs(B_MAX - B_MIN);

    int Channel_Index = -1;
    int Middle_Channel_Index = -1;
    int Last_Channel_Index = -1;

    if(R_RANGE >= G_RANGE && R_RANGE >= B_RANGE)
    {
        Channel_Index = 0;
        if (G_RANGE >= B_RANGE)
        {
            Middle_Channel_Index = 1;
            Last_Channel_Index = 2;
        }
        else
        {
            Middle_Channel_Index = 2;
            Last_Channel_Index = 1;
        }
    }
    else if(G_RANGE >= R_RANGE && G_RANGE >= B_RANGE)
    {
        Channel_Index = 1;
        if (R_RANGE >= B_RANGE)
        {
            Middle_Channel_Index = 0;
            Last_Channel_Index = 2;
        }
        else
        {
            Middle_Channel_Index = 2;
            Last_Channel_Index = 0;
        }
    }
    else if(B_RANGE >= G_RANGE && B_RANGE >= R_RANGE)
    {
        Channel_Index = 2;
        if (G_RANGE >= R_RANGE)
        {
            Middle_Channel_Index = 1;
            Last_Channel_Index = 0;
        }
        else
        {
            Middle_Channel_Index = 0;
            Last_Channel_Index = 1;
        }
    }


    if (Channel_Index == -1)
        return image;

    // Sort pixels by largest channel
    if(Channel_Index == 0)
        std::sort(this->pixels.begin(),this->pixels.end(),R_Comp);
    else if(Channel_Index == 1)
        std::sort(this->pixels.begin(),this->pixels.end(),G_Comp);
    else if(Channel_Index == 2)
        std::sort(this->pixels.begin(),this->pixels.end(),B_Comp);

    // Sort by middle channel index

    std::vector<PIXEL_RGB>::iterator begin_iter = this->pixels.begin();
    int Channel_Value = this->pixels.at(0).RGB[Channel_Index];

    for(std::vector<PIXEL_RGB>::iterator iter = this->pixels.begin()+1;
        iter != pixels.end();
        iter++)
    {
        int pixel_channel_value = (*iter).RGB[Channel_Index];

        if(Channel_Value != pixel_channel_value || ((iter == this->pixels.end() -1 && iter - begin_iter != 0)))
        {
            Channel_Value = pixel_channel_value;

            if(Middle_Channel_Index == 0)
                std::sort(begin_iter, iter, R_Comp);
            else if(Middle_Channel_Index == 1)
                std::sort(begin_iter, iter, G_Comp);
            else if(Middle_Channel_Index == 2)
                std::sort(begin_iter, iter, B_Comp);

            begin_iter = iter;
        }
    }

    // Sort by last channel index

    begin_iter = this->pixels.begin();
    int Channel_Value_Middle = this->pixels.at(0).RGB[Middle_Channel_Index];
    Channel_Value = this->pixels.at(0).RGB[Channel_Index];

    for(std::vector<PIXEL_RGB>::iterator iter = this->pixels.begin()+1;
        iter != pixels.end();
        iter++)
    {
        int pixel_channel_value_higher = (*iter).RGB[Channel_Index];
        int pixel_channel_value_middle = (*iter).RGB[Middle_Channel_Index];

        if((Channel_Value_Middle != pixel_channel_value_middle)  ||
           (Channel_Value != pixel_channel_value_higher)  ||
           ((iter == this->pixels.end() -1 && iter - begin_iter != 0)))
        {
            Channel_Value = pixel_channel_value_higher;
            Channel_Value_Middle = pixel_channel_value_middle;

            if(Last_Channel_Index == 0)
                std::sort(begin_iter, iter, R_Comp);
            else if(Last_Channel_Index == 1)
                std::sort(begin_iter, iter, G_Comp);
            else if(Last_Channel_Index == 2)
                std::sort(begin_iter, iter, B_Comp);

            begin_iter = iter;
        }
    }
    color_boxes.push_back(this->pixels);

    // Generate color palette


    while(true)
    {
        if(color_boxes.size() >= size_t(ColorNumber))
            break;

        int PaletteCount_BeforeProcess = this->color_boxes.size();

        std::vector<std::vector<PIXEL_RGB>> ColorPalettes;

        for(std::vector<PIXEL_RGB> ColorPalette : this->color_boxes)
        {
            // Divide each color palettes into 2 sub color palettes according their median values
            if(ColorPalette.size() > 0)
            {

                std::vector<PIXEL_RGB> Lower_Palette;
                std::vector<PIXEL_RGB> Upper_Palette;

                if(IsMedianCutQuantizer)
                {
                    double median = findMedian(ColorPalette,ColorPalette.size(),Channel_Index);

                    for(PIXEL_RGB Pixel : ColorPalette)
                    {
                        if(double(Pixel.RGB[Channel_Index]) >= median)
                            Upper_Palette.push_back(Pixel);
                        else
                            Lower_Palette.push_back(Pixel);
                    }
                }
                else
                {
                    for(size_t i = 0 ; i< ColorPalette.size() ; i++)
                    {
                        if(i < ColorPalette.size() / 2)
                            Lower_Palette.push_back(ColorPalette.at(i));
                        else
                            Upper_Palette.push_back(ColorPalette.at(i));
                    }
                }

                ColorPalettes.push_back(Lower_Palette);
                ColorPalettes.push_back(Upper_Palette);
            }
        }

        this->color_boxes.clear();

        for(std::vector<PIXEL_RGB> Palette : ColorPalettes)
            this->color_boxes.push_back(Palette);

        int PaletteCount_AfterProcess = this->color_boxes.size();

        // If can not divide palettes, end the process
        if(PaletteCount_BeforeProcess == PaletteCount_AfterProcess)
            break;

        ColorPalettes.clear();
    }

    // Find average color for each palette

    for(std::vector<PIXEL_RGB> palette : this->color_boxes)
    {
        if(palette.size() > 0)
        {
            int R = 0, G = 0, B = 0;

            for(PIXEL_RGB pixel : palette)
            {
                R += pixel.RGB[0];
                G += pixel.RGB[1];
                B += pixel.RGB[2];
            }

            int palette_size = int(palette.size());

            R = R / palette_size;
            G = G / palette_size;
            B = B / palette_size;

            // Apply average color to the image

            for(PIXEL_RGB pixel : palette)
            {
                QColor pixelColor = image.pixelColor(pixel.x, pixel.y);
                pixelColor.setRed(R);
                pixelColor.setGreen(G);
                pixelColor.setBlue(B);

                image.setPixelColor(pixel.x, pixel.y, pixelColor);
            }

        }
        else
            continue;
    }
    return image;
}

