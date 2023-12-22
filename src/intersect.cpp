#include <iostream>
#include <vector>
#include <Windows.h>
#include <algorithm>
#include "InvertedIndex.h"
#include "util.h"
#include "Adp.h"
#include "Bitmap.h"
#include "SkipBitmap.h"
#include "Hash.h"

// 使用不同的算法，只需要修改下面这一行
// 可测试的算法包括：ADP, Bitmap, SkipBitmap, Hash
#define Algorithm Hash

using namespace std;

int main() {
	//测试
	//verify();
	//读取二进制文件
	static vector<InvertedIndex> invertedLists;
	static int query[1000][5] = { 0 };// 单个查询最多5个docId,全部读取
	int count;
	getData(invertedLists, query,count);
	//------倒排链表求交------
	long long head, tail, freq;

	//预处理
	if (Algorithm == Hash)
		preprocessing(invertedLists, 2000);
	else if (Algorithm == Bitmap || Algorithm == SkipBitmap)
		bitMapProcessing(invertedLists, 2000);
	int step = 50;
	for (int k = 50; k <= count; k += step)
	{	
		float totalTime = 0;
		//开始计时检测
		for (int t = 0; t < 5; t++)
		{
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
			QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time
			for (int i = 0; i < k; i++) // k个查询
			{
				int num = 0;// query查询项数
				for (int j = 0; j < 5; j++)
					if (query[i][j] != 0)
						num++;
				int* list = new int[num];// 要传入的list
				for (int j = 0; j < num; j++)
					list[j] = query[i][j];
				sorted(list, (invertedLists), num);//按长度排序
                Algorithm(list, invertedLists, num);
			}
			QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
			totalTime += (tail - head) * 1000.0 / freq;
		}
		//cout << k << endl;
		cout << totalTime/5<<endl;
		if (k == 100)
			step = 100;
		if (k == 400)
			step = 200;
	}

	return 0;
}