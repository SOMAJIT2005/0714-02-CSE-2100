# 1. Compiler and Output
CXX = g++
TARGET = WORDS_COLLIDE.exe

# 2. Directories
SRC_DIR = src
INC_DIR = include

# 3. Compiler and Linker Flags
# Added your specific SDL2 include path here
CXXFLAGS = -std=c++17 -Wall -I$(INC_DIR) -IE:/SDL2-2.32.0/i686-w64-mingw32/include -pipe
# Added your specific SDL2 lib path here
LDFLAGS = -LE:/SDL2-2.32.0/i686-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_mixer -lSDL2_net -lSDL2_ttf

# 4. Source Files 
SRCS = $(SRC_DIR)/main.cpp \
       $(wildcard $(SRC_DIR)/Core/*.cpp) \
       $(wildcard $(SRC_DIR)/Game/*.cpp) \
       $(wildcard $(SRC_DIR)/Systems/*.cpp) \
       $(wildcard $(SRC_DIR)/States/*.cpp) \
       $(wildcard $(SRC_DIR)/UI/*.cpp) \
       $(wildcard $(SRC_DIR)/MVC/*.cpp)

# 5. Object Files 
OBJS = $(SRCS:.cpp=.o)

# --- BUILD RULES ---

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)