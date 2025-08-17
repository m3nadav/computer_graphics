CXX = g++
CXXFLAGS = -std=c++11 -I./include -DGL_SILENCE_DEPRECATION
LDFLAGS = -framework OpenGL -framework GLUT

# Common source files used by all targets
COMMON_SRCS = src/common/camera.cpp src/common/input.cpp src/common/drawing.cpp src/common/menu.cpp

# Main application (with environment models)
MAIN_SRCS = main.cpp src/cow/cow.cpp src/cow/body.cpp src/cow/tail.cpp src/cow/head.cpp src/cow/spots.cpp src/shapes/shapes.cpp src/scene/scene.cpp src/environment/environment.cpp src/environment/lights.cpp $(COMMON_SRCS)
MAIN_TARGET = main_scene
MAIN_FLAGS = -DCOW_CONTROLS_AVAILABLE

# Manual tests
TEST_SRCS = test.cpp src/cow/cow.cpp src/cow/body.cpp src/cow/tail.cpp src/cow/head.cpp src/cow/spots.cpp src/shapes/shapes.cpp src/scene/scene.cpp src/environment/environment.cpp src/environment/lights.cpp $(COMMON_SRCS)
TEST_TARGET = test_scene
TEST_FLAGS = -DCOW_CONTROLS_AVAILABLE

all: $(MAIN_TARGET) $(TEST_TARGET)

$(MAIN_TARGET): $(MAIN_SRCS)
	$(CXX) $(CXXFLAGS) $(MAIN_FLAGS) $(MAIN_SRCS) -o $(MAIN_TARGET) $(LDFLAGS)

$(TEST_TARGET): $(TEST_SRCS)
	$(CXX) $(CXXFLAGS) $(TEST_FLAGS) $(TEST_SRCS) -o $(TEST_TARGET) $(LDFLAGS)

main: $(MAIN_TARGET)
	@echo "Running main application..."
	./$(MAIN_TARGET)

test: $(TEST_TARGET)
	@echo "Running manual tests..."
	./$(TEST_TARGET) 1

clean:
	rm -f $(MAIN_TARGET) $(TEST_TARGET)

.PHONY: clean all main test 