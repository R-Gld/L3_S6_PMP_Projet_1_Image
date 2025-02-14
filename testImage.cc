#include <gtest/gtest.h>

#include "Image.h"

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

/**
 *  This function is a copy of the function getMaxInContext in `Image.h`. Copied to avoid problems with tests with a different implem.
 *
 * @tparam T numeric type
 * @return the numeric limit of a non floating_point number or 1.0 otherwise.
 */
template<typename T>
constexpr T getMax() {
  return std::is_floating_point_v<T> ? static_cast<T>(1.0) : std::numeric_limits<T>::max();
}


/** ----- Conversion Check ----- **/

/**
 * @tparam PixelSrc The Pixel type (img::PixelRGB, img::PixelBGR, img::PixelRGBA, ...) of the source pixel.
 * @tparam PixelDst The Pixel Type of the destination pixel.
 * @param blueIndexSrc the index of the blue field (ex: 2 in RGB (R: 0, G: 1, B: 2) or 0 in BGR (B: 0)) in the Source Pixel Type.
 * @param blueIndexDst  the index of the blue field in the Destination Pixel Type.
 */
template<typename PixelSrc, typename PixelDst>
void checkConversion(const size_t blueIndexSrc, const size_t blueIndexDst) {
  using T_src = typename PixelSrc::DataType;
  using T_dst = typename PixelDst::DataType;

  const img::Image<PixelSrc> src(30, 30);
  const img::Image<PixelDst> dst = src;

  EXPECT_EQ(src.getWidth(), dst.getWidth());
  EXPECT_EQ(src.getHeight(), dst.getHeight());
  const int height = src.getHeight();
  const int width = src.getWidth();

  T_src raw_data_src[PixelSrc::PlaneCount];
  T_dst raw_data_dst[PixelDst::PlaneCount];

  for (int row = 0; row < height; row++) {
    for (int col = 0; col < width; col++) {
      const auto src_color = src.getColor(col, row);
      PixelSrc::toRaw(raw_data_src, src_color);

      const auto dst_color = dst.getColor(col, row);
      PixelDst::toRaw(raw_data_dst, dst_color);

      EXPECT_EQ(raw_data_src[blueIndexSrc], getMax<T_src>());
      EXPECT_EQ(raw_data_dst[blueIndexDst], getMax<T_dst>());
    }
  }
}

/**
 * Check the conversion of every combinaisons of not Gray Pixels (PixelRGB, PixelRGBA, PixelBGR, PixelBGRA).
 * @tparam T the DataType of the pixels (Ex: int, float, double, ...)
 */
template<typename T>
void checkConversionEverPixelType() {
  // Different Pixel Type
  checkConversion<img::PixelRGB<T>, img::PixelRGBA<T>>(2, 2);
  checkConversion<img::PixelRGB<T>, img::PixelBGR<T>>(2, 0);
  checkConversion<img::PixelRGB<T>, img::PixelBGRA<T>>(2, 0);
  checkConversion<img::PixelRGBA<T>, img::PixelBGR<T>>(2, 0);
  checkConversion<img::PixelRGBA<T>, img::PixelBGRA<T>>(2, 0);
  checkConversion<img::PixelBGR<T>, img::PixelBGRA<T>>(0, 0);
}

TEST(Conversion, uint8_t) { checkConversionEverPixelType<uint8_t>(); }
TEST(Conversion, float) { checkConversionEverPixelType<float>(); }
TEST(Conversion, double) { checkConversionEverPixelType<double>(); }
TEST(Conversion, size_t) { checkConversionEverPixelType<size_t>(); }
TEST(Conversion, long_double) { checkConversionEverPixelType<long double>(); }
TEST(Conversion, long_long) { checkConversionEverPixelType<long long>(); }
TEST(Conversion, int) { checkConversionEverPixelType<int>(); }


/** ----- Pixel::toRaw Check ----- **/

/**
 * Check the Pixel::toRaw function
 * @tparam Pixel the Pixel type (Ex: img::PixelRGB, img::PixelBGR, img::PixelRGBA, ...)
 */
template<typename Pixel>
void checkToRaw() {
  using DataType = typename Pixel::DataType;
  if (std::is_same_v<Pixel, img::PixelGray<DataType>>) return; // here we return because we can't test with this if it's a Gray Pixel.

  DataType max = Pixel::Max;

  img::Color<DataType> color{};
  color.red = 0;
  color.green = max/2;
  color.blue = 0;
  color.alpha = max;

  const int planeCount = Pixel::PlaneCount;
  DataType data[planeCount];
  Pixel::toRaw(data, color);
  if (planeCount == 4) {
    EXPECT_EQ(data[3], max);
  }
  EXPECT_EQ(data[0], 0);
  EXPECT_EQ(data[1], max/2);
  EXPECT_EQ(data[2], 0);
}

template<typename DataType>
void checkToRawEveryPixelType() {
  checkToRaw<img::PixelRGB<DataType>>();
  checkToRaw<img::PixelRGBA<DataType>>();
  checkToRaw<img::PixelBGR<DataType>>();
  checkToRaw<img::PixelBGRA<DataType>>();
}

TEST(checkToRaw, uint8_t) { checkToRawEveryPixelType<uint8_t>(); }
TEST(checkToRaw, float) { checkToRawEveryPixelType<float>(); }
TEST(checkToRaw, double) { checkToRawEveryPixelType<double>(); }
TEST(checkToRaw, size_t) { checkToRawEveryPixelType<size_t>(); }
TEST(checkToRaw, long_double) { checkToRawEveryPixelType<long double>(); }
TEST(checkToRaw, long_long) { checkToRawEveryPixelType<long long>(); }
TEST(checkToRaw, int) { checkToRawEveryPixelType<int>(); }


/** ----- Pixel::fromRaw Check ----- **/

/**
 * Check the Pixel::fromRaw function
 * @tparam Pixel the Pixel type (Ex: img::PixelRGB, img::PixelBGR, img::PixelRGBA, ...)
 */
template<typename Pixel>
void checkFromRaw() {
  using DataType = typename Pixel::DataType;

  DataType max = Pixel::Max;
  DataType split_max = max/2;
  const int planeCount = Pixel::PlaneCount;

  DataType data[planeCount];
  if (planeCount == 4) {
    data[3] = split_max;
  }
  data[0] = max;
  data[1] = 0;
  data[2] = max;

  img::Color<DataType> color{};
  Pixel::fromRaw(color, data);

  if (std::is_same_v<Pixel, img::PixelGray<DataType>>) {

  } else {
    if (planeCount == 4) {
      EXPECT_EQ(color.alpha, split_max);
    }
    EXPECT_EQ(color.red, max);
    EXPECT_EQ(color.green, 0);
    EXPECT_EQ(color.blue, max);
  }
}

template<typename DataType>
void checkFromRawEveryPixelType() {
  checkToRaw<img::PixelRGB<DataType>>();
  checkToRaw<img::PixelRGBA<DataType>>();
  checkToRaw<img::PixelBGR<DataType>>();
  checkToRaw<img::PixelBGRA<DataType>>();
  checkToRaw<img::PixelGray<DataType>>();
}

TEST(checkFromRaw, uint8_t) { checkFromRawEveryPixelType<uint8_t>(); }
TEST(checkFromRaw, float) { checkFromRawEveryPixelType<float>(); }
TEST(checkFromRaw, double) { checkFromRawEveryPixelType<double>(); }
TEST(checkFromRaw, size_t) { checkFromRawEveryPixelType<size_t>(); }
TEST(checkFromRaw, long_double) { checkFromRawEveryPixelType<long double>(); }
TEST(checkFromRaw, long_long) { checkFromRawEveryPixelType<long long>(); }
TEST(checkFromRaw, int) { checkFromRawEveryPixelType<int>(); }

/**
 * Séquence réalisée à la page 5 du pdf de consigne.
 */
TEST(ExempleUtilisation, inpdf) {
  // ReSharper disable CppDFAConstantConditions
  img::ImageRGB imageUInt8(256u, 256u);
  for (size_t row = 0; row < imageUInt8.getHeight(); ++row) {
    for (size_t col = 0; col < imageUInt8.getWidth(); ++col) {
      img::ImageRGB::ColorType color; // PixelRGB<uint8_t>
      color.alpha = 0xFF;
      if (row < 128 && col < 128) {
        color.red = 0xFF;
        color.green = 0x00;
        color.blue = 0x00;
      } else if (row < 128 && col >= 128) {
        color.red = 0x00;
        color.green = 0xFF;
        color.blue = 0x00;
      } else if (row >= 128 && col < 128) {
        color.red = 0xFF;
        color.green = 0xFF;
        color.blue = 0xFF;
      } else { // here (row >= 128 && col >= 128) is equivalent to else
        color.red = 0x00;
        color.green = 0x00;
        color.blue = 0x00;
      }
      imageUInt8.setColor(col, row, color);
    }
  }

  const img::Image<img::PixelRGB<float>> imageFloat(imageUInt8);
  const auto& color1 = imageFloat.getColor(0, 0);
  EXPECT_EQ(color1.red, 1.0f);
  EXPECT_EQ(color1.green, 0.0f);
  EXPECT_EQ(color1.blue, 0.0f);

  const auto& color2 = imageFloat.getColor(imageFloat.getWidth()-1, 0);
  EXPECT_EQ(color2.red, 0.0f);
  EXPECT_EQ(color2.green, 1.0f);
  EXPECT_EQ(color2.blue, 0.0f);

  const auto& color3 = imageFloat.getColor(0, imageFloat.getHeight() -1);
  EXPECT_EQ(color3.red, 1.0f);
  EXPECT_EQ(color3.green, 1.0f);
  EXPECT_EQ(color3.blue, 1.0f);

  const auto& color4 = imageFloat.getColor(imageFloat.getWidth()-1, imageFloat.getHeight()-1);
  EXPECT_EQ(color4.red, 0.0f);
  EXPECT_EQ(color4.green, 0.0f);
  EXPECT_EQ(color4.blue, 0.0f);
}

TEST(Methods, getWidth_Height) {
  const img::ImageRGB imageUInt8(256u, 256u);
  EXPECT_EQ(imageUInt8.getWidth(), 256u);
  EXPECT_EQ(imageUInt8.getHeight(), 256u);
  const img::ImageBGR imageBGR(imageUInt8);
  EXPECT_EQ(imageBGR.getWidth(), 256u);
  EXPECT_EQ(imageBGR.getHeight(), 256u);
}

TEST(Methods, GetSetColor) {
  constexpr img::Color<uint8_t> color = { 128u, 0, 128u, 255u };
  uint8_t data[4];
  img::PixelRGB<uint8_t>::toRaw(data, color);
  const img::ImageRGB imageUInt8(1u, 1u, data);
  auto [red, green, blue, alpha] = imageUInt8.getColor(0, 0);
  EXPECT_EQ(red, 128u);
  EXPECT_EQ(green, 0);
  EXPECT_EQ(blue, 128u);
  EXPECT_EQ(alpha, 255u);
}

TEST(AssignmentConversion, RGBtoRGBA) {
  constexpr std::size_t width = 2;
  constexpr std::size_t height = 2;

  img::ImageRGB imageRGB(width, height);

  img::ImageRGB::ColorType color1 { 255, 0, 0, 255 };   // rouge
  img::ImageRGB::ColorType color2 { 0, 255, 0, 255 };   // vert
  img::ImageRGB::ColorType color3 { 0, 0, 255, 255 };   // bleu
  img::ImageRGB::ColorType color4 { 255, 255, 255, 255 }; // blanc

  imageRGB.setColor(0, 0, color1);
  imageRGB.setColor(1, 0, color2);
  imageRGB.setColor(0, 1, color3);
  imageRGB.setColor(1, 1, color4);

  img::ImageRGBA imageRGBA;
  imageRGBA = imageRGB; // Use of the template operator=

  EXPECT_EQ(imageRGBA.getWidth(), width);
  EXPECT_EQ(imageRGBA.getHeight(), height);

  auto rgba1 = imageRGBA.getColor(0, 0);
  auto rgba2 = imageRGBA.getColor(1, 0);
  auto rgba3 = imageRGBA.getColor(0, 1);
  auto rgba4 = imageRGBA.getColor(1, 1);

  EXPECT_EQ(rgba1.red, color1.red);
  EXPECT_EQ(rgba1.green, color1.green);
  EXPECT_EQ(rgba1.blue, color1.blue);
  EXPECT_EQ(rgba1.alpha, color1.alpha);

  EXPECT_EQ(rgba2.red, color2.red);
  EXPECT_EQ(rgba2.green, color2.green);
  EXPECT_EQ(rgba2.blue, color2.blue);
  EXPECT_EQ(rgba2.alpha, color2.alpha);

  EXPECT_EQ(rgba3.red, color3.red);
  EXPECT_EQ(rgba3.green, color3.green);
  EXPECT_EQ(rgba3.blue, color3.blue);
  EXPECT_EQ(rgba3.alpha, color3.alpha);

  EXPECT_EQ(rgba4.red, color4.red);
  EXPECT_EQ(rgba4.green, color4.green);
  EXPECT_EQ(rgba4.blue, color4.blue);
  EXPECT_EQ(rgba4.alpha, color4.alpha);
}

TEST(AssignmentConversion, RGBAtoBGR) {
  constexpr std::size_t width = 2;
  constexpr std::size_t height = 2;
  img::ImageRGBA imageRGBA(width, height);

  img::ImageRGBA::ColorType c1 { 10, 20, 30, 40 };
  img::ImageRGBA::ColorType c2 { 50, 60, 70, 80 };
  img::ImageRGBA::ColorType c3 { 90, 100, 110, 120 };
  img::ImageRGBA::ColorType c4 { 130, 140, 150, 160 };

  imageRGBA.setColor(0, 0, c1);
  imageRGBA.setColor(1, 0, c2);
  imageRGBA.setColor(0, 1, c3);
  imageRGBA.setColor(1, 1, c4);

  img::ImageBGR imageBGR;
  imageBGR = imageRGBA;

  auto max = img::getMaxInContext<uint8_t>();

  auto bgr1 = imageBGR.getColor(0, 0);
  auto bgr2 = imageBGR.getColor(1, 0);
  auto bgr3 = imageBGR.getColor(0, 1);
  auto bgr4 = imageBGR.getColor(1, 1);

  EXPECT_EQ(bgr1.red, c1.red);
  EXPECT_EQ(bgr1.green, c1.green);
  EXPECT_EQ(bgr1.blue, c1.blue);
  EXPECT_EQ(bgr1.alpha, max);

  EXPECT_EQ(bgr2.red, c2.red);
  EXPECT_EQ(bgr2.green, c2.green);
  EXPECT_EQ(bgr2.blue, c2.blue);
  EXPECT_EQ(bgr2.alpha, max);

  EXPECT_EQ(bgr3.red, c3.red);
  EXPECT_EQ(bgr3.green, c3.green);
  EXPECT_EQ(bgr3.blue, c3.blue);
  EXPECT_EQ(bgr3.alpha, max);

  EXPECT_EQ(bgr4.red, c4.red);
  EXPECT_EQ(bgr4.green, c4.green);
  EXPECT_EQ(bgr4.blue, c4.blue);
  EXPECT_EQ(bgr4.alpha, max);
}

TEST(AssignmentConversion, FloatToUint8) {
  constexpr std::size_t width = 2;
  constexpr std::size_t height = 2;
  img::Image<img::PixelRGB<float>> imageFloat(width, height);

  img::Color<float> fcolor1 { 1.0f, 0.0f, 0.0f, 1.0f };
  img::Color<float> fcolor2 { 0.0f, 1.0f, 0.0f, 1.0f };
  img::Color<float> fcolor3 { 0.0f, 0.0f, 1.0f, 1.0f };
  img::Color<float> fcolor4 { 1.0f, 1.0f, 1.0f, 1.0f };

  imageFloat.setColor(0, 0, fcolor1);
  imageFloat.setColor(1, 0, fcolor2);
  imageFloat.setColor(0, 1, fcolor3);
  imageFloat.setColor(1, 1, fcolor4);

  img::Image<img::PixelRGB<std::uint8_t>> imageUint8;
  imageUint8 = imageFloat;

  auto c1 = imageUint8.getColor(0, 0);
  auto c2 = imageUint8.getColor(1, 0);
  auto c3 = imageUint8.getColor(0, 1);
  auto c4 = imageUint8.getColor(1, 1);

  EXPECT_EQ(c1.red, 255);
  EXPECT_EQ(c1.green, 0);
  EXPECT_EQ(c1.blue, 0);
  EXPECT_EQ(c1.alpha, 255);

  EXPECT_EQ(c2.red, 0);
  EXPECT_EQ(c2.green, 255);
  EXPECT_EQ(c2.blue, 0);
  EXPECT_EQ(c2.alpha, 255);

  EXPECT_EQ(c3.red, 0);
  EXPECT_EQ(c3.green, 0);
  EXPECT_EQ(c3.blue, 255);
  EXPECT_EQ(c3.alpha, 255);

  EXPECT_EQ(c4.red, 255);
  EXPECT_EQ(c4.green, 255);
  EXPECT_EQ(c4.blue, 255);
  EXPECT_EQ(c4.alpha, 255);
}


TEST(AssignmentConversion, RGBtoGray) {
  constexpr std::size_t width = 1;
  constexpr std::size_t height = 1;
  img::ImageRGB imageRGB(width, height);
  constexpr img::ImageRGB::ColorType rgbColor { 100, 150, 200, 255 };
  imageRGB.setColor(0, 0, rgbColor);

  const img::ImageGray imageGray = imageRGB;

  const auto [red, green, blue, alpha] = imageGray.getColor(0, 0);
  // ici, on a à peu près 140 => 0.299*100 + 0.587*150 + 0.114*200 ~= 140.75
  // la conversion tronque à 140.
  constexpr uint8_t expectedGray = 140;
  EXPECT_EQ(red, expectedGray);
  EXPECT_EQ(green, expectedGray);
  EXPECT_EQ(blue, expectedGray);
  EXPECT_EQ(alpha, 255);
}

TEST(AssignmentConversion, SelfAssignment) {
  constexpr std::size_t width = 2;
  constexpr std::size_t height = 2;
  img::ImageRGB imageRGB(width, height);

  constexpr img::ImageRGB::ColorType color { 123, 45, 67, 255 };
  //
  for (std::size_t row = 0; row < height; ++row) {
    for (std::size_t col = 0; col < width; ++col) {
      imageRGB.setColor(col, row, color);
    }
  }

  // test d'auto assignement.
  imageRGB = imageRGB;

  for (std::size_t row = 0; row < height; ++row) {
    for (std::size_t col = 0; col < width; ++col) {
      auto [red, green, blue, alpha] = imageRGB.getColor(col, row);
      EXPECT_EQ(red, color.red);
      EXPECT_EQ(green, color.green);
      EXPECT_EQ(blue, color.blue);
      EXPECT_EQ(alpha, color.alpha);
    }
  }
}

TEST(CopySemantics, CopyConstructor) {
  constexpr std::size_t width = 2, height = 2;
  img::ImageRGB original(width, height);
  constexpr img::ImageRGB::ColorType color { 200, 100, 50, 255 };
  for (std::size_t row = 0; row < height; ++row) {
    for (std::size_t col = 0; col < width; ++col) {
      original.setColor(col, row, color);
    }
  }
  const img::ImageRGB copy(original);

  constexpr img::ImageRGB::ColorType newColor { 0, 0, 0, 255 };
  original.setColor(0, 0, newColor);

  const auto [red, green, blue, alpha] = copy.getColor(0, 0);
  EXPECT_EQ(red, color.red);
  EXPECT_EQ(green, color.green);
  EXPECT_EQ(blue, color.blue);
  EXPECT_EQ(alpha, color.alpha);
}

TEST(BufferConstructor, Initialization) {
  constexpr std::size_t width = 2, height = 2;
  std::uint8_t rawData[width * height * img::PixelRGBA<std::uint8_t>::PlaneCount] = {
    10, 20, 30, 156,   // pixel (0,0)
    40, 50, 60, 128,   // pixel (1,0)
    70, 80, 90, 74,    // pixel (0,1)
    100, 110, 120, 1   // pixel (1,1)
  };
  img::Image<img::PixelRGBA<uint8_t>> image(width, height, rawData);

  auto c00 = image.getColor(0, 0);
  auto c10 = image.getColor(1, 0);
  auto c01 = image.getColor(0, 1);
  auto c11 = image.getColor(1, 1);

  EXPECT_EQ(c00.red, 10);
  EXPECT_EQ(c00.green, 20);
  EXPECT_EQ(c00.blue, 30);
  EXPECT_EQ(c00.alpha, 156);

  EXPECT_EQ(c10.red, 40);
  EXPECT_EQ(c10.green, 50);
  EXPECT_EQ(c10.blue, 60);
  EXPECT_EQ(c10.alpha, 128);

  EXPECT_EQ(c01.red, 70);
  EXPECT_EQ(c01.green, 80);
  EXPECT_EQ(c01.blue, 90);
  EXPECT_EQ(c01.alpha, 74);

  EXPECT_EQ(c11.red, 100);
  EXPECT_EQ(c11.green, 110);
  EXPECT_EQ(c11.blue, 120);
  EXPECT_EQ(c11.alpha, 1);
}
