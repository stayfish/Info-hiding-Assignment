#include<iostream>
#include<fstream>
#include<random>
#include<vector>
#include<cmath>
#include<cstdlib>

#include<time.h>

using namespace std;

//default_random_engine e;
#define SEED ((unsigned int) time(nullptr))
//#define SEED e
#define N 10000
#define PI 3.1415926
enum class Distribution {
	Normal,
	Exp,
	GGD
};

template<typename T>
struct Parameter
{
	T mu;
	T delta2;
	void show(bool show2 = 1)
	{
		if (show2 == 1)
		{
			cout << "均值为 " << mu << " " << endl;
			cout << "方差为 " << delta2 << " " << endl;
		}
		else {
			cout << "估计的参数为 " << mu << endl;
		}
	}
};

//Print the element in a vector
template<typename T>
ostream& operator<<(ostream& out, vector<T> vec)
{
	for (T elem : vec)
		out << elem << ",";
	out << endl;
	return out;
}

template<typename T>
Parameter<T> MaxLikelihoodEstimation(vector<T>X,Distribution distri)
{
	if (distri == Distribution::Normal)
	{
		T mu(0), delta2(0);
		for (auto elem : X)
			mu += elem;
		mu = mu / N;
		for (auto elem : X)
			delta2 += (elem - mu) * (elem - mu);
		delta2 = delta2 / N;
		return { mu,delta2 };
	}
	else if (distri == Distribution::Exp)
	{
		T beta(0);
		for (auto elem : X)
			beta += elem;
		beta = beta / N;
		return { beta,0 };
	}
}

template<typename T>
Parameter<T> MomentEstimation(vector<T>X,Distribution distri)
{
	if (distri == Distribution::Normal)
	{
		T mu(0), delta2(0);
		for (auto elem : X)
		{
			mu += elem;
			delta2 += elem * elem;
		}
		mu = mu / N;
		delta2 = delta2 / N - mu * mu;
		return { mu,delta2 };
	}
	
}

//Uniform(a,b)
template<typename T>
vector<T> Uniform(int l, int r)
{
	static mt19937_64 gen(SEED);
	static uniform_real_distribution<T> dis(l, r);
	vector<T> seq(N);
	for (int i=0;i<N;i++)
		seq[i] = dis(gen);
	return seq;
}

//N(mu,delta^2)
template<typename T>
vector<T> Normal(T mu, T delta2)
{
	ofstream out;

	vector<T> u1 = Uniform<T>(0, 1);
	vector<T> u2 = Uniform<T>(0, 1);
	vector<T> seq(N);
	for (int i = 0; i < N; i++)
		seq[i] = cos(2 * PI * u1[i]) * sqrt(-2 * log(u2[i])) * sqrt(delta2) + mu;
	
	// 导出数据
	out.open("u1.dat", ios::out);
	if (!out)
	{
		cout << "ERROR:fail to open file u1.dat" << endl;
		exit(0);
	}
	for (int i = 1; i < N - 1; i++)
		out << u1[i] << ",";
	out << u1[N - 1];
	out.close();

	out.open("u2.dat", ios::out);
	if (!out)
	{
		cout << "ERROR:fail to open file u2.dat" << endl;
		exit(0);
	}
	for (int i = 1; i < N - 1; i++)
		out << u2[i] << ",";
	out << u2[N - 1];
	out.close();

	out.open("Normal.dat", ios::out);
	if (!out)
	{
		cout << "ERROR:fail to open file Normal.dat" << endl;
		exit(0);
	}
	for (int i = 1; i < N - 1; i++)
		out << seq[i] << endl;
	out << seq[N - 1];
	out.close();
	
	return seq;
}

template<typename T>
void TestNormal()
{
	T mu, delta2;
	cout << "均值" << endl;
	cin >> mu;
	cout << "方差" << endl;
	cin >> delta2;
	vector<T> normalSeq = Normal(mu, delta2);
	//cout << normalSeq;

	Parameter<T> param;
	cout << "矩估计" << endl;
	param = MomentEstimation(normalSeq,Distribution::Normal);
	param.show();
	cout << "极大似然估计" << endl;
	param = MaxLikelihoodEstimation(normalSeq,Distribution::Normal);
	param.show();
}

template<typename T>
vector<T> Exp(T beta)
{
	vector<T> u = Uniform<T>(0, 1);
	vector<T> seq(N);
	for (int i = 0; i < N; i++)
		seq[i] = -beta * log(1 - u[i]);
	
	ofstream out;
	out.open("u_for_exp.dat", ios::out);
	if (!out)
	{
		cout << "ERROR:fail to open file u_for_exp.dat" << endl;
		exit(0);
	}
	for (int i = 0; i < N - 1; i++)
		out << u[i] << '\n';
	out << u[N - 1];
	out.close();

	out.open("Exp.dat", ios::out);
	if (!out)
	{
		cout << "ERROR:fail to open file Exp.dat" << endl;
		exit(0);
	}
	for (int i = 0; i < N - 1; i++)
		out << seq[i] << endl;
	out.close();

	return seq;
}

template<typename T>
void TestExp()
{
	T beta;
	cout << "参数 beta" << endl;
	cin >> beta;
	vector<T> expSeq = Exp(beta);

	Parameter<T> param;
	cout << "极大似然估计" << endl;
	param = MaxLikelihoodEstimation(expSeq, Distribution::Exp);
	param.show(0);
}

// the probability of x=1 is p
vector<int> Bernoulli(double p)
{
	vector<int> seq(N);
	vector<double> u = Uniform<double>(0, 1);
	for (int i = 0; i < N; i++)
		seq[i] = (u[i] <= p) ? 1 : 0;
	return seq;
}

template<typename T>
vector<T> GDD_C_1(T beta)
{
	vector<int> bSeq = Bernoulli(0.5);
	vector<T> expSeq = Exp(beta);
	vector<T> seq(N);
	for (int i = 0; i < N; i++)
	{
		if (bSeq[i] == 0)
			seq[i] = -expSeq[i];
		else
			seq[i] = expSeq[i];
	}

	ofstream out;

	out.open("GDD_1.dat", ios::out);
	if (!out)
	{
		cout << "ERROR:fail to open file GDD_1.dat" << endl;
		exit(0);
	}
	for (int i = 0; i < N - 1; i++)
		out << seq[i] << endl;
	out.close();

	return seq;
}

template<typename T>
vector<T> GDD_C_half(T beta)
{
	vector<int> bSeq = Bernoulli(0.5);
	vector<T> expSeq1 = Exp(sqrt(beta));
	vector<T> expSeq2 = Exp(sqrt(beta));
	vector<T> gammaSeq(N);
	for (int i = 0; i < N; i++)
		gammaSeq[i] = expSeq1[i] + expSeq2[i];
	vector<T> seq(N);
	for (int i = 0; i < N; i++)
	{
		if (bSeq[i] == 0)
			seq[i] = -gammaSeq[i] * gammaSeq[i];
		else
			seq[i] = gammaSeq[i] * gammaSeq[i];
	}

	ofstream out;

	out.open("GDD_Half.dat", ios::out);
	if (!out)
	{
		cout << "ERROR:fail to open file GDD_Half.dat" << endl;
		exit(0);
	}
	for (int i = 0; i < N - 1; i++)
		out << seq[i] << endl;
	out.close();

	return seq;
}

template<typename T>
void testGDD()
{
	T beta;
	cout << "参数 beta" << endl;
	cin >> beta;
	vector<T> gdd1Seq = GDD_C_1(beta);
	vector<T> gddHalfSeq = GDD_C_half(beta);

}

int main()
{
	//TestNormal<double>();
	//TestExp<double>();
	testGDD<double>();
}