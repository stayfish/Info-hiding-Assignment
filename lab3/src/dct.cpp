#include"dct.h"

double c(int u) {
    if(u == 0)
        return sqrt(1.0 / NUM);
    else 
        return sqrt(2.0 / NUM);
}

void DCT(const BLOCK& blk1,BLOCK& blk2) {
    
    for(int u = 0; u < NUM; u++) {
        for(int v = 0; v < NUM; v++) {
            double cu = c(u);
            double cv = c(v);
            double sum(0);
            for(int i = 0; i < NUM; i++) {
                for(int j = 0; j < NUM; j++) {
                    sum += blk1[i][j] * 
                    cos((2 * i + 1) * PI * u / (2 * NUM)) *
                    cos((2 * j + 1) * PI * v / (2 * NUM)); 
                }
            }
            blk2[u][v] = cu * cv * sum;
        }
    }
}

void inverse_DCT(const BLOCK& blk1, BLOCK& blk2, bool flag) {
    for(int i = 0; i < NUM; i++) {
        for(int j = 0; j < NUM; j++) {
            double sum(0);
            for(int u = 0; u < NUM; u++) {
                for(int v = 0; v < NUM; v++) {
                    double cu = c(u);
                    double cv = c(v);
                    sum += cu * cv * blk1[u][v] * 
                    cos((2 * i + 1) * PI * u / (2 * NUM)) *
                    cos((2 * j + 1) * PI * v / (2 * NUM)); 
                }
            }
            if(flag) {
                if(sum > 255) sum = 255;
                else if (sum < 0) sum = 0;
                sum = nearest(sum);
            }
            blk2[i][j] = sum;
        }
    }
}

void DCT_test() {
    BLOCK blk1, blk2;
    DCT(test_blk, blk2);
    inverse_DCT(blk2, blk1, false);

    cout << "blk1 is :\n";
    for(int i = 0; i < NUM; i++) {
        for(int j = 0; j < NUM; j++) {
            cout << blk1[i][j] << ' ';
        }
        cout << "\n";
    }
    cout << "orignal block is :\n";
    for(int i = 0; i < NUM; i++) {
        for(int j = 0; j < NUM; j++) {
            cout << test_blk[i][j] << ' ';
        }
        cout << "\n";
    }
}

