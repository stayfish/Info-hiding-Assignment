#pragma once

#include<vector>
#include<string>
using std::vector;
using std::string;

#include"dct.h"
#include"bitmap.h"

class STDM {
public:
    STDM();
    void set_delta(double);
    void encode(const string&, const string&, const string&);
    double decode(const string&, const string&, const string&);
private:
    double m_d[2];
    double m_delta;
    vector<int> m_w;
    vector<int> m_zig_mid;
    int m_k;
    int m_n;
    BMP m_logo;
private:
    void get_w();
    void get_zig_mid();
    void quantization(BLOCK&, int);
    int quantization_decode(const BLOCK&) const;
};