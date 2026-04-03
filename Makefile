# Makefile for filesys project

# 定义编辑器
CXX = g++
# 定义编译选项
CXXFLAGS = -Wall -g

# 定义目标文件
OBJS = main.o igetput.o iallfre.o ballfre.o name.o access.o log.o close.o create.o delete.o dir.o dirlt.o open.o rdwt.o format.o install.o halt.o

# 定义最终可执行文件
TARGET = filesys

# 默认目标
all: $(TARGET)

# 链接目标文件生成可执行文件
# $@ 代表目标文件，$^ 代表所有的依赖文件（ .o 文件）
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^


# 定义编译规则(模式规则)
%.o: %.cpp filesys.h
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	rm -f $(OBJS) $(TARGET)