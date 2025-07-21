CXX = g++
CXXFLAGS = -std=c++11 -I./include -framework OpenGL -framework GLUT
LDFLAGS = -framework OpenGL -framework GLUT

# Cow test
COW_SRCS = src/cow/cow_test.cpp src/cow/cow.cpp src/cow/body.cpp src/cow/spots.cpp src/shapes/shapes.cpp
COW_OBJS = $(COW_SRCS:.cpp=.o)
COW_TARGET = cow_test.app

# Shapes test
SHAPES_SRCS = src/shapes/shapes_test.cpp src/shapes/shapes.cpp
SHAPES_OBJS = $(SHAPES_SRCS:.cpp=.o)
SHAPES_TARGET = shapes_test.app

# Spots test
SPOTS_SRCS = src/cow/spots_test.cpp src/cow/spots.cpp src/shapes/shapes.cpp
SPOTS_OBJS = $(SPOTS_SRCS:.cpp=.o)
SPOTS_TARGET = spots_test.app

all: $(COW_TARGET) $(SHAPES_TARGET) $(SPOTS_TARGET)

$(COW_TARGET): $(COW_OBJS)
	$(CXX) $(COW_OBJS) -o $(COW_TARGET) $(LDFLAGS)

$(SHAPES_TARGET): $(SHAPES_OBJS)
	$(CXX) $(SHAPES_OBJS) -o $(SHAPES_TARGET) $(LDFLAGS)

$(SPOTS_TARGET): $(SPOTS_OBJS)
	$(CXX) $(SPOTS_OBJS) -o $(SPOTS_TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Test targets that compile and run
test-cow: $(COW_TARGET)
	@echo "Running cow test..."
	./$(COW_TARGET)

test-shapes: $(SHAPES_TARGET)
	@echo "Running shapes test..."
	./$(SHAPES_TARGET)

test-spots: $(SPOTS_TARGET)
	@echo "Running spots test..."
	./$(SPOTS_TARGET)

# Run all tests
test: test-cow test-shapes test-spots

clean:
	rm -f $(COW_OBJS) $(SHAPES_OBJS) $(SPOTS_OBJS) $(COW_TARGET) $(SHAPES_TARGET) $(SPOTS_TARGET)

.PHONY: clean all test test-cow test-shapes test-spots 