#include"SS.h"

#ifdef TEST_BLK
void BLOCK_test() {
    ofstream out(test_path, ios::out);
    for(int i = 0; i < test_height * NUM; i++) {
        for(int j = 0; j < test_width * NUM; j++) {
            out << setw(4) << hex << (int)test_bmp[i * test_width * NUM + j] << ' ';
        }
        out << '\n';
    }
    out << '\n';
    BITMAP test_bitmap = new BYTE[test_width * NUM * test_height * NUM];
    for(int i = 0; i < test_width * NUM * test_height * NUM; i++) {
        test_bitmap[i] = test_bmp[i];
    }
    BYTE_BLOCK* byte_blks = get_byte_blocks(test_bitmap, test_width * NUM, test_height * NUM);
    for(int i = 0; i < test_height; i++) {
        for(int m = 0; m < NUM; m++) {
            for(int j = 0; j < test_width; j++) {
                for(int k = 0; k < NUM; k++)
                    out << setw(4) << hex << (int)byte_blks[i * test_width + j][m][k] << ' ';
            }
            out << '\n';
        }   
    }
    out << '\n';

    BITMAP byte_bmp = byte_blocks_to_BITMAP(byte_blks, test_width * NUM, test_height * NUM);
    for(int i = 0; i < test_height * NUM; i++) {
        for(int j = 0; j < test_width * NUM; j++) {
            out << setw(4) << hex << (int)byte_bmp[i * test_width * NUM + j] << ' ';
        }
        out << '\n';
    }
    out << '\n';
    delete[] byte_bmp;
    delete[] byte_blks;
}
#endif

BYTE_BLOCK* get_byte_blocks(const BITMAP& bitmap, int width, int height) {
    int n = width * height;
    int block_count = n / (NUM * NUM); // 块的数量
    int row_block_total_count = width / NUM; // 一行的块数
    
    BYTE_BLOCK* res = new BYTE_BLOCK[block_count];
    int row = 0, col = 0; // 第 row 行，第 col 列
    int row_blk_cnt = 0; // 当前行已经处理了的块数
    for(int i = 0; i < block_count; i++) {
        // 处理块
        for(int j = 0; j < NUM; j++) {
            for(int k = 0; k < NUM; k++) {
                res[i][j][k] = bitmap[width * (row + j) * col + k];
            }
        }
        // 处理下一块
        col += NUM;
        row_blk_cnt++;
        // 当这一行的块处理完后
        if(row_blk_cnt >= row_block_total_count) {
            row_blk_cnt = 0;
            col = 0;
            row += NUM;
        }
    }
    return res;
}


BITMAP byte_blocks_to_BITMAP(const BYTE_BLOCK* byte_blks, int width, int height) {
    int n = width * height;
    int block_count = n / (NUM * NUM); // 块的数量
    int row_block_total_count = width / NUM; // 一行的块数
    BITMAP res = new BYTE[n];

    for(int i = 0; i < block_count; i++) {
        int block_row = i / row_block_total_count; // 第 block_row 行的块
        int block_col = i % row_block_total_count; // 第 block_col 列的块
        int base = block_row * NUM * width + block_col * NUM;
        // 处理块
        for(int j = 0; j < NUM; j++) {
            for(int k = 0; k < NUM; k++) {
                res[base + j * width + k] = byte_blks[i][j][k];
            }
        }
    }

    return res;
}


// todo: logo_size 需要修改
BMP ASS_encode(const BMP& pic, const BMP& logo, double a, const string& path) {
    BMP res(move(pic));

    BITMAP origin_bitmap = res.get_bitmap();
    BITMAP logo_bitmap = logo.get_bitmap();
    int logo_size = logo.get_bitmap_size();
    // int logo_size = 1024;
    int L = logo_size * 8;
    int* b = new int[L];
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
    ofstream out(b0path, ios::out);
    for(int i = 0; i < L; i++) {
        if(i % 8 == 0 && i != 0)
            out << '\n';
        if(b[i] == 1)
            out << '1';
        else 
            out << '0';
    }

    int width = res.get_width();
    int height = res.get_height();

    int n = width * height;
    int block_count = n / (NUM * NUM); // 块的数量 M
    int K = N * L / block_count;

    BLOCK blk;
    BITMAP new_bitmap = new BYTE[n];
    int count = 0; // 当前操作的是同一 b 的第 count 块
    int l = 0; // 当前嵌入的是 b 的第 l 位
    for(int i = 0; i < width; i += 8) {
        for(int j = 0; j < height; j += 8) {
            if(count == N / K) {
                count = 0;
                l++;
            }
            // 分块
            for(int w = 0; w < NUM; w++) {
                for(int h = 0; h < NUM; h++)  {
                    blk[w][h] = origin_bitmap[width * (i + w) + j + h];
                }
            }
            embed_block(blk, a, b[l], K, count);
            count++;
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
    res.write_bmp(path);
    out.close();
    return res;
}

void embed_block(BLOCK& blk, double a, int b, int K, int base) {
    BLOCK dct_blk;
    DCT(blk, dct_blk);
    double sample;
    for(int i = base * K; i < K * (base + 1); i++) {
        dct_blk[zig_mid[i % K] / NUM][zig_mid[i % K] % NUM] += b * a * w[i];
    }
    inverse_DCT(dct_blk, blk);
}

BMP ASS_decode(const BMP& pic,const BMP& bmp, const string& path, double a) {
    BMP res(bmp);

    BITMAP origin_bitmap = pic.get_bitmap();

    BITMAP logo_bitmap = bmp.get_bitmap();
    int logo_size = bmp.get_bitmap_size();
    int L = logo_size * 8;
    int* b = new int[L]{};

    int width = pic.get_width();
    int height = pic.get_height();
    // BLOCK* blocks = get_block(origin_bitmap, width, height);
    int n = width * height;
    int block_count = n / (NUM * NUM); // 块的数量 M
    int K = N * L / block_count ;

    int count = 0; // 当前操作的是同一 b 的第 count 块
    int l = 0; // 当前嵌入的是 b 的第 l 位
    BLOCK blk;
    double s = 0;
    for(int i = 0; i < width; i += 8) {
        for(int j =0; j < height; j += 8) {
            if(count == N / K) {
                count = 0;
                l++;
                s = s / N;
                if(s > 0) b[l] = 1;
                s = 0;
            }
            for(int w = 0; w < NUM; w++) {
                for(int h = 0; h < NUM; h++)  {
                    blk[w][h] = origin_bitmap[width * (i + w) + j + h];
                }
            }
            double sum = decode_block(blk, K, count);
            s += sum;
            count++;

        }
    }

    stringstream ss;
    ss << bpath << a << ".txt";

    ofstream out(ss.str(), ios::out);
    for(int i = 0; i < L; i++) {
        if(i % 8 == 0 && i != 0)
            out << '\n';
        out << b[i];
    }

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
    res.set_bitmap(logo_bitmap, logo_size);
    delete b;

    // BITMAP new_bitmap = blocks_to_BITMAP(blocks, width, height);
    // res.set_bitmap(new_bitmap, logo_size);
    res.write_bmp(path);
    out.close();
    return res;
}

double decode_block(BLOCK& blk, int K, int base) {
    BLOCK dct_blk;
    DCT(blk, dct_blk);
    double sum(0);
    for(int i = K * base; i < K * (base + 1); i++) {
        sum += dct_blk[zig_mid[i % K] / NUM][zig_mid[i % K] % NUM] * w[i];
    }
    inverse_DCT(dct_blk, blk);
    return sum;
}

void handle(int& fa, int& m, int& n, char origin, char decode) {
    if(origin == '\n' && decode == '\n')
        return;
    else {
        n++;
        if(origin == decode)
            return;
        else if(origin == '1') {
            m++;
        } else if(origin == '0') {
            fa++;
        }
    }
}

void performance(double a) {
    stringstream ss;
    ss << bpath << a << ".txt";
    ifstream origin(b0path, ios::in);
    ifstream decode(ss.str(), ios::in);
    int n(0), fa(0), m(0);
    char origin_bit, decode_bit;
    origin_bit = origin.get();
    decode_bit = decode.get();
    handle(fa, m, n, origin_bit, decode_bit);
    while(n < 1024) {
        if(decode.eof())
            cout << "b and b0 have different length" << std::endl;
        origin_bit = origin.get();
        decode_bit = decode.get();
        handle(fa, m, n, origin_bit, decode_bit);
    }
    ss.str("");
    ss << perf_path << a << ".txt";
    ofstream perf(ss.str(), ios::out);
    double pfa = double(fa) / double(n);
    double pm = double(m) / double(n);
    perf << "p_fa: " << pfa << "\n";
    perf << "p_m: " << pm << "\n";
}

