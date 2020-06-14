#include "util.hpp"

#include <array>
#include <fstream>
#include <zlib.h>

constexpr uint16_t CHUNK{16384};

// PNG custom write/flush functions for streams
void write_data(png_structp png_ptr, png_bytep data, png_size_t length) {
    static_cast<std::ostream *>(png_get_io_ptr(png_ptr))
        ->write(reinterpret_cast<const char *>(data), length);
}

void flush_data(png_structp png_ptr) {
    static_cast<std::ostream *>(png_get_io_ptr(png_ptr))->flush();
}

// Read a 4-byte (32-bit) integer from a specified position in an istream
uint32_t read_uint32(std::istream &file, int position) {
    file.seekg(position);
    std::array<uint8_t, 4> bytes;
    file.read(reinterpret_cast<char *>(bytes.data()), 4);
    return bytes[0] << 24 | bytes[1] << 16 | bytes[2] << 8 | bytes[3];
}

// Read a 2-byte (16-bit) integer from a specified position in an istream
uint16_t read_uint16(std::istream &file, int position) {
    file.seekg(position);
    std::array<uint8_t, 2> bytes;
    file.read(reinterpret_cast<char *>(bytes.data()), 2);
    return bytes[0] << 8 | bytes[1];
}

// Read a given number of bytes from a specified stream position into a vector of bytes
std::vector<uint8_t> read_vector(std::istream &file, int position, std::size_t length) {
    std::vector<uint8_t> buffer;
    if (length > buffer.max_size()) {
        throw std::length_error("File too large or malformed");
    }
    buffer.resize(length);
    file.seekg(position);
    file.read(reinterpret_cast<char *>(buffer.data()), length);
    return buffer;
}

// Inflate zlib-compressed data
std::vector<uint8_t> decompress_zlib(std::vector<uint8_t> &buffer) {
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    try {
        int ret = inflateInit(&stream);
        if (ret != Z_OK) {
            throw std::bad_alloc();
        }

        // Read in all data at once
        stream.avail_in = buffer.size();
        stream.next_in = buffer.data();

        std::array<unsigned char, CHUNK> out; // Output buffer
        auto result = std::vector<uint8_t>();

        do {
            // Decompress chunk by chunk
            stream.avail_out = CHUNK;
            stream.next_out = out.data();

            ret = inflate(&stream, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR) {
                throw std::bad_alloc();
            }

            // Compute amount decompressed
            unsigned int have = CHUNK - stream.avail_out;
            // Append inflated data to result vector
            result.insert(result.end(), out.data(), out.data() + have);
        } while (stream.avail_out == 0); // End when we have remaining space in output buffer
        inflateEnd(&stream);

        return result;
    } catch (std::exception &exception) {
        throw std::runtime_error("Could not decompress C2P image data");
    }
}