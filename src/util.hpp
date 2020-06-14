#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <png.h>
#include <vector>

// PNG custom write/flush functions for streams
void write_data(png_structp png_ptr, png_bytep data, png_size_t length);
void flush_data(png_structp png_ptr);

// Multi-byte integer read functions
uint32_t read_uint32(std::istream &file, int position);
uint16_t read_uint16(std::istream &file, int position);
std::vector<uint8_t> read_vector(std::istream &file, int position, std::size_t length);

// Deflate zlib data
std::vector<uint8_t> decompress_zlib(std::vector<uint8_t> &buffer);

#endif