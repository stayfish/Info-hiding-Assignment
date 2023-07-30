#pragma once

#include<cmath>
#include<iostream>
using std::cout;

typedef unsigned char BYTE;

const int NUM = 8; // 一个块方阵的行数
const double PI = 3.1415926535;

typedef double BLOCK[NUM][NUM];

const BLOCK test_blk ={
    123, 234, 132, 321, 432, 314, 314, 341,
    432, 314, 314, 341, 123, 234, 132, 321,
    12, 24, 12, 31, 43, 324, 313, 331,
    13, 234, 12, 513, 4, 34, 54, 31,
    124, 24, 132, 342, 432, 314, 314, 413,
    213, 34, 12, 45, 64, 314, 314, 141,
    125, 334, 12, 421, 64, 314, 431, 341,
    163, 131, 13, 34, 43, 413, 126, 164,
};

// blk1 -> blk2
double c(int);
void DCT(const BLOCK&, BLOCK&);
void inverse_DCT(const BLOCK&, BLOCK&, bool flag = true); // flag 标识是否按像素值处理
void DCT_test();
#define nearest(n) ((int)(n+0.5)) 