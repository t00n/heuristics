CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -g

SRC := $(wildcard *.cpp)
HEAD := $(wildcard *.hpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))
EXEC = fssp

all: $(EXEC)


$(EXEC): $(OBJ) $(HEAD)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

clean:
	rm -rf $(OBJ) $(EXEC)

