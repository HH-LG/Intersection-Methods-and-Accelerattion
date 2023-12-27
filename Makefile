# 定义编译器
CC = g++

# 定义编译选项
CFLAGS = -Wall -std=c++11 -fexec-charset=GBK
INC = -I.\include
OBJ_PATH = build\obj

# 源文件和目标文件
INTER_SRC = src/intersect.cpp
INTERSECT_OBJ = $(patsubst src/%.cpp, $(OBJ_PATH)/%.o, $(INTER_SRC))
INTERSECT = build\intersect.exe

TARGET = $(INTERSECT) 
# 默认目标
all: $(TARGET)

$(INTERSECT): $(INTERSECT_OBJ) 
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_PATH)/%.o: src/%.cpp
	if not exist $(OBJ_PATH) mkdir $(OBJ_PATH)
	$(CC) $(CFLAGS) $(INC) -MMD -MP -c $< -o $@
	$(CC) $(CFLAGS) $(INC) -MMD -MP -MF $(patsubst %.o, %.d, $@) -c $< -o $@

# 运行求交程序
run: $(INTERSECT)
	./$(INTERSECT)

# 清理目标
clean:
	del /Q $(OBJ_PATH)\*.o
	del /Q $(OBJ_PATH)\*.d
	del /Q $(TARGET)

.PHONY: all clean run 
-include $(OBJ_PATH)/*.d
