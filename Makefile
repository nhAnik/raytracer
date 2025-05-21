# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17

# OpenGL libraries
LIBS = -lGL -lGLU -lglut

# Source files list
SRCS = \
	main.cpp \
	scene.cpp \
	sphere.cpp \
	pyramid.cpp \
	point.cpp

# Object files list (auto-generated from SRCS)
OBJS = $(SRCS:.cpp=.o)

# Output binary
TARGET = raytracer

# Default rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compile rule for .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)

# Run rule
run: $(TARGET)
	./$(TARGET) --input input.txt --output out.bmp
