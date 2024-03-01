CXX=clang++
CXXFLAGS=-march=native -std=c++26 -Wall
CXXLIBFLAGS=-shared -fpic
LD=lld

INC=-I./include -I./src

SRC_PARSER=parser.cpp
LIB_PARSER=build/yapvm_parser.so

SRC=main.cpp

all: executable

debug: CXXFLAGS += -ggdb
debug: executable


executable:
	$(CXX) $(CXXFLAGS) $(CXXLIBFLAGS) $(INC) -o $(LIB_PARSER) $(SRC_PARSER)

clean:
	rm -r $(LIB_PARSER)
