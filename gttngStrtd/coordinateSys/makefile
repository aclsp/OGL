CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude
LIBS = -lglfw -lGL -ldl -lX11 -lpthread

SRC = src/main.cpp src/glad.c src/shader.cpp src/stb_image.cpp
OUT = build/main

all:
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LIBS)
clean:
	rm -rf build
