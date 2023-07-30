#include<random>
#include<cmath>

#ifdef DEBUG
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#endif

int* shuffle(int, int);

double* gauss(double, double, int, int);

/**
 * @param beta: scale parameter
 * @param c: shape parameter
 * @param n: length
 * @param key: seed
 * @return ** double* 
 */
double* gdd(double, double, int, int); 

/**
 * @param c: shape scale
 * @param stddev: standard deviation
 * @return scale parameter
 */
double get_beta(double, double);
