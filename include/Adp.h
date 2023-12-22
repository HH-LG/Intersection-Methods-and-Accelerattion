#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "InvertedIndex.h"
using namespace std;

// adp实现
class QueryItem {
public:
	int cursor;// 当前读到哪了
	int length;// 倒排索引总长度
	int key;// 关键字值
};
bool operator<(const QueryItem& q1, const QueryItem& q2) {// 选剩余元素最少的元素
	return (q1.length - q1.cursor) < (q2.length - q2.cursor);
}
InvertedIndex ADP(int* queryList, vector<InvertedIndex>& index, int num)
{
	InvertedIndex S;
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// 预处理
	{
		list[i].cursor = 0;
		list[i].key = queryList[i];
		list[i].length = index[queryList[i]].length;
	}

	while (list[0].cursor < list[0].length) {// 最短的列表非空
		bool isFind = true;
		int s = 1;
		unsigned int e = index[list[0].key].docIdList[list[0].cursor];
		while (s != num && isFind == true) {
			isFind = false;
			while (list[s].cursor < list[s].length) {// 检查s列表
				if (e == index[list[s].key].docIdList[list[s].cursor]) {
					isFind = true;
					break;
				}
				else if (e < index[list[s].key].docIdList[list[s].cursor])
					break;
				list[s].cursor++;// 当前访问过，且没找到合适的，往后移
			}
			s++;// 下一个链表
		}
		list[0].cursor++;// 当前元素已被访问过
		if (s == num && isFind) {
			S.docIdList.push_back(e);
			S.length++;
		}
		//sort(list, list + num);// 重排，将未探查元素少的列表前移
	}
	return S;
}