#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <iostream>
#include <bitset>
#include "pfd.h"
#include <mpi.h>
#define LIST_NUM 2000
using namespace std;

int main(int argc, char* argv[])
{
	//---------------------------------------读取---------------------------------------
	vector<vector<unsigned>> invertedLists;//读取的链表
	vector<vector<unsigned>> decompressed;//解压后的链表应该等于invertedLists

	//计时
	long long head, tail, freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time

	getIndex(invertedLists);//读取倒排链表

	//计时终止
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
	cout << "Read(orignal) Time:" << (tail - head) * 1000.0 / freq << "ms" << endl;

	vector<unsigned> compressedRes;//压缩结果
	vector<unsigned> compressed; //读出来的压缩链表
	vector<unsigned> curList;//当前解压的链表

	//---------------------------------------压缩---------------------------------------------
	int idx = 0;
	for (int i = 0; i < (int)invertedLists.size(); i++) // 对每个链表进行压缩，存到一个vector<unsigned> compressedRes中
	    dgapCompress(invertedLists[i], compressedRes, idx);
	
	vectorToBin(compressedRes, "compress.bin");

	//---------------------------------------解压---------------------------------------------
	MPI_Init(&argc, &argv);
	int rank, proNum;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proNum);

	//计时
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time

	//从第0个bit开始读数据，解压
	binToVector("compress.bin", compressed);//读取到compressed中，应该compressed等于compressedRes

	//dgapDecompressAll(compressed, decompressed);
    dgapDeMpi(compressed, decompressed, rank, proNum);

	//计时终止
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
	cout << "Decompresss Time:" << (tail - head) * 1000.0 / freq << "ms" << endl;

	MPI_Finalize();
	return 0;
}