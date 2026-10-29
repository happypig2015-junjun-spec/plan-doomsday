#==============================================================================
# Makefile —— 《末日回声》构建脚本
#   使用标准 C++20，开启常用警告，头文件统一在 include/ 下。
#==============================================================================

CXX      := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra -Wpedantic -Iinclude
TARGET   := doomsday
SRCS     := src/main.cpp src/Terminal.cpp src/StoryData.cpp src/GameEngine.cpp

.PHONY: all clean run

all: $(TARGET)

# 链接所有源文件生成可执行程序。
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# 构建并直接运行。
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
