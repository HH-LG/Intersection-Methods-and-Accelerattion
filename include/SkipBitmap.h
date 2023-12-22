#pragma once
#include <string>
#include <list>
#include <vector>
#include <algorithm>
#include "InvertedIndex.h"
#include "Bitmap.h"
InvertedIndex SkipBitmap(int* queryList, vector<InvertedIndex>& idx, int num)
{
    copy(bitmapList[queryList[0]].secondIndex.begin(), bitmapList[queryList[0]].secondIndex.end(), chosenList.secondIndex.begin());
	copy(bitmapList[queryList[0]].firstIndex.begin(), bitmapList[queryList[0]].firstIndex.end(), chosenList.firstIndex.begin());
	copy(bitmapList[queryList[0]].bits.begin(), bitmapList[queryList[0]].bits.end(), chosenList.bits.begin());
	InvertedIndex res;
	for (int i = 1; i < num; i++)
	{
		list<int>::iterator tmp;
		list<int>::iterator j = skipPointer.begin();
		while (j!=skipPointer.end())
		{
			chosenList.secondIndex[*j] &= bitmapList[queryList[i]].secondIndex[*j];//按位与
			if (chosenList.secondIndex[*j])
			{
				for (int t = (*j) * 32; t < (*j) * 32 + 32; t++)
				{
					chosenList.firstIndex[t] &= bitmapList[queryList[i]].firstIndex[t];//按位与
					if (chosenList.firstIndex[t])
					{
						for (int l = t * 32; l < t * 32 + 32; l++)
						{
							chosenList.bits[l] &= bitmapList[queryList[i]].bits[l];//按位与
							if (i == num - 1 && chosenList.bits[l])
							{
								int bit = chosenList.bits[l];//取出该段
								for (int m = 0; m < 32; m++)
								{
									if ((bit & 1) != 0)
										res.docIdList.push_back(32 * l + m);
									bit = bit >> 1;
								}
							}
						}
							
					}
				}
				j++;
			}
			else
			{
				tmp = j;
				j++;
				skipPointer.erase(tmp);
			}
		}
	}
	return res;
}