CXX     = g++
CXXLAGS = -Wall -pedantic -std=c++11 
INCL	= -lsimlib -lm
SRC     = src/*.cpp
EXEC    = ims



all: build run

build:
	$(CXX) $(CXXLAGS) $(SRC) $(INCL) -o $(EXEC)

run:
	@./$(EXEC)

clean:
	rm -rf $(EXEC)
