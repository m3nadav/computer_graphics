CXX = g++
CXXFLAGS = -std=c++11 -I./include -framework OpenGL -framework GLUT
LDFLAGS = -framework OpenGL -framework GLUT

# Shapes test
SHAPES_SRCS = src/shapes/shapes_test.cpp src/shapes/shapes.cpp
SHAPES_TARGET = shapes_test

# Visual test
VISUAL_SRCS = src/visual_test.cpp src/cow/cow.cpp src/cow/body.cpp src/cow/head.cpp src/cow/spots.cpp src/shapes/shapes.cpp src/scene/scene.cpp
VISUAL_TARGET = visual_test

all: $(SHAPES_TARGET) $(VISUAL_TARGET)

$(SHAPES_TARGET): $(SHAPES_SRCS)
	$(CXX) $(CXXFLAGS) $(SHAPES_SRCS) -o $(SHAPES_TARGET) $(LDFLAGS)

$(VISUAL_TARGET): $(VISUAL_SRCS)
	$(CXX) $(CXXFLAGS) $(VISUAL_SRCS) -o $(VISUAL_TARGET) $(LDFLAGS)

test-shapes: $(SHAPES_TARGET)
	@echo "Running shapes test..."
	./$(SHAPES_TARGET)

test-visual: $(VISUAL_TARGET)
	@echo "Running visual test..."
	./$(VISUAL_TARGET)

test: test-shapes test-visual

clean:
	rm -f $(SHAPES_TARGET) $(VISUAL_TARGET) 

.PHONY: clean all test test-shapes test-visual