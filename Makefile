# Compiler to use
CXX = clang++

# Compiler flags
CXXFLAGS = -Wall -std=c++20

# Name of the executable
TARGET = coherence

# List of source files
SOURCES = $(wildcard *.cpp)

# List of object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule
all: $(TARGET)

# Rule to link object files to create the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean the build
clean:
	rm -f $(OBJECTS) $(TARGET)

