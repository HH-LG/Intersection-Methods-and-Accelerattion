#pragma once
#include<iostream>
#include<bitset>
#include<vector>
#include<string>
#include<omp.h>
#include<fstream>
#include <bitset>
#include <algorithm>
#include <sstream>
#include <math.h>
#include <emmintrin.h>
#include "bitOp.h"
#define LIST_NUM 2000
#define NUM_THREADS 4
using namespace std;

void dgapTransform(const vector<unsigned>& invertedIndex, vector<unsigned>& result)
{
	unsigned indexLen = invertedIndex.size();
	if (indexLen == 0)
		return;

	result.push_back(invertedIndex[0]);

	// d-gap前一项减去后一项,同时求最大间隔
	for (unsigned i = 1; i < indexLen; i++)
	{
		unsigned delta = invertedIndex[i] - invertedIndex[i - 1];
		result.push_back(delta);
	}
}

void dgapCompress(const vector<unsigned>& invertedIndex, vector<unsigned>& result, int& idx)
{
	unsigned indexLen = invertedIndex.size();
	if (indexLen == 0)
		return;

	vector<unsigned> deltaId;
	deltaId.push_back(invertedIndex[0]);
	unsigned maxDelta = invertedIndex[0];//最大间隔

	// d-gap前一项减去后一项,同时求最大间隔
	for (unsigned i = 1; i < indexLen; i++)
	{
		unsigned delta = invertedIndex[i] - invertedIndex[i - 1];
		deltaId.push_back(delta);
		if (delta > maxDelta)
			maxDelta = delta;
	}

	unsigned maxBitNum = ceil(log2(maxDelta + 1));//整体最多使用maxBitNum位存储

	long long bitCnt = idx + 32 + 6 + maxBitNum * indexLen;//总共使用bit数 = 以使用的bit数+存该链表要使用的bit数
	result.resize((int)ceil(bitCnt / 32.0));

	writeBitData(result, idx, indexLen, 32);//写入元素个数
	writeBitData(result, idx + 32, maxBitNum, 6);//写入delta元素长度

	idx += 38;//从index+38位开始写压缩后的delta
	for (unsigned i = 0; i < indexLen; i++)
	{
		writeBitData(result, idx, deltaId[i], maxBitNum);
		idx += maxBitNum;
	}
}

// invertedIndex: dgap压缩好的倒排索引
// result：解缩后的索引
// idx：从第idx个bit开始解压
// return：解压后大小（bit)
//int dgapDecompress(const vector<unsigned>& compressedLists, vector<unsigned>& result,int& idx)
//{
//	//前32位是长度
//	unsigned len = readBitData(compressedLists, idx, 32);
//	// cout << len << endl;
//	if (len == 0)
//		return -1;
//
//	//6位是用的Bit数
//	int bitNum = (int)readBitData(compressedLists, idx+32, 6);
//	idx += 38;
//
//	unsigned delta = readBitData(compressedLists, idx, bitNum);
//	idx += bitNum;
//	result.push_back(delta);//第一个delta直接进去
//	for (unsigned i = 1; i < len; i++)
//	{
//		delta = readBitData(compressedLists,idx,bitNum);
//		idx += bitNum;
//		result.push_back(result[i - 1] + delta);//后续的都要加上前一个放进去
//	}
//	return idx;
//}


 //invertedIndex: dgap压缩好的倒排索引
 //result：解缩后的索引
 //idx：从第idx个bit开始解压
 //return：解压后大小（bit)
vector<unsigned> dgapDecompress(const vector<unsigned>& compressedLists, int& idx)
{
	vector<unsigned> result;
	//前32位是长度
	unsigned len = readBitData(compressedLists, idx, 32);
	result.reserve(len);// 不同与后面，resize
	//printf("%d\n", len);
	if (len == 0)
		return result;

	//6位是用的Bit数
	int bitNum = (int)readBitData(compressedLists, idx + 32, 6);
	idx += 38;

	unsigned delta = readBitData(compressedLists, idx, bitNum);
	idx += bitNum;
	result.push_back(delta);//第一个delta直接进去
	for (unsigned i = 1; i < len; i++)
	{
		delta = readBitData(compressedLists, idx, bitNum);
		idx += bitNum;
		result.push_back(result[i - 1] + delta);//后续的都要加上前一个放进去
	}
	return result;
}

//void dgapDecompressAll(const vector<unsigned>& compressedLists, vector<vector<unsigned>>& result)
//{
//	vector<unsigned> curList;
//	int idx = 0;
//	for (int i = 0; i < LIST_NUM; i++) //解压2000个链表,结果放在decompressed
//	{
//		curList = dgapDecompress(compressedLists, idx);
//		result.push_back(curList);
//		curList.clear();
//	}
//}

void dgapDecompressAll(const vector<unsigned>& compressedLists, vector<vector<unsigned>>& result)
{
	vector<unsigned> curList;
	int idx = 0;

	for (int i = 0; i < 2000; i++) //解压2000个链表,结果放在decompressed
	{
		curList = dgapDecompress(compressedLists, idx);

		//验证正确性用
		/*if (curList1.size() != curList2.size())
		{
			printf("wrong length\n");
			cout << curList1.size() << " " << curList2.size() << endl;
		}
		else {
			cout << curList1.size() << endl;
			for (int j = 0; j < curList1.size(); j++)
				if (curList1[j] != curList2[j])
					cout << "wrong" << endl;
		}*/

		result.push_back(curList);
		//curList.clear();
	}
}

void dgapDeMpi(const vector<unsigned>& compressedLists, vector<vector<unsigned>>& result,int rank, int proNum)
{
	int idx = 0;
	int listIndex[LIST_NUM] = { 0 };
	int bitNum;

	//读取每个链表的长度和每个docID所用的bit数
	for (int i = 0; i < LIST_NUM; i++)
	{
		// 前32位是长度
		int len = readBitData(compressedLists, idx, 32);

		//6位是用的Bit数
		bitNum = (int)readBitData(compressedLists, idx + 32, 6);

		// 下一个长度的位置为+=32 + 6 + len*bitNum,同时保存
		idx += 32 + 6 + len * bitNum;
		if (i != LIST_NUM - 1)
			listIndex[i + 1] = idx;
	}

	int start = LIST_NUM / proNum * rank;
	int end = min(LIST_NUM / proNum * (rank + 1), LIST_NUM);
	for (int i = start; i < end; i++)
	{
		vector<unsigned> curList;
		dgapDecompress(compressedLists, listIndex[i]);
		result.push_back(curList);
	}
}