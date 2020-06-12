#ifndef C2P_HPP
#define C2P_HPP

#include <png.h>
#include <string>
#include <vector>

class C2PFile {
  public:
    explicit C2PFile(const std::string &input_path);
    void write_png(const std::string &output_path);

  private:
    C2PFile(int width, int height, int size);
    static C2PFile load_file(std::string &input_path);
    std::vector<png_bytep> convert_to_rgb888();
    std::vector<uint8_t> rgb888;
    std::vector<uint8_t> rgb565;
    const int width;
    const int height;
    const size_t size;
};

#endif