#ifndef IMG_IMAGE_H
#define IMG_IMAGE_H

#include <cstddef>

namespace img {

  // Struct to store a color
  template<typename T>
  struct Color {
    T red;
    T green;
    T blue;
    T alpha;
  };

  template<typename T>
  struct PixelRGB {
    static constexpr int PlaneCount = /* implementation defined */;
    static constexpr auto Max = /* implementation defined */;

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
    }
  };

  template<typename T>
  struct PixelBGR {
    static constexpr int PlaneCount = /* implementation defined */;
    static constexpr auto Max = /* implementation defined */;

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
    }
  };

  template<typename T>
  struct PixelRGBA {
    static constexpr int PlaneCount = /* implementation defined */;
    static constexpr auto Max = /* implementation defined */;

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
    }
  };

  template<typename T>
  struct PixelBGRA {
    static constexpr int PlaneCount = /* implementation defined */;
    static constexpr auto Max = /* implementation defined */;

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
    }
  };

  template<typename T>
  struct PixelGray {
    static constexpr int PlaneCount = /* implementation defined */;
    static constexpr auto Max = /* implementation defined */;

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
    }
  };

  template<typename Pixel>
  class Image {
  public:
    using DataType = /* implementation defined */;
    using ColorType = /* implementation defined */;

    // No Image
    Image()
    {}

    // Blue image
    Image(std::size_t width, std::size_t height)
    {}

    // From buffer
    Image(std::size_t width, std::size_t height, const DataType* data)
    {}

    // Conversions
    template<typename OtherPixel>
    Image(const Image<OtherPixel>& other)
    {}

    template<typename OtherPixel>
    Image& operator=(const Image<OtherPixel>& other)
    {}

    // Get image width in pixel
    std::size_t getWidth() const
    {}

    // Get image height in pixel
    std::size_t getHeight() const
    {}

    // Get the pointer to the raw data
    const DataType* getData() const
    {}

    // Get the color of a pixel
    Color<DataType> getColor(std::size_t col, std::size_t row) const
    {}

    // Set the color of a pixel
    void setColor(std::size_t col, std::size_t row, Color<DataType> color)
    {}
  };

  // Some pretty aliases
  using ImageRGB = Image<PixelRGB<std::uint8_t>>;
  using ImageBGR = Image<PixelBGR<std::uint8_t>>;
  using ImageRGBA = Image<PixelRGBA<std::uint8_t>>;
  using ImageBGRA = Image<PixelBGRA<std::uint8_t>>;
  using ImageGray = Image<PixelGray<std::uint8_t>>;
}

#endif // IMG_IMAGE_H
