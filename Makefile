OBJ_DIR=obj
SRC_DIR=src
OUT_DIR=out

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

CXX=g++
INCLUDE=-I$(SRC_DIR)/include -I/usr/local/include
CFLAGS=-g $(INCLUDE) -c -Wall -std=c++11
LDFLAGS=
LDLIBS=-lsfml-graphics -lsfml-window -lsfml-system -lboost_filesystem -lboost_system

all: dirs main
        
dirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(OUT_DIR)

resources:
	cp -r $(SRC_DIR)/Resources $(OUT_DIR)/Resources

main: resources $(OBJS)
	$(CXX) -o $(OUT_DIR)/$@ $(OBJS) $(LDLIBS) $(LDFLAGS) 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CFLAGS) $< -o $@ $(LDLIBS) $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(OUT_DIR)
