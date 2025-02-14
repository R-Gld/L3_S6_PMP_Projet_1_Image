#ifndef IMG_IMAGE_H
#define IMG_IMAGE_H

#include <cstddef>
#include <limits>
#include <type_traits>

namespace img {

  template<typename T>
  constexpr T getMaxInContext() {
    return std::is_floating_point_v<T> ? static_cast<T>(1.0) : std::numeric_limits<T>::max();
  }

  template<typename TargetT, typename SourceT>
  constexpr TargetT convert(SourceT& src) {
    if constexpr (std::is_same_v<TargetT, SourceT>) return src;
    TargetT targetMaxValue = getMaxInContext<TargetT>();
    SourceT sourceMaxValue = getMaxInContext<SourceT>();
    return static_cast<TargetT>(src * targetMaxValue / sourceMaxValue);
  }

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
    static constexpr int PlaneCount = 3;
    static constexpr auto Max = getMaxInContext<T>();

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
      color.red = data[0];
      color.green = data[1];
      color.blue = data[2];
      color.alpha = Max;
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
      data[0] = color.red;
      data[1] = color.green;
      data[2] = color.blue;
    }
  };

  template<typename T>
  struct PixelBGR {
    static constexpr int PlaneCount = 3;
    static constexpr auto Max = getMaxInContext<T>();

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
      color.blue = data[0];
      color.green = data[1];
      color.red = data[2];
      color.alpha = Max;
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
      data[0] = color.blue;
      data[1] = color.green;
      data[2] = color.red;
    }
  };

  template<typename T>
  struct PixelRGBA {
    static constexpr int PlaneCount = 4;
    static constexpr auto Max = getMaxInContext<T>();

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
      color.red = data[0];
      color.green = data[1];
      color.blue = data[2];
      color.alpha = data[3];
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
      data[0] = color.red;
      data[1] = color.green;
      data[2] = color.blue;
      data[3] = color.alpha;
    }
  };

  template<typename T>
  struct PixelBGRA {
    static constexpr int PlaneCount = 4;
    static constexpr auto Max = getMaxInContext<T>();

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
      color.blue = data[0];
      color.green = data[1];
      color.red = data[2];
      color.alpha = data[3];
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
      data[0] = color.blue;
      data[1] = color.green;
      data[2] = color.red;
      data[3] = color.alpha;
    }
  };

  template<typename T>
  struct PixelGray {
    static constexpr int PlaneCount = 1;
    static constexpr auto Max = getMaxInContext<T>();

    using DataType = T;

    // Cast a raw color value to a Color object
    static constexpr void fromRaw(Color<T>& color, const T* data) {
      color.red = data[0];
      color.green = data[0];
      color.blue = data[0];
      color.alpha = Max;
    }

    // Cast a Color object to a raw value
    static constexpr void toRaw(T* data, const Color<T>& color) {
        data[0] = 0.299 * color.red + 0.587 * color.green + 0.114 * color.blue;
    }
  };

  template<typename Pixel>
  class Image {
  public:
    using DataType = typename Pixel::DataType;
    using ColorType = Color<DataType>;
    using PixelType = Pixel;

    std::size_t width{0}, height{0};
    DataType* data;


    /**
     * Return the index calculated from the column and row.
     * @param col column
     * @param row row
     * @return std::size_t the index corresponding.
     */
    [[nodiscard]] std::size_t index(const std::size_t col, const std::size_t row) const {
      return index(col, row, PixelType::PlaneCount);
    }

    /**
     * Return the index calculated from the column and row.
     * @param col column
     * @param row row
     * @param planeCount the number of field characterizing a color.
     * @return a `size_t`, the index corresponding.
     */
    [[nodiscard]] std::size_t index(const std::size_t col, const std::size_t row, const int planeCount) const {
      return (col + row * width) * planeCount;
    }

    // Destructor
    ~Image() {
      delete[] data;
    }

    /**
     * Empty Image with width and height equal 0.
     */
    Image(): data(nullptr) {}

    /**
     * Construct a blue image.
     * @param width the width of the image
     * @param height the height of the image
     */
    Image(std::size_t width, std::size_t height) : width(width), height(height) {
      const size_t total_size = width * height * PixelType::PlaneCount;
      data = new DataType[total_size];

      Color<DataType> color {0, 0, getMaxInContext<DataType>(), getMaxInContext<DataType>()};
      for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
          setColor(i, j, color);
        }
      }
    }

    /**
     * Construct an image from a buffer.
     * @param width the width of the image
     * @param height the height of the image
     * @param data the buffer containing the data. (Should not be verified here.)
     */
    Image(std::size_t width, std::size_t height, const DataType* data) : width(width), height(height) {
      const size_t total_size = width * height * PixelType::PlaneCount;
      data = new DataType[total_size];

      for (size_t row = 0; row < height; ++row) {
        for (size_t col = 0; col < width; ++col) {
          setColor(col, row, data[index(col, row)]);
        }
      }
    }

    // Conversions
    template<typename OtherPixel>
    Image(const Image<OtherPixel>& other) : width(other.getWidth()), height(other.getHeight())
    {
      const std::size_t total_size = width * height * PixelType::PlaneCount;
      data = new DataType[total_size];

      for (std::size_t row = 0; row < height; ++row) {
        for (std::size_t col = 0; col < width; ++col) {
          Color<typename OtherPixel::DataType> srcColor = other.getColor(col, row);

          Color<DataType> dstColor {
            convert<DataType>(srcColor.red),
            convert<DataType>(srcColor.green),
            convert<DataType>(srcColor.blue),
            convert<DataType>(srcColor.alpha)
          };

          setColor(col, row, dstColor);
        }
      }
    }


    template<typename OtherPixel>
    Image& operator=(const Image<OtherPixel>& other) {
      if (this == &other) { return *this; }

      width = other.getWidth();
      height = other.getHeight();

      const std::size_t total_size = width * height * PixelType::PlaneCount;
      auto* newData = new DataType[total_size];
      for (int i = 0; i < total_size; ++i) {
        newData[i] = other.data[i];
      }
      delete[] data;

      data = newData;

      return *this;
    }

    Image(Image&& other) noexcept : width(other.width), height(other.height), data(other.data) {
      other.data = nullptr;
      other.width = 0;
      other.height = 0;
    }

    Image& operator=(Image&& other) noexcept {
      if (this == &other) { return *this; }

      delete[] data;

      width = other.width;
      height = other.height;
      data = other.data;

      other.data = nullptr;
      other.width = 0;
      other.height = 0;

      return *this;
    }

    // Get image width in pixel
    [[nodiscard]] std::size_t getWidth() const
    { return this->width; }

    // Get image height in pixel
    [[nodiscard]] std::size_t getHeight() const
    { return this->height; }

    // Get the pointer to the raw data
    const DataType* getData() const
    { return data; }

    // Get the color of a pixel
    Color<DataType> getColor(std::size_t col, std::size_t row) const {
      Color<DataType> color;
      std::size_t idx = index(col, row);
      Pixel::fromRaw(color, data + idx);

      return color;
    }

    // Set the color of a pixel
    void setColor(std::size_t col, std::size_t row, Color<DataType> color) {
      std::size_t idx = index(col, row);
      Pixel::toRaw(data + idx, color);
    }
  };

  // Some pretty aliases
  using ImageRGB = Image<PixelRGB<std::uint8_t>>;
  using ImageBGR = Image<PixelBGR<std::uint8_t>>;
  using ImageRGBA = Image<PixelRGBA<std::uint8_t>>;
  using ImageBGRA = Image<PixelBGRA<std::uint8_t>>;
  using ImageGray = Image<PixelGray<std::uint8_t>>;
}

#endif // IMG_IMAGE_H
