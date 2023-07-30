#include<string>
#include<fstream>
#include<sstream>
using std::stringstream;
using std::ofstream;
#include"stdm.h"
#include"bitmap.h"


const std::string path_lena = "../data/LENA.BMP";
const std::string path_logo = "../data/tj-logo64.bmp";
const std::string path_lena_test = "../data/lena-test.bmp";
const std::string path_logo_test = "../data/logo-test.bmp";
const std::string path_encode = "../build/lena-encode";
const std::string path_decode = "../build/logo-decode";
const std::string path_rate = "../build/rate";
const std::string dat_suffix = ".dat";
const std::string bmp_suffix = ".bmp";

const int N = 10;
const int delta[N] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

string str(int dat) {
    stringstream ss;
    ss << dat;
    return ss.str();
}

int main() {
    BMP lena, logo;
    STDM stdm;
    // stdm.encode(path_lena, path_logo, "../build/lena-encode.bmp");
    // double rate = stdm.decode("../build/lena-encode.bmp", "../build/logo-decode.bmp", path_logo);
    // ofstream out("../build/rate.dat", std::ios::out);
    // out << rate;
    ofstream out(path_rate + dat_suffix, std::ios::out);
    for(int i = 0; i < N; i++) {
        string encode = path_encode + "-delta-" + str(delta[i]) + bmp_suffix;
        string decode = path_decode + "-delta-" + str(delta[i]) + bmp_suffix;
        stdm.set_delta(delta[i]);
        stdm.encode(path_lena, path_logo, encode);
        double rate = stdm.decode(encode, decode, path_logo);
        out << rate << std::endl;
    }
    
    return 0;
}