#include <gtest/gtest.h>

#include "Image.h"

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


template<typename PixelSrc, typename PixelDst>
void checkConversionTwoPixels(const size_t indexSrc, const size_t indexDst) {
  using T_src = typename PixelSrc::DataType;
  using T_dst = typename PixelDst::DataType;
  const img::Image<PixelSrc> src(1, 500); // TODO fix lorsque width < height => crash
  const img::Image<PixelDst> dst = src;
  EXPECT_EQ(src.getWidth(), dst.getWidth());
  EXPECT_EQ(src.getHeight(), dst.getHeight());
  constexpr int planeCount_src = PixelSrc::PlaneCount;
  constexpr int planeCount_dst = PixelDst::PlaneCount;

  for (int row = 0; row < src.getHeight(); row++) {
    for (int col = 0; col < src.getWidth(); col++) {
      const auto raw_data_src = &src.data[(col + row * src.getWidth()) * planeCount_src];
      const auto raw_data_dst = &dst.data[(col + row * dst.getWidth()) * planeCount_dst];
      EXPECT_EQ(raw_data_src[indexSrc], img::getMaxInContext<T_src>());
      EXPECT_EQ(raw_data_dst[indexDst], img::getMaxInContext<T_dst>());
    }
  }
}

template<typename T>
void checkConversionType() {
  checkConversionTwoPixels<img::PixelRGB<T>, img::PixelRGBA<T>>(2, 2);
  checkConversionTwoPixels<img::PixelRGB<T>, img::PixelBGR<T>>(2, 0);
  checkConversionTwoPixels<img::PixelRGB<T>, img::PixelBGRA<T>>(2, 0);
  checkConversionTwoPixels<img::PixelRGBA<T>, img::PixelBGR<T>>(2, 0);
  checkConversionTwoPixels<img::PixelRGBA<T>, img::PixelBGRA<T>>(2, 0);
  checkConversionTwoPixels<img::PixelBGR<T>, img::PixelBGRA<T>>(0, 0);
}

TEST(Conversion, AllConversions_uint8_t) {
  checkConversionType<uint8_t>();
}

TEST(Conversion, AllConversions_float) {
  checkConversionType<float>();
}

TEST(Conversion, AllConversions_double) {
  checkConversionType<double>();
}

TEST(Conversion, AllConversions_size_t) {
  checkConversionType<size_t>();
}

TEST(Conversion, AllConversions_long_double) {
  checkConversionType<long double>();
}

TEST(Conversion, AllConversions_long_long) {
  checkConversionType<long long>();
}

TEST(Conversion, AllConversions_int) {
  checkConversionType<int>();
}

TEST(ExempleUtilisation, inpdf) {
  img::ImageRGB imageUInt8(256u, 256u);
  for (int row = 0; row < imageUInt8.getHeight(); ++row) {
    for (int col = 0; col < imageUInt8.getWidth(); ++col) {
      img::ImageRGB::ColorType color; // PixelRGB<uint8_t>
      color.alpha = 0xFF;
      if (row < 128 && col < 128) {
        color.red = 0xFF;
        color.green = 0x00;
        color.blue = 0x00;
        // ReSharper disable once CppDFAConstantConditions
      } else if (row < 128 && col >= 128) {
        color.red = 0x00;
        color.green = 0xFF;
        color.blue = 0x00;
        // ReSharper disable once CppDFAConstantConditions
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