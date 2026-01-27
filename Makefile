CXX = g++
CXXFLAGS = -Wall -Wextra -O2

TARGET = main
SRCS = main.cpp

# SDL package name for pkg-config
PKGS = sdl3

CFLAGS := $(shell pkg-config --cflags $(PKGS))
LIBS   := $(shell pkg-config --libs $(PKGS))

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(CFLAGS) -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
