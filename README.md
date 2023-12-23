# Intersection-Methods-and-Accelerattion
IR-hw6 brain stroming for infomation retrieval

## 项目结构说明
./
│  .gitignore
│  ExpIndex                 // 用于测试的索引文件(文件较大，未上传)
│  ExpQuery                 // 用于测试的查询文件(文件较大，未上传)
│  Makefile                 
│  README.md
│  
├─build
│  │  intersect.exe         // make生成的可执行文件
│  │
│  └─obj
│          intersect.d
│          intersect.o
│
├─include
│      Adp.h                // Adaptive算法
│      Bitmap.h             // Bitmap算法
│      Hash.h               // Hash算法
│      SkipBitmap.h         // SkipBitmap算法
│      InvertedIndex.h      // 倒排索引结构
│      util.h
│
└─src
        intersect.cpp        // 主程序


## Usage

```c
/* ./src/intersect.cpp中 */

// 使用不同的算法，只需要修改下面这一行
// 可测试的算法包括：ADP, Bitmap, SkipBitmap, Hash
#define Algorithm Hash

```

选择好算法后执行：
```
make run
```
对于算法进行测试。