#include "octreequantizer.h"

OctTree::OctTree(int maxColorNum) : maxColors(maxColorNum), root(nullptr) {
  std::make_heap(heap.begin(), heap.end(), comparator);
  root = new OctNode;
}
vector<uint8_t> OctTree::generateColorIndex(uint8_t blue, uint8_t green, uint8_t red) {
  vector<uint8_t> ret(8, 0);
  for (int i = OCT_NUM - 1; i >= 0; i--) {
    ret[i] = (uint8_t) (((blue & 0x1) << 2) + ((green & 0x1) << 1) + (red & 0x1));
    blue >>= 1;
    green >>= 1;
    red >>= 1;
  }
  return ret;
}

void OctTree::insertColor(uint8_t blue, uint8_t green, uint8_t red) {
  auto indexes = generateColorIndex(blue, green, red);
  OctNode *cur = root;
  cur->cnt++;
  for (int i = 0; i < OCT_NUM; i++) {
    const auto idx = indexes[i];
    auto &child = cur->children[idx];
    uint8_t ib = shrinkColor(blue, i),
        ig = shrinkColor(green, i),
        ir = shrinkColor(red, i);
    if (child == nullptr) {
      child = new OctNode(
          idx, i, ib, ig, ir, cur);
      if (i == OCT_NUM - 1) {
        heap.push_back(child);
        std::push_heap(heap.begin(), heap.end(), comparator);
      }
    }
    child->cnt++;
    cur = child;
  }
}

void OctTree::generatePalette() {
  for (auto *node : heap) {
    node->mix = node->cnt;
  }
  while (heap.size() > maxColors)
  {
    std::pop_heap(heap.begin(), heap.end(), comparator);
    auto &cur = heap.back();
    heap.pop_back();

    const auto idx = cur->idx;
    auto *parent = cur->parent;
    mixColor(parent, cur);
    if (parent->mix == 0) {
      heap.push_back(parent);
      std::push_heap(heap.begin(), heap.end(), comparator);
    }
    parent->mix += cur->cnt;
    parent->children[idx] = nullptr;
  }
}

bool OctTree::comparator(const OctNode *lon, const OctNode *ron) {
  return lon->cnt > ron->cnt || (lon->cnt == ron->cnt && lon->depth < ron->depth);
}

inline uint8_t OctTree::shrinkColor(uint8_t color, int remain) {
  return color & (uint8_t) (UCHAR_MAX << (OCT_NUM - remain - 1));
}

void OctTree::mixColor(OctNode *parent, OctNode *child) {

#define __OTQ_OCT_TREE_MIX_COLOR(color) \
parent->color = static_cast<uint8_t>( \
(parent->mix * parent->color + child->cnt * child->color) \
/ (parent->mix + child->cnt) \
);
  __OTQ_OCT_TREE_MIX_COLOR(blue);
  __OTQ_OCT_TREE_MIX_COLOR(green);
  __OTQ_OCT_TREE_MIX_COLOR(red);
}
void OctTree::insertBitmap(const vector<vector<Pixel>> &bitmap) {
  for (int i = static_cast<int>(bitmap.size() - 1); i >= 0; i--) {
    for (auto pixel : bitmap[i]) {
      insertColor(pixel.blue, pixel.green, pixel.red);
    }
  }
}
const vector<OctNode *> &OctTree::getHeap() const {
  return heap;
}
OctNode *OctTree::getRoot() const {
  return root;
}

OctNode::OctNode(uint8_t idx, int depth, uint8_t blue, uint8_t green, uint8_t red, OctNode *parent) :
    idx(idx), depth(depth), blue(blue), green(green), red(red), parent(parent), cnt(0), mix(0) {
  for (auto &item : children) {
    item = nullptr;
  }
}

OctNode::OctNode() : parent(nullptr) {
  mix = cnt = idx = blue = green = red = 0;
  depth = -1;
  for (auto &item : children) {
    item = nullptr;
  }
}
std::ostream &operator<<(std::ostream &os, const OctNode &node) {
  os << "blue: " << (uint32_t) node.blue << " green: " << (uint32_t) node.green << " red: " << (uint32_t) node.red
     << " cnt: " << node.cnt << " depth: "
     << node.depth;
  return os;
}
bool OctNode::isLeaf() const {
  return cnt == mix;
}
int OctNode::removeChildren() {
  int ret = 0;
  uint64_t _blue = 0, _green = 0, _red = 0;
  for (auto &child : children) {
    if (child != nullptr) {
      ret++;
      _blue += child->blue * child->cnt;
      _green += child->green * child->cnt;
      _red += child->red * child->cnt;
      child = nullptr;
    }
  }
  blue = (uint8_t) (_blue / cnt);
  green = (uint8_t) (_green / cnt);
  red = (uint8_t) (_red / cnt);
  mix = cnt;
  return ret;
}
WidePixel::WidePixel(int32_t blue, int32_t green, int32_t red, int32_t alpha)
    : blue(blue), green(green), red(red), alpha(alpha) {}
WidePixel::WidePixel(OctNode *node) :
    blue(node->blue), green(node->green), red(node->red), alpha(0) {
}
WidePixel::WidePixel(const Pixel &pixel) :
    blue(pixel.blue), green(pixel.green), red(pixel.red), alpha(pixel.red) {}
WidePixel WidePixel::operator*(const int &val) const {
  return {blue * val, green * val, red * val, alpha * val};
}
WidePixel WidePixel::operator/(const int &val) const {
  return {blue / val, green / val, red / val, alpha / val};
}
WidePixel WidePixel::operator-(const Pixel &pixel) const {
  return {blue - pixel.blue, green - pixel.green, red - pixel.red, alpha - pixel.alpha};
}
std::ostream &operator<<(std::ostream &os, const WidePixel &pixel) {
  os << "blue: " << pixel.blue << " green: " << pixel.green << " red: " << pixel.red;
  return os;
}

inline uint32_t getPaddingBytesPerRow(uint32_t rowBytes) {
  return (ROW_ALIGNMENT - (rowBytes) % ROW_ALIGNMENT) % ROW_ALIGNMENT;
}

size_t BmpFileHeader::size() const {
  return 2 + 4 + 2 + 2 + 4;  // Cos of c++ `struct` alignment
}
BmpFileHeader::BmpFileHeader(uint32_t size) {
  bfType = 0x4D42;
  bfSize = size;
  bfReserved1 = 0;
  bfReserved2 = 0;
  bfOffBits = 54 + 256 * 4;
}

size_t BmpInfoHeader::size() const {
  return biSize;
}
BmpInfoHeader::BmpInfoHeader(uint32_t width, uint32_t height) {
  biSize = 40;
  biWidth = width;
  biHeight = height;
  biPlanes = 1;
  biBitCount = 8;
  biCompression = 0;
  biSizeImage = (getPaddingBytesPerRow(width * BITS_PER_CHANNEL) + width) * height;
  biXPixelsPerMeter = biYPixelsPerMeter = biClrUsed = biClrImportant = 0;
}

Pixel::Pixel(const OctNode *node) {
  blue = node->blue;
  green = node->green;
  red = node->red;
  alpha = 0;
}
Pixel::Pixel(uint8_t blue, uint8_t green, uint8_t red, uint8_t alpha)
    : blue(blue), green(green), red(red), alpha(alpha) {}

Pixel &Pixel::operator+=(const WidePixel &pixel) {
  blue += pixel.blue;
  green += pixel.green;
  red += pixel.red;
  alpha += alpha;
  return *this;
}

BmpImage::BmpImage(QImage image) {

    this->image = QImage(image);
    this->bitmap.clear();

    for(int y_idx = 0 ; y_idx < image.height() ; y_idx ++)
    {
      vector<Pixel> row;

      for(int x_idx = 0 ; x_idx < image.width() ; x_idx ++)
      {
        Pixel _pixel;
        QColor _pixelColor = image.pixelColor(x_idx, y_idx);

        _pixel.red = _pixelColor.red();
        _pixel.green = _pixelColor.green();
        _pixel.blue = _pixelColor.blue();
        _pixel.alpha = _pixelColor.alpha();

        row.push_back(_pixel);
      }

      this->bitmap.push_back(row);
    }
}

const vector<vector<Pixel>> &BmpImage::getBitmap() const {
  return bitmap;
}

void BmpImage::setPalette(const OctTree &tree) {
  const auto &nodeList = tree.getHeap();
  OctNode *root = tree.getRoot();

  palette.reserve(PALETTE_SPACE);
  for (int i = 0; i < nodeList.size(); i++) {
    palette.emplace_back(nodeList[i]);
    nodeList[i]->ptr = i;
  }
  palette.resize(PALETTE_SPACE, Pixel());

  for (int y = this->image.height() - 1; y >= 0; y--)
  {
    for (int x = 0; x < this->image.width()-1 ; x++)
    {
      Pixel pixel = bitmap[y][x];
      OctNode *altNode = getClosestColor(root, pixel.blue, pixel.green, pixel.red);

      QColor _pixelColor = this->image.pixelColor(x,y);
      _pixelColor.setRed(altNode->red);
      _pixelColor.setGreen(altNode->green);
      _pixelColor.setBlue(altNode->blue);

      this->image.setPixelColor(x,y,_pixelColor);
    }
  }
}

OctNode *BmpImage::getClosestColor(OctNode *root, uint8_t blue, uint8_t green, uint8_t red) const {
  vector<uint8_t> indexes = OctTree::generateColorIndex(blue, green, red);
  for (int i = 0; i < OCT_NUM; i++) {
    OctNode *child = root->children[indexes[i]];
    if (child == nullptr) {
      return root;
    } else {
      root = child;
    }
  }
  return root;
}

QImage BmpImage::getImage()
{
    return this->image;
}

OctreeQuantizer::OctreeQuantizer()
{

}

QImage OctreeQuantizer::OctreeQuantize()
{
    BmpImage BmpImage(this->image);
    OctTree tree(this->MaxColor);
    tree.insertBitmap(BmpImage.getBitmap());
    tree.generatePalette();
    BmpImage.setPalette(tree);

    return BmpImage.getImage();
}

void OctreeQuantizer::initQuantizer(QImage image, int MaxColor)
{
    this->image = QImage(image);
    this->MaxColor = MaxColor;
}
