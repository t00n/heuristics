CXXFLAGS = -Wall -Wextra -Werror -std=c++17
CXXFLAGS := -floop-interchange -floop-strip-mine -floop-block -floop-parallelize-all -ftree-parallelize-loops=4 -Ofast
# CXXFLAGS := -g

SRC := $(wildcard *.cpp)
HEAD := $(wildcard *.hpp)
OBJ := $(patsubst %.cpp,%.o,$(SRC))
EXEC = fssp

all: $(EXEC)


$(EXEC): $(OBJ) $(HEAD)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $@

clean:
	rm -rf $(OBJ) $(EXEC)

