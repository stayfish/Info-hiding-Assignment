#include"stdm.h"
#include<cmath>
#include<fstream>
#include<string>
using std::ifstream;
using std::ofstream;

const std::string path_of_w = "../data/w.dat";
const std::string path_of_zig_mid = "../data/zig_mid.dat";
const std::string path_of_b0 = "../build/b0.txt";
const std::string path_of_b = "../build/b.txt";
const double DEFAULT_DELTA = 4;

void STDM::get_w() {
    ifstream w_file(path_of_w, std::ios::in);
    w_file >> m_n;
    m_w.resize(m_n);
    for (int i = 0; i < m_n; i++) 
        w_file >> m_w[i];
}

void STDM::get_zig_mid() {
    ifstream zig_file(path_of_zig_mid, std::ios::in);
    zig_file >> m_k;
    m_zig_mid.resize(m_k);
    for (int i = 0; i < m_k; i++)
        zig_file >> m_zig_mid[i];
}

STDM::STDM() 
    : m_delta(DEFAULT_DELTA)
{
    get_zig_mid();
    get_w();
    m_d[0] = -0.2 * m_delta;
    m_d[1] = 0.3 * m_delta;
}

void STDM::set_delta(double d) {
    m_delta = d;
    m_d[0] = -0.2 * m_delta;
    m_d[1] = 0.3 * m_delta;
}

void STDM::encode(const string& path_of_lena, const string& path_of_logo, const string& path_of_res) {
    // 载入数据
    BMP lena, logo;
    lena.load_bmp(path_of_lena);
    logo.load_bmp(path_of_logo);
    BITMAP lena_bitmap = lena.get_bitmap();
    BITMAP logo_bitmap = logo.get_bitmap();
    // 生成编码解码的数据
    BMP res = move(lena);
    m_logo = logo;
        
    // 获取每个像素的值
    int logo_size = logo.get_bitmap_size();
    int L = logo_size * 8;
    vector<int> b = vector<int>(L);
    for(int i = 0; i < logo_size; i++) {
        BYTE cur_byte = logo_bitmap[i];
        for(int j = 0; j < 8; j++) {
            int tmp = (cur_byte >> j) & 1;
            if(tmp == 1)
                b[7 - j + i * 8] = 1;
            else
                b[7 - j + i * 8] = -1;
        }
    }
    ofstream out(path_of_b0, ios::out);
    for(int i = 0; i < L; i++) {
        if(i % 8 == 0 && i != 0)
            out << '\n';
        if(b[i] == 1)
            out << '1';
        else 
            out << '0';
    }
    out.close();

    int width = res.get_width();
    int height = res.get_height();

    int n = width * height;
    int M = n / (NUM * NUM); // 块的数量 M
    int N = M * m_k / L;

    BLOCK blk;
    BITMAP new_bitmap = new BYTE[n];
    int l = 0; // 当前嵌入的是 b 的第 l 位
    for(int i = 0; i < width; i += 8) {
        for(int j = 0; j < height; j += 8) {
            // 分块
            for(int w = 0; w < NUM; w++) {
                for(int h = 0; h < NUM; h++)  {
                    blk[w][h] = lena_bitmap[width * (i + w) + j + h];
                }
            }
            quantization(blk, b[l]);
            l++;
            // 还原块
            for(int w = 0; w < NUM; w++) {
                for(int h = 0; h < NUM; h++)  {
                    new_bitmap[width * (i + w) + j + h] = blk[w][h];
                }
            }
        }
    }
    res.set_bitmap(new_bitmap, n);
    delete new_bitmap;
    res.write_bmp(path_of_res);
}

void STDM::quantization(BLOCK& blk, int b) {
    BLOCK dct_blk;
    DCT(blk, dct_blk);
    double x = 0;
    for (int i = 0; i < m_k; i++)
        x += dct_blk[m_zig_mid[i] / NUM][m_zig_mid[i] % NUM] * m_w[i];
    x = x / m_k;
    double a = m_d[b] + 
               m_delta * floor(
                        (x - m_d[b]) / m_delta + 0.5
               ) - x;
    for(int i = 0; i < m_k; i++) {
        dct_blk[m_zig_mid[i] / NUM][m_zig_mid[i] % NUM] += m_w[i] * a;
    }
    inverse_DCT(dct_blk, blk);
}

double STDM::decode(const string& path, const string& path_of_logo, const string& path_origin) {
    BMP lena;
    lena.load_bmp(path);
    BITMAP lena_bitmap = lena.get_bitmap();

    BITMAP logo_bitmap = m_logo.get_bitmap();
    int logo_size = m_logo.get_bitmap_size();
    int L = logo_size * 8;
    vector<int> b(L);

    int width = lena.get_width();
    int height = lena.get_height();

    int l = 0; // 当前嵌入的是 b 的第 l 位
    BLOCK blk;
    for(int i = 0; i < width; i += 8) {
        for(int j =0; j < height; j += 8) {
            for(int w = 0; w < NUM; w++) {
                for(int h = 0; h < NUM; h++)  {
                    blk[w][h] = lena_bitmap[width * (i + w) + j + h];
                }
            }
            b[l] = quantization_decode(blk);
            l++;
        }
    }

    ofstream out(path_of_b, ios::out);
    for(int i = 0; i < L; i++) {
        if(i % 8 == 0 && i != 0)
            out << '\n';
        out << b[i];
    }
    out.close();

    for(int i = 0; i < logo_size; i++) {
        BYTE tmp(0x0);
        for(int j = 0; j < 8; j++) {
            int cur_b = b[7 - j + i * 8];
            if(cur_b == 1) {
                tmp = tmp | (1 << j);
            }
        }
        logo_bitmap[i] = tmp;
    }
    m_logo.set_bitmap(logo_bitmap, logo_size);
    m_logo.write_bmp(path_of_logo);

    BMP origin;
    origin.load_bmp(path_origin);
    BITMAP origin_bitmap = origin.get_bitmap();
    vector<int> b0 = vector<int>(L);
    for(int i = 0; i < logo_size; i++) {
        BYTE cur_byte = origin_bitmap[i];
        for(int j = 0; j < 8; j++) {
            int tmp = (cur_byte >> j) & 1;
            b0[7 - j + i * 8] = 0;
        }
    }
    int cnt = 0;
    for(int i = 0; i < L; i++) {
        if(b[i] != b0[i])
            cnt++;
    }
    return double(cnt)/double(L);
}

int STDM::quantization_decode(const BLOCK& blk) const {
    BLOCK dct_blk;
    DCT(blk, dct_blk);
    double y = 0;
    for (int i = 0; i < m_k; i++)
        y += dct_blk[m_zig_mid[i] / NUM][m_zig_mid[i] % NUM] * m_w[i];
    y = y / m_k;
    double err = m_d[0] + 
               m_delta * floor(
                        (y - m_d[0]) / m_delta + 0.5
               ) - y;
    return (abs(err) < m_delta / 4) ? 0 : 1;
}

