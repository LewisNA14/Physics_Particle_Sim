CXX = g++
CXXFLAGS = -Wall -Wextra -O2
TARGET = test_physics
SRCS = test_physics.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -lgtest -lgtest_main -pthread -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)