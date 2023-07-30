#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<cmath>
#include<random>

const std::string prefix = "../build/performance_";
const std::string suffix = ".txt";

#include<chrono>

#include"rand.h"


const int KEY = 100;
const int N = 1000; // 序列长度
// const int NUM = 1000; // 模拟次数 for debug
const size_t NUM = 100000;
// 阈值的数量
const int g = 6;
const double pfa0[g] = {
    1E-6, 1E-5, 1E-4, 1E-3, 0.01, 0.1
};
const double a[2] = {1.5, 1.8};
const double c[3] = {2, 1, 0.5};
const double STD = 10; 



int* generate_w(int* index, int N) {
    int * w0 = new int[N];
    int * res = new int[N];
    for (int i = 0; i < N; i++) {
        w0[i] = (i % 2 == 0) ? 1 : -1;
    }
    for (int i = 0; i < N; i++) {
        res[i] = w0[index[i]];
    }
    delete w0;
    return res;
}

void print(int* w, int N) {
    for (int i = 0; i < N; i++) {
        std::cout << w[i] << ' ';
    }
    std::cout << std::endl;
}

double ierfc(const double y)
// inverse of the error function erfc
// Copyright(C) 1996 Takuya OOURA (email: ooura@mmm.t.u-tokyo.ac.jp)
{
        const double IERFC_LIM = 27;
        double z = (y > 1)? 2-y: y;
        if (z < 1e-300) return (y > 1)? -IERFC_LIM : IERFC_LIM ;
        double w = 0.916461398268964 - log(z);
        double u = sqrt(w);
        double s = (log(u) + 0.488826640273108) / w;
        double t = 1 / (u + 0.231729200323405);
        double x = u * (1 - s * (s * 0.124610454613712 + 0.5)) -
                ((((-0.0728846765585675 * t + 0.269999308670029) * t +
                0.150689047360223) * t + 0.116065025341614) * t +
                0.499999303439796) * t;
        t = 3.97886080735226 / (x + 3.97886080735226);
        u = t - 0.5;
        s = (((((((((0.00112648096188977922 * u +
                1.05739299623423047e-4) * u - 0.00351287146129100025) * u -
                7.71708358954120939e-4) * u + 0.00685649426074558612) * u +
                0.00339721910367775861) * u - 0.011274916933250487) * u -
                0.0118598117047771104) * u + 0.0142961988697898018) * u +
                0.0346494207789099922) * u + 0.00220995927012179067;
        s = ((((((((((((s * u - 0.0743424357241784861) * u -
                0.105872177941595488) * u + 0.0147297938331485121) * u +
                0.316847638520135944) * u + 0.713657635868730364) * u +
                1.05375024970847138) * u + 1.21448730779995237) * u +
                1.16374581931560831) * u + 0.956464974744799006) * u +
                0.686265948274097816) * u + 0.434397492331430115) * u +
                0.244044510593190935) * t -
                z * exp(x * x - 0.120782237635245222);
        x += s * (x * s + 1);
        return (y > 1)? -x: x;
}

double L(double* S, int* w, int n) {
    double res = 0;
    for(int i = 0; i < n; i++) {
        res += S[i] * w[i];
    }
    res = res / n;
    return res;
}

double* embed(double* x, int* w, double a, int n, bool noisy = false, double* noise = nullptr) {
    double* res = new double[n];
    for(int i = 0; i < n; i++) {
        res[i] = x[i] + a * w[i];
        if(noisy == true) 
            res[i] += noise[i];
    }
    return res;
}

void performance(double LS, double* psi, int* fa, int* m, int g, int n, bool is_origin) {
    for(int i = 0; i < g; i++) {
        if(is_origin) {
            if(LS > psi[i]) {
                fa[i]++;
            }
        } else {
            if(LS < psi[i]) {
                m[i]++;
            }
        }
    }
}

double threshold(double pfa, double sigma, int n) {
    double res = pow(2, 0.5) * sigma * ierfc(2 * pfa);
    return res / pow(n, 0.5);
}


int main() {

    auto beforeTime = std::chrono::steady_clock::now();

    // double x[3][N];
    double* x;
    double* v;
    // double s[3][N];
    double* s;
    // double pfa[3][g];
    double* pfa = new double[g];
    // double pm[3][g];
    double* pm = new double[g];
    // double psi[g];
    double psi[g];
    // int fa[3][g];
    int* fa = new int[g]{};
    // int m[3][g];
    int* m = new int[g]{};
    int* w;
    double LS_origin;
    double LS_embeded;

    int key;
    std::mt19937_64 engine(KEY);
    std::uniform_int_distribution<int> rd(0, NUM);

    w = generate_w(shuffle(N, KEY), N);
    for (int i = 0; i < g; i++) {
        psi[i] = threshold(pfa0[i], STD, N); 
    }

    std::stringstream ss;

    // for (int i = 0; i < 2; i++) {
    //     for (int j = 0; j < 3; j++) {
    //         ss.str("");
    //         ss << prefix << "GGD_a" << a[i] << "_";
    //         // 清空数组
    //         for(int k = 0; k < g; k++) {
    //             fa[k] = 0;
    //             m[k] = 0;
    //         }
    //         // 生成对应的性能文件
    //         ss << "c" << c[j] << suffix;
    //         std::ofstream out(ss.str(), std::ios::out);
    //         if(!out)
    //             std::cout << "fail opening file, file = " << ss.str() << std::endl;
    //         for (int k = 0; k < NUM; k++) {
    //             // 生成服从 gdd 分布的序列，长度为 N
    //             key = rd(engine);
    //             x = gdd(get_beta(c[j], STD), c[j], N, key);
    //             // 嵌入水印
    //             s = embed(x, w, a[i], N);
    //             LS_origin = L(x, w, N);
    //             LS_embeded = L(s, w, N);
    //             // 计算性能
    //             performance(LS_origin, psi, fa, m, g, N, true);
    //             performance(LS_embeded, psi, fa, m, g, N, false);
    //         }
            
    //         for (int k = 0; k < g; k++) {
    //             pfa[k] = double(fa[k]) / double(NUM);
    //             pm[k] = double(m[k]) / double(NUM);
    //             out << pfa[k] << " " << pm[k] << "\n";
    //         }
    //         // 
    //         delete x;
    //         delete s;
    //     }
    // }


    for (int j = 0; j < 3; j++) {
        ss.str("");
        ss << prefix << "Gauss_a" << a[1] << "_";
        // 清空数组
        for(int k = 0; k < g; k++) {
            fa[k] = 0;
            m[k] = 0;
        }
        // 生成对应的性能文件
        ss << "c" << c[j] << suffix;
        std::ofstream out(ss.str(), std::ios::out);
        if(!out)
            std::cout << "fail opening file, file = " << ss.str() << std::endl;
        for (int k = 0; k < NUM; k++) {
            // 生成服从 gdd 分布的序列，长度为 N
            key = rd(engine);
            x = gdd(get_beta(c[j], STD), c[j], N, key);
            v = gauss(0, 10, N, key);
            // 嵌入水印
            s = embed(x, w, a[1], N, true, v);
            LS_origin = L(x, w, N);
            LS_embeded = L(s, w, N);
            // 计算性能
            performance(LS_origin, psi, fa, m, g, N, true);
            performance(LS_embeded, psi, fa, m, g, N, false);
        }
        
        for (int k = 0; k < g; k++) {
            pfa[k] = double(fa[k]) / double(NUM);
            pm[k] = double(m[k]) / double(NUM);
            out << pfa[k] << " " << pm[k] << "\n";
        }
        // 
        delete x;
        delete s;
        delete v;
    }
    

    auto afterTime = std::chrono::steady_clock::now();
    std::cout << "总耗时:" << std::endl;
	//秒
	double duration_second = std::chrono::duration<double>(afterTime - beforeTime).count();
	std::cout << duration_second << "秒" << std::endl;
}