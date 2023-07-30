#include"rand.h"
void swap(int& a, int& b) {
    int tmp = a;
    a = b;
    b = tmp;
}

int* shuffle(int n, int key) {
    int* res = new int[n];
    int cur;
    for (int i = 0; i < n; i++) {
        res[i] = i;
    }

    std::mt19937_64 engine(key);
    std::uniform_int_distribution<int> rd(0, n - 1);
    for (int i = 0; i < n; i++) {
        decltype(rd.param()) range(i, n - 1);
        rd.param(range);
        cur = rd(engine);
        swap(res[i], res[cur]);
    }

    return res;
}

double* gauss(double mean, double stddev, int n, int key) {
    double* res = new double[n];
    std::mt19937_64 engine(key);
    std::normal_distribution<double> normal(mean, stddev);
    for(int i = 0; i < n; i++) {
        res[i] = normal(engine);
    }
    return res;
}

double* gdd(double beta, double c, int n, int key) {
    double* res = new double[n];
    bool W;
    double E;
    double tmp;
#ifdef DEBUG
    std::stringstream ss;
    static int counts = 0;
    ss << "../build/gdd" << counts << "_beta" << beta << "_c" << c << ".txt";
    std::string path = ss.str();
    std::ofstream out(path, std::ios::out);
    if(!out)
        std::cout << "fail opening file" << std::endl;
#endif
    std::mt19937_64 engine(key);
    std::bernoulli_distribution bern(0.5);
    std::gamma_distribution<double> gamma(1/c, 1/pow(beta, c));
    for(int i = 0; i < n; i++) {
        W = bern(engine);
        E = gamma(engine);
        double tmp = pow(E, 1/c);
        res[i] = (W) ? tmp : -tmp;
#ifdef DEBUG
        out << res[i] << std::endl;
#endif
    }
    return res;
}

double get_beta(double c, double stddev) {
    return 1 / stddev * pow(tgamma(3/c) / tgamma(1/c), 0.5);
}

