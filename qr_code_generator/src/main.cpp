#include <iostream>
#include <string>
#include <qrencode.h>
#include <png.h>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string RESET = "\033[0m";

void save_png(const std::string& filename, const unsigned char* data, int width, int height) {
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cerr << RED << "Error opening file for writing: " << filename << RESET << std::endl;
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        std::cerr << RED << "Error creating PNG write structure" << RESET << std::endl;
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        std::cerr << RED << "Error creating PNG info structure" << RESET << std::endl;
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        std::cerr << RED << "Error during PNG creation" << RESET << std::endl;
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);

    png_set_IHDR(png, info, width, height, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    for (int y = 0; y < height; ++y) {
        png_write_row(png, data + y * width);
    }

    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

void generate_qr_code(const std::string& url, const std::string& output_file) {
    QRcode* qr = QRcode_encodeString(url.c_str(), 0, QR_ECLEVEL_L, QR_MODE_8, 1);
    if (!qr) {
        std::cerr << RED << "Error encoding QR code" << RESET << std::endl;
        return;
    }

    int scale = 10;  
    int size = qr->width * scale;
    std::vector<unsigned char> img(size * size);

    for (int y = 0; y < qr->width; ++y) {
        for (int x = 0; x < qr->width; ++x) {
            unsigned char color = (qr->data[y * qr->width + x] & 1) ? 0 : 255;
            for (int dy = 0; dy < scale; ++dy) {
                for (int dx = 0; dx < scale; ++dx) {
                    img[(y * scale + dy) * size + (x * scale + dx)] = color;
                }
            }
        }
    }

    save_png(output_file, img.data(), size, size);
    QRcode_free(qr);
}

void print_welcome_message() {
    std::cout << MAGENTA << "===================================" << RESET << std::endl;
    std::cout << CYAN << "      QR Code Generator" << RESET << std::endl;
    std::cout << MAGENTA << "===================================" << RESET << std::endl;
}

std::string ensure_png_extension(const std::string& path) {
    if (path.length() < 4 || path.compare(path.length() - 4, 4, ".png") != 0) {
        return path + ".png";
    }
    return path;
}

int main() {
    print_welcome_message();

    std::string url;
    std::string output_file;

    std::cout << YELLOW << "Enter URL: " << RESET;
    std::getline(std::cin, url);

    std::cout << YELLOW << "Enter output file path: " << RESET;
    std::getline(std::cin, output_file);

    output_file = ensure_png_extension(output_file);

    fs::path path(output_file);
    if (!fs::exists(path.parent_path())) {
        std::cerr << RED << "Directory does not exist: " << path.parent_path() << RESET << std::endl;
        return 1;
    }

    generate_qr_code(url, output_file);

    if (fs::exists(output_file)) {
        std::cout << GREEN << "QR code saved to " << output_file << RESET << std::endl;
    } else {
        std::cerr << RED << "Failed to save QR code" << RESET << std::endl;
    }

    return 0;
}
