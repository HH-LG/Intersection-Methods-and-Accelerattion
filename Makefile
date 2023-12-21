# 定义编译器
CC = g++

# 定义编译选项
CFLAGS = -Wall -std=c++11
INC = -I.\include -I"C:\Program Files (x86)\Microsoft SDKs\MPI\Include"
LIB = -L "C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi
OBJ_PATH = build\obj

# 源文件和目标文件
INTER_SRC = src/intersect.cpp
INTERSECT_OBJ = $(patsubst src/%.cpp, $(OBJ_PATH)/%.o, $(INTER_SRC))
COMPRESS_SRC = src/compress.cpp
COMPRESS_OBJ = $(patsubst src/%.cpp, $(OBJ_PATH)/%.o, $(COMPRESS_SRC))
COMPRESS_MPI_SRC = src/mpi_compress.cpp
COMPRESS_MPI_OBJ = $(patsubst src/%.cpp, $(OBJ_PATH)/%.o, $(COMPRESS_MPI_SRC))
COMPRESS = build\compress.exe
INTERSECT = build\intersect.exe
COMPRESS_MPI = build\mpi_compress.exe

TARGET = $(INTERSECT) $(COMPRESS) $(COMPRESS_MPI)
# 默认目标
all: $(TARGET)

$(INTERSECT): $(INTERSECT_OBJ) 
	$(CC) $(CFLAGS) $^ -o $@

$(COMPRESS): $(COMPRESS_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

$(COMPRESS_MPI): $(COMPRESS_MPI_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIB)

$(OBJ_PATH)/%.o: src/%.cpp
	if not exist $(OBJ_PATH) mkdir $(OBJ_PATH)
	$(CC) $(CFLAGS) $(INC) -MMD -MP -c $< -o $@
	$(CC) $(CFLAGS) $(INC) -MMD -MP -MF $(patsubst %.o, %.d, $@) -c $< -o $@

# 运行求交程序
run: $(INTERSECT)
	./$(INTERSECT)

# 运行压缩程序
rcs: $(COMPRESS)
	./$(COMPRESS)

# 运行并行压缩程序
rcs_mpi: $(COMPRESS_MPI)
	mpiexec -n 4 $(COMPRESS_MPI)

# 清理目标
clean:
	del /Q $(OBJ_PATH)\*.o
	del /Q $(OBJ_PATH)\*.d
	del /Q $(TARGET)

.PHONY: all clean run rcs
-include $(OBJ_PATH)/*.d
