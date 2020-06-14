#include "c2p.hpp"

#include "util.hpp"

#include <fstream>

C2PFile::C2PFile(const std::string &input_path) : C2PFile{load_file(input_path)} {}

C2PFile::C2PFile(int width, int height, int size) : width(width), height(height), size(size) {}

C2PFile C2PFile::load_file(const std::string &input_path) {
    std::ifstream file(input_path, std::ifstream::in | std::ifstream::binary);
    if (!file)
        throw std::system_error(ENOENT, std::system_category());

    // Enable fstream exceptions
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // 4 bytes at offset 0x30 indicate (total size - 0x20)
        size_t total_size = read_uint32(file, 0x30) + 0x20;
        // Size of compressed data is (total size - header size - footer size)
        size_t data_size = total_size - 0xDC - 0x17C;

        // 2 bytes at 0xC2 and 0xC4 store width and hight respectively
        uint16_t width = read_uint16(file, 0xC2);
        uint16_t height = read_uint16(file, 0xC4);

        // Initialise class storing size/dimension info
        auto c2p = C2PFile(width, height, data_size);

        // ZLIB-compressed data begins at 0xDC
        std::vector<uint8_t> buffer = read_vector(file, 0xDC, c2p.size);
        c2p.rgb565 = decompress_zlib(buffer);

        return c2p;
    } catch (std::ios_base::failure &exception) {
        throw std::ios_base::failure(
            "Error reading C2P file. Please ensure you pass in a valid C2P file.");
    }
}

std::vector<png_bytep> C2PFile::convert_to_rgb888() {
    std::vector<png_bytep> row_pointers(height);
    // RGB888 requires 3 bytes per pixel
    this->rgb888.resize(height * width * 3);
    for (int i = 0; i < this->height; i++) {
        // Store a pointer to the beginning of each row (for libpng)
        row_pointers[i] = &this->rgb888[i * this->width * 3];
        for (int j = 0; j < this->width * 2; j += 2) {
            // Get two-byte rgb565 integer
            uint8_t b1 = this->rgb565[i * this->width * 2 + j];
            uint8_t b2 = this->rgb565[i * this->width * 2 + j + 1];
            uint16_t rgb565 = b1 << 8 | b2;

            // Convert & scale 2-byte RGB565 value to 3 bytes for RGB888
            // Red byte
            this->rgb888[i * 3 * this->width + j / 2 * 3] = ((rgb565 & 0xF800) >> 11) * 255 / 31;
            // Green byte
            this->rgb888[i * 3 * this->width + j / 2 * 3 + 1] = ((rgb565 & 0x07E0) >> 5) * 255 / 63;
            // Blue byte
            this->rgb888[i * 3 * this->width + j / 2 * 3 + 2] = ((rgb565 & 0x001F)) * 255 / 31;
        }
    }
    return row_pointers;
}

void C2PFile::write_png(const std::string &output_path) {
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    std::ofstream png_file(output_path, std::ios::out | std::ios::binary);
    // Enable fstream exceptions
    png_file.exceptions(std::ofstream::failbit | std::ofstream::badbit);

    // Set custom write function for std::ostream
    png_set_write_fn(png_ptr, &png_file, write_data, flush_data);

    try {
        // Set header info
        png_set_IHDR(png_ptr, info_ptr, this->width, this->height, 8, PNG_COLOR_TYPE_RGB,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        // Convert rgb565 data to rgb888
        auto row_pointers = this->convert_to_rgb888();

        // Write png to file
        png_set_rows(png_ptr, info_ptr, row_pointers.data());
        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

        // Clean up libpng structs/data
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
        png_destroy_write_struct(&png_ptr, &info_ptr);
    } catch (std::exception &exception) {
        throw std::runtime_error("Unable to write PNG file");
    }
}
