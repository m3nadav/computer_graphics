CXX = g++
CXXFLAGS = -std=c++11 -I./include -framework OpenGL -framework GLUT
LDFLAGS = -framework OpenGL -framework GLUT

# Cow test
COW_SRCS = src/cow/cow_test.cpp src/cow/cow.cpp src/shapes/shapes.cpp
COW_OBJS = $(COW_SRCS:.cpp=.o)
COW_TARGET = cow_test

# Shapes test
SHAPES_SRCS = src/shapes/shapes_test.cpp src/shapes/shapes.cpp
SHAPES_OBJS = $(SHAPES_SRCS:.cpp=.o)
SHAPES_TARGET = shapes_test

all: $(COW_TARGET) $(SHAPES_TARGET)

$(COW_TARGET): $(COW_OBJS)
	$(CXX) $(COW_OBJS) -o $(COW_TARGET) $(LDFLAGS)

$(SHAPES_TARGET): $(SHAPES_OBJS)
	$(CXX) $(SHAPES_OBJS) -o $(SHAPES_TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test targets that compile and run
test-cow: $(COW_TARGET)
	@echo "Running cow test..."
	./$(COW_TARGET)

test-shapes: $(SHAPES_TARGET)
	@echo "Running shapes test..."
	./$(SHAPES_TARGET)

# Run both tests
test: test-cow test-shapes

clean:
	rm -f $(COW_OBJS) $(SHAPES_OBJS) $(COW_TARGET) $(SHAPES_TARGET)

.PHONY: clean all test test-cow test-shapes 