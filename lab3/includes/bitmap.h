#pragma once

#include<cmath>
#include<cstdlib>
#include<cstring>

#include<string>
using std::string;

#include<fstream>
using std::ofstream;
using std::ifstream;

#include<iostream>
using std::ostream;
using std::istream;
using std::cout;
#include<iomanip>
using std::ios;
using std::hex;
using std::setw;
using std::setfill;

using std::move;

typedef unsigned char BYTE;

#ifdef DEBUG
#define TEST_BMP
void print_BYTE(BYTE*, int);
#endif

/**
 * 多位 BYTE 存储的时候采用大端，高位放在大地址
 * 读取的字节，低地址存在数组的低索引处
 */

struct HEADER {
    BYTE type[2] ;
    BYTE bmp_size[4];
    BYTE reserved1[2];
    BYTE reserved2[2];
    BYTE offset[4];
};

struct BITMAPINFOHEADER {
    BYTE info_header_size[4]; // 该 Header 的大小，40
    BYTE width[4];
    BYTE height[4];
    BYTE planes[2]; // 色彩平面数
    BYTE depth[2]; // 像素位数
    BYTE method[4]; // 压缩方法
    BYTE image_size[4]; // 图片大小
    BYTE h_resolution[4]; // 横向分辨率
    BYTE v_resolution[4]; // 纵向分辨率
    BYTE palette_colors[4]; // 调色板颜色数
    BYTE key_colors[4]; // 重要颜色数
};

struct RGBQUAD {
    BYTE blue;
    BYTE green;
    BYTE red;
    BYTE reserved;
};

typedef RGBQUAD* PALETTE;
typedef BYTE* BITMAP;

// depth = 8 的 BMP
class BMP {
public:
    BMP(): m_palette(nullptr), m_bitmap(nullptr) {};
    ~BMP();
    BMP(const BMP&);
    void operator=(const BMP&);
    void operator=(BMP&&);
    BMP(BMP&&);
    void set_bitmap(const BITMAP&, int n);
    BITMAP get_bitmap() const ;
    int get_bitmap_size() const ;
    int get_width() const ;
    int get_height() const ;
    int get_depth() const;
    void load_bmp(const string&);
    void write_bmp(const string&);
#ifdef TEST_BMP
    void output_BYTE();
    void bmp_test();
#endif
private:
    void write_BYTE_to(ostream&, BYTE*, int);
    // void read_BYTE_from(const istream&, BYTE*, int);
    int get_row_size() const ;
    int get_padding();
    int get_palette_colors();
private:
    int m_colors;
    int m_row_size;
    int m_bitmap_size;
    HEADER m_header;
    BITMAPINFOHEADER m_info_header;
    PALETTE m_palette;
    BITMAP m_bitmap;
};
void copy_BYTE(BYTE*, const BYTE*, int); // byte1 is a copy of byte2
int BYTE_to_int(const BYTE*, int);
void create_platte(int);