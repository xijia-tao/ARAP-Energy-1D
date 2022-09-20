all: main
clean:
	rm -f out *.o
.PHONY: all clean

CXX = g++
CPPFLAGS =
CXXFLAGS = -std=c++11 -Wall -Wextra -g
LIBS = -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -pthread -lXi -ldl
LDFLAGS = -g

main: main.cpp Particle.cpp Spring.cpp Vector.cpp arap.cpp
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

run:
	./main files/simplemass.txt