# MODE := RELEASE
MODE := DEBUG

SRC_DIR := src
INC_DIR := includes
BUILD_DIR := build

CC := g++
#CC := g++
TARGET := lab2-monte-carlo
# 源文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
# 目标文件
OBJS = $(patsubst %.cpp, $(BUILD_DIR)/%.o, $(notdir $(SRCS)))
# 头文件
DEPS = $(wildcard $(INC_DIR)/*.h)

FLAGS := -I $(INC_DIR) -Weffc++
ifeq ($(MODE), DEBUG)
	FLAGS += -DDEBUG -g3 -ggdb -gdwarf-2
endif

.PHONY : clean

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) -g -o $@ $^
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DPES)
	if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) -g -c -o $@ $< $(FLAGS)
clean:
# rm -rf $(BUILD_DIR)
	rmdir /s $(BUILD_DIR)
