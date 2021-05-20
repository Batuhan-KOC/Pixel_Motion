#ifndef OCTREEQUANTIZER_H
#define OCTREEQUANTIZER_H

/*
 * Copyright owner of this structure : inhzus
 *
 * Github : https://github.com/inhzus
 *
 * Repo : https://github.com/inhzus/Octree-Color-Quantization
*/

#include <vector>
#include <string>
#include <iostream>
#include <queue>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <map>
#include <fstream>
#include <cassert>

#include <QImage>

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::map;

const unsigned RGB_CHANNELS = 3;
const unsigned BITS_PER_CHANNEL = 1;
const unsigned ROW_ALIGNMENT = 4;
const int OCT_NUM = 8;
const int PALETTE_SPACE = 256;

//#define CLUSTER

struct OctNode;
struct WidePixel;

struct Pixel {
  uint8_t blue;
  uint8_t green;
  uint8_t red;
  uint8_t alpha;
  int x;
  int y;
  Pixel() : blue(0), green(0), red(0), alpha(0), x(0), y(0) {};
  Pixel(uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha);
  Pixel &operator+=(const WidePixel &pixel);
  Pixel(const OctNode *node); // NOLINT(google-explicit-constructor)
};

struct WidePixel {
  int32_t blue, green, red, alpha;
  WidePixel(int32_t blue, int32_t green, int32_t red, int32_t alpha);
  WidePixel(const Pixel &pixel);
  explicit WidePixel(OctNode *node);
  WidePixel operator-(const Pixel &pixel) const;
  WidePixel operator*(const int &val) const;
  WidePixel operator/(const int &val) const;
  friend std::ostream &operator<<(std::ostream &os, const WidePixel &pixel);
};

struct OctNode {
  uint8_t blue, green, red, idx;
  uint64_t cnt, mix;
  int ptr, depth;
  OctNode *children[OCT_NUM];
  OctNode *parent;
  OctNode();
  OctNode(uint8_t idx, int depth, uint8_t blue, uint8_t green, uint8_t red, OctNode *parent);
  friend std::ostream &operator<<(std::ostream &os, const OctNode &node);
  inline bool isLeaf() const;
  inline int removeChildren();
};

class OctTree {
 private:
  OctNode *root;
  int maxColors;
  vector<OctNode *> heap;
  static bool comparator(const OctNode *lon, const OctNode *ron);
  inline uint8_t shrinkColor(uint8_t color, int remain);
  inline void mixColor(OctNode *parent, OctNode *child);
 public:
  explicit OctTree(int maxColorNum);
  OctNode *getRoot() const;
  const vector<OctNode *> &getHeap() const;
  static vector<uint8_t> generateColorIndex(uint8_t blue, uint8_t green, uint8_t red);
  void insertColor(uint8_t blue, uint8_t green, uint8_t red);
  void insertBitmap(const vector<vector<Pixel>> &bitmap);
  void generatePalette();
};

struct BmpFileHeader {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
  BmpFileHeader() = default;
  explicit BmpFileHeader(uint32_t size);
  size_t size() const;
};

struct BmpInfoHeader {
  uint32_t biSize;
  uint32_t biWidth;
  uint32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  uint32_t biXPixelsPerMeter;
  uint32_t biYPixelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
  BmpInfoHeader() = default;
  BmpInfoHeader(uint32_t width, uint32_t height);
  inline size_t size() const;
};


class BmpImage {
 private:
  vector<Pixel> palette;
  vector<uint8_t> storage;
  vector<vector<Pixel>> bitmap;
  BmpInfoHeader infoHeader;
  BmpFileHeader fileHeader;
  QImage image;
  OctNode *getClosestColor(OctNode *root, uint8_t blue, uint8_t green, uint8_t red) const;
 public:
  explicit BmpImage(QImage image);
  const vector<vector<Pixel>> &getBitmap() const;
  void setPalette(const OctTree &tree);
  QImage getImage();
};

class OctreeQuantizer
{
public:
    OctreeQuantizer();
    QImage OctreeQuantize();
    void initQuantizer(QImage image,int MaxColor);

private:
    QImage image;
    int MaxColor;
};

#endif // OCTREEQUANTIZER_H
