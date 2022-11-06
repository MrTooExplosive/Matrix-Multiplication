#include <iostream>
#include <cmath>
#include <string>
#include <fstream>
#include <utility>
#include <chrono>
#include <random>
using namespace std;

int** classical(int**, int**, int);

int** divAndCon(int**, int**, int);
int** divAndConWorker(int**, int**, int, int, int, int, int);

int** strassen(int**, int**, int);
int** strassenWorker(int**, int**, int, int, int, int, int);

string matrixToStr(int**, int);
int** readMatrix(string);
int** add(int**, int**,int);
int** sub(int**, int**,int);
int** addPortion(int**, int**, int, int, int, int, int);
int** subPortion(int**, int**, int, int, int, int, int);
pair<int**,int> makePerfect(int**, int);
int** trim(int**, int, int);
int** createRandom(int);

int main()
{
	int** A = readMatrix("a.txt");
	int** B = readMatrix("b.txt");
	int** C = classical(A, B, 4);
	cout << "A:\n" << matrixToStr(A, 4) << "\n\n\n\n\nB:\n" << matrixToStr(B, 4) << "\n\n\n\nClassical Answer:\n" << matrixToStr(C, 4);
	for (int j = 0; j < 4; j++)
		delete[] C[j];
	delete[] C;

	C = divAndCon(A, B, 4);
	cout << "\n\n\n\nNaive Divide and Conquer Answer:\n" << matrixToStr(C, 4);
	for (int j = 0; j < 4; j++)
		delete[] C[j];
	delete[] C;

	C = strassen(A, B, 4);
	cout << "\n\n\n\nStrassen Answer:\n" << matrixToStr(C, 4);
	for (int j = 0; j < 4; j++)
		delete[] C[j];
	delete[] C;

	for (int i = 0; i < 4; i++)
	{
		delete[] A[i];
		delete[] B[i];
	}
	delete[] A;
	delete[] B;

	cout << "\n\n\n\n";
	ofstream fout;
	fout.open("times.csv", ios_base::app);
	unsigned long long n = 2048;
	chrono::time_point<chrono::high_resolution_clock> start, end;
	chrono::duration<double, micro> timeTaken;
	double classSum, divSum, strassSum;
	while (true)
	{
		classSum = 0;
		divSum = 0;
		strassSum = 0;
		fout << n;
		for (int i = 0; i < 10; i++)
		{
			A = createRandom(n);
			B = createRandom(n);

			start = chrono::high_resolution_clock::now();
			C = classical(A, B, n);
			end = chrono::high_resolution_clock::now();
			timeTaken = end - start;
			classSum += timeTaken.count();
			for (int j = 0; j < n; j++)
				delete[] C[j];
			delete[] C;

			start = chrono::high_resolution_clock::now();
			C = divAndCon(A, B, n);
			end = chrono::high_resolution_clock::now();
			timeTaken = end - start;
			divSum += timeTaken.count();
			for (int j = 0; j < n; j++)
				delete[] C[j];
			delete[] C;

			start = chrono::high_resolution_clock::now();
			C = strassen(A, B, n);
			end = chrono::high_resolution_clock::now();
			timeTaken = end - start;
			strassSum += timeTaken.count();

			for (int i = 0; i < n; i++)
			{
				delete[] A[i];
				delete[] B[i];
				delete[] C[i];
			}
			delete[] A;
			delete[] B;
			delete[] C;
		}
		fout << ',' << classSum / 10 << ',' << divSum / 10 << ',' << strassSum / 10 << '\n';
		fout.flush();
		cout << "Finished n = " << n << '\n';
		n *= 2;
	}

	return 0;
}

int** subPortion(int** a, int** b, int aRowStart, int aColStart, int bRowStart, int bColStart, int size)
{
	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int[size];
		for (int j = 0; j < size; j++)
			ans[i][j] = a[aRowStart + i][aColStart + j] - b[bRowStart + i][bColStart + j];
	}
	return ans;
}

pair<int**,int> makePerfect(int** a, int initSize)
{
	int size = pow(2, ceil(log2(initSize)));
	int** result = new int* [size];
	for (int i = 0; i < size; i++)
	{
		result[i] = new int[size];
		for (int j = 0; j < size; j++)
			result[i][j] = j < initSize && i < initSize ? a[i][j] : 0;
	}

	return make_pair(result,size);
}

int** trim(int** res, int newSize, int oldSize)
{
	int** ans = new int* [newSize];
	for (int i = 0; i < newSize; i++)
	{
		ans[i] = new int[newSize];
		for (int j = 0; j < newSize; j++)
			ans[i][j] = res[i][j];
		if (i < oldSize)
			delete[] res[i];

	}
	delete[] res;
	return ans;
}

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> distrib(-200, 200);

int** createRandom(int size)
{
	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int[size];
		for (int j = 0; j < size; j++)
			ans[i][j] = distrib(gen);
	}
	return ans;
}

int** addPortion(int** a, int** b, int aRowStart, int aColStart, int bRowStart, int bColStart, int size)
{
	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int[size];
		for (int j = 0; j < size; j++)
			ans[i][j] = a[aRowStart + i][aColStart + j] + b[bRowStart + i][bColStart + j];
	}
	return ans;
}

int** strassen(int** a, int** b, int size)
{
	if (log2(size) == round(log2(size)))
		return strassenWorker(a, b, 0, 0, 0, 0, size);
	pair<int**, int> aMat = makePerfect(a, size);
	pair<int**, int> bMat = makePerfect(b, size);
	int** ans = strassenWorker(aMat.first, bMat.first, 0, 0, 0, 0, aMat.second);
	for (int i = 0; i < aMat.second; i++)
	{
		delete[] aMat.first[i];
		delete[] bMat.first[i];
	}
	delete[] aMat.first;
	delete[] bMat.first;

	ans = trim(ans, size, aMat.second);
	return ans;
}
int** strassenWorker(int** a, int** b, int ArowStart, int AcolStart, int BrowStart, int BcolStart, int size)
{
	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int[size];
		for (int j = 0; j < size; j++)
			ans[i][j] = 0;
	}

	if (size == 2)
	{
		ans[0][0] = a[ArowStart][AcolStart] * b[BrowStart][BcolStart] + a[ArowStart][AcolStart + 1] * b[BrowStart + 1][BcolStart];
		ans[0][1] = a[ArowStart][AcolStart] * b[BrowStart][BcolStart + 1] + a[ArowStart][AcolStart + 1] * b[BrowStart + 1][BcolStart + 1];
		ans[1][0] = a[ArowStart + 1][AcolStart] * b[BrowStart][BcolStart] + a[ArowStart + 1][AcolStart + 1] * b[BrowStart + 1][BcolStart];
		ans[1][1] = a[ArowStart + 1][AcolStart] * b[BrowStart][BcolStart + 1] + a[ArowStart + 1][AcolStart + 1] * b[BrowStart + 1][BrowStart + 1];
		return ans;
	}

	int** sub1 = subPortion(b, b, 0, size / 2, size / 2, size / 2, size / 2);
	int** p1 = strassenWorker(a, sub1, 0, 0, 0, 0, size / 2);

	int** add2 = addPortion(a, a, 0, 0, 0, size / 2, size / 2);
	int** p2 = strassenWorker(add2 , b, 0, 0, size / 2, size / 2, size / 2);

	int** add3 = addPortion(a, a, size / 2, 0, size / 2, size / 2, size / 2);
	int** p3 = strassenWorker(add3, b, 0, 0, 0, 0, size / 2);

	int** sub4 = subPortion(b, b, size / 2, 0, 0, 0, size / 2);
	int** p4 = strassenWorker(a, sub4, size / 2, size / 2, 0, 0, size / 2);

	int** add5a = addPortion(a, a, 0, 0, size / 2, size / 2, size / 2);
	int** add5b = addPortion(b, b, 0, 0, size / 2, size / 2, size / 2);
	int** p5 = strassenWorker(add5a, add5b,0,0,0,0,size/2);

	int** sub6 = subPortion(a, a, 0, size / 2, size / 2, size / 2, size / 2);
	int** add6 = addPortion(b, b, size / 2, 0, size / 2, size / 2, size / 2);
	int** p6 = strassenWorker(sub6, add6, 0, 0, 0, 0, size / 2);

	int** sub7 = subPortion(a, a, 0, 0, size / 2, 0, size / 2);
	int** add7 = addPortion(b, b, 0, 0, 0, size / 2, size / 2);
	int** p7 = strassenWorker(sub7, add7, 0, 0, 0, 0, size / 2);


	int** c11 = add(p4, p5, size / 2);
	c11 = add(c11, p6, size / 2);
	c11 = sub(c11, p2, size / 2);

	int** c12 = add(p1, p2, size / 2);
	int** c21 = add(p3, p4, size / 2);

	int** c22 = add(p1, p5, size / 2);
	c22 = sub(c22, p3, size / 2);
	c22 = sub(c22, p7, size / 2);

	for (int i = 0; i < size / 2; i++)
	{
		for (int j = 0; j < size / 2; j++)
		{
			ans[i][j] = c11[i][j];
			ans[i][j + size / 2] = c12[i][j];
			ans[i + size / 2][j] = c21[i][j];
			ans[i + size / 2][j + size / 2] = c22[i][j];
		}
	}

	for (int i = 0; i < size / 2; i++)
	{
		delete[] p1[i];
		delete[] p2[i];
		delete[] p3[i];
		delete[] p4[i];
		delete[] p5[i];
		delete[] p6[i];
		delete[] p7[i];
		delete[] c11[i];
		delete[] c12[i];
		delete[] c21[i];
		delete[] c22[i];
		delete[] sub1[i];
		delete[] add2[i];
		delete[] add3[i];
		delete[] sub4[i];
		delete[] add5a[i];
		delete[] add5b[i];
		delete[] sub6[i];
		delete[] add6[i];
		delete[] sub7[i];
		delete[] add7[i];
	}
	delete[] p1;
	delete[] p2;
	delete[] p3;
	delete[] p4;
	delete[] p5;
	delete[] p6;
	delete[] p7;
	delete[] c11;
	delete[] c12;
	delete[] c21;
	delete[] c22;
	delete[] sub1;
	delete[] add2;
	delete[] add3;
	delete[] sub4;
	delete[] add5a;
	delete[] add5b;
	delete[] sub6;
	delete[] add6;
	delete[] sub7;
	delete[] add7;

	return ans;
}

int** divAndCon(int** a, int** b, int size)
{
	if (log2(size) == round(log2(size)))
		return divAndConWorker(a, b,0,0,0,0,size);
	pair<int**,int> aMat = makePerfect(a, size);
	pair<int**, int> bMat = makePerfect(b, size);
	int** ans = divAndConWorker(aMat.first, bMat.first, 0, 0, 0, 0, aMat.second);
	ans = trim(ans, size, aMat.second);
	for (int i = 0; i < aMat.second; i++)
	{
		delete[] aMat.first[i];
		delete[] bMat.first[i];
	}
	delete[] aMat.first;
	delete[] bMat.first;
	return ans;
}
int** divAndConWorker(int** a, int** b, int ArowStart, int AcolStart, int BrowStart, int BcolStart, int size)
{

	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int[size];
		for (int j = 0; j < size; j++)
		{
			ans[i][j] = 0;
		}
	}

	if (size == 2)
	{
		ans[0][0] = a[ArowStart][AcolStart] * b[BrowStart][BcolStart] + a[ArowStart][AcolStart + 1] * b[BrowStart+1][BcolStart];
		ans[0][1] = a[ArowStart][AcolStart] * b[BrowStart][BcolStart+1] + a[ArowStart][AcolStart+1] * b[BrowStart+1][BcolStart+1];
		ans[1][0] = a[ArowStart+1][AcolStart] * b[BrowStart][BcolStart] + a[ArowStart+1][AcolStart+1] * b[BrowStart+1][BcolStart];
		ans[1][1] = a[ArowStart+1][AcolStart] * b[BrowStart][BcolStart+1] + a[ArowStart+1][AcolStart+1] * b[BrowStart+1][BrowStart+1];
		return ans;
	}

	int** c11a = divAndConWorker(a, b, 0, 0, 0, 0, size / 2);
	int** c11b = divAndConWorker(a, b, 0, size / 2, size / 2, 0, size / 2);
	int** c11 = add(c11a, c11b, size / 2);

	int** c12a = divAndConWorker(a, b, 0, 0, 0, size / 2, size / 2);
	int** c12b = divAndConWorker(a, b, 0, size / 2, size / 2, size / 2, size / 2);
	int** c12 = add(c12a, c12b, size / 2);

	int** c21a = divAndConWorker(a, b, size / 2, 0, 0, 0, size / 2);
	int** c21b = divAndConWorker(a, b, size / 2, size / 2, size / 2, 0, size / 2);
	int** c21 = add(c21a, c21b, size / 2);

	int** c22a = divAndConWorker(a, b, size / 2, 0, 0, size / 2, size / 2);
	int** c22b = divAndConWorker(a, b, size / 2, size / 2, size / 2, size / 2, size / 2);
	int** c22 = add(c22a, c22b, size / 2);

	for (int i = 0; i < size/2; i++)
	{
		for (int j = 0; j < size/2; j++)
		{
			ans[i][j] = c11[i][j];
			ans[i][j + size / 2] = c12[i][j];
			ans[i + size / 2][j] = c21[i][j];
			ans[i + size / 2][j + size / 2] = c22[i][j];
		}
	}

	for (int i = 0; i < size / 2; i++)
	{
		delete[] c11[i];
		delete[] c12[i];
		delete[] c21[i];
		delete[] c22[i];
		delete[] c11a[i];
		delete[] c11b[i];
		delete[] c12a[i];
		delete[] c12b[i];
		delete[] c21a[i];
		delete[] c21b[i];
		delete[] c22a[i];
		delete[] c22b[i];
	}

	delete[] c11;
	delete[] c12;
	delete[] c21;
	delete[] c22;
	delete[] c11a;
	delete[] c11b;
	delete[] c12a;
	delete[] c12b;
	delete[] c21a;
	delete[] c21b;
	delete[] c22a;
	delete[] c22b;

	return ans;
}

int** sub(int** a, int** b, int size)
{
	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int[size];
		for (int j = 0; j < size; j++)
			ans[i][j] = a[i][j] - b[i][j];
	}
	return ans;
}

int** add(int** a, int** b, int size)
{
	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int[size];
		for (int j = 0; j < size; j++)
			ans[i][j] = a[i][j] + b[i][j];
	}
	return ans;
}

int** classical(int** a, int** b, int size)
{
	int** ans = new int* [size];
	for (int i = 0; i < size; i++)
	{
		ans[i] = new int [size];
		for (int j = 0; j < size; j++)
		{
			ans[i][j] = 0;
			for (int c = 0; c < size; c++)
				ans[i][j] += a[i][c] * b[c][j];
		}
	}

	return ans;
}

string matrixToStr(int** a, int size)
{
	string ans = "";
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
			ans += to_string(a[i][j]) + '\t';
		ans += '\n';
	}
	return ans;
}

int** readMatrix(string filename)
{
	int** ans;

	ifstream fin(filename);
	short size = 0;
	fin >> size;

	ans = new int* [size];
	for (int i = 0; i < size; i++)
		ans[i] = new int[size];

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
			fin >> ans[i][j];
	}

	return ans;
}
