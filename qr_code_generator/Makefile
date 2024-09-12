CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS = -lqrencode -lpng

SRC = src/main.cpp
OBJ = $(SRC:.cpp=.o)
EXEC = qr_code_generator

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)
