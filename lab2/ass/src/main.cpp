#include"SS.h"
#include<iostream>
using std::cin;
using std::cout;
int main() {
#ifdef DEBUG
    cout << "this is the debug version\n";
#endif

#ifdef TEST_BMP
    BMP bmp;
    bmp.bmp_test();
#endif
#ifdef TEST_BLK
    BLOCK_test();
#endif
    string lena_path = "../data/LENA.BMP";
    string logo_path = "../data/tj-logo.bmp";
    string save_path = "../build/ass";
    string decode_path = "../build/decode";
    stringstream ss;
    BMP lena, logo;
    BMP ass, decode;
    lena.load_bmp(lena_path);
    logo.load_bmp(logo_path);
    int n;
    cout << "输入组数\n";
    cin >> n;
    double a;
    for(int i = 0; i < n; i++){
        cout << "输入嵌入强度\n";
        cin >> a;
        ss.str("");
        ss << save_path << a << ".bmp";
        ass = ASS_encode(lena, logo, a,ss.str());
        ss.str("");
        ss << decode_path << a << ".bmp";
        decode = ASS_decode(ass, logo, ss.str(), a);
        performance(a);
    }

    // DCT_test();
    return 0;
}