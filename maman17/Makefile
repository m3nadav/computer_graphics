CXX = g++
CXXFLAGS = -std=c++11 -I./include -DGL_SILENCE_DEPRECATION
LDFLAGS = -framework OpenGL -framework GLUT

# Common source files used by all targets
COMMON_SRCS = src/common/camera.cpp src/common/input.cpp src/common/drawing.cpp src/common/menu.cpp

# Visual test
VISUAL_SRCS = src/visual_test.cpp src/cow/cow.cpp src/cow/body.cpp src/cow/tail.cpp src/cow/head.cpp src/cow/spots.cpp src/shapes/shapes.cpp src/scene/scene.cpp src/environment/environment.cpp src/environment/lights.cpp $(COMMON_SRCS)
VISUAL_TARGET = visual_test
VISUAL_FLAGS = -DCOW_CONTROLS_AVAILABLE

# Main application (with environment models)
MAIN_SRCS = main.cpp src/cow/cow.cpp src/cow/body.cpp src/cow/tail.cpp src/cow/head.cpp src/cow/spots.cpp src/shapes/shapes.cpp src/scene/scene.cpp src/environment/environment.cpp src/environment/lights.cpp $(COMMON_SRCS)
MAIN_TARGET = main
MAIN_FLAGS = -DCOW_CONTROLS_AVAILABLE

# Environment test
ENV_SRCS = src/env_test.cpp src/cow/cow.cpp src/cow/body.cpp src/cow/tail.cpp src/cow/head.cpp src/cow/spots.cpp src/shapes/shapes.cpp src/scene/scene.cpp src/environment/environment.cpp src/environment/lights.cpp $(COMMON_SRCS)
ENV_TARGET = env_test
ENV_FLAGS = -DCOW_CONTROLS_AVAILABLE

all: $(VISUAL_TARGET) $(MAIN_TARGET) $(ENV_TARGET)

$(VISUAL_TARGET): $(VISUAL_SRCS)
	$(CXX) $(CXXFLAGS) $(VISUAL_FLAGS) $(VISUAL_SRCS) -o $(VISUAL_TARGET) $(LDFLAGS)

$(MAIN_TARGET): $(MAIN_SRCS)
	$(CXX) $(CXXFLAGS) $(MAIN_FLAGS) $(MAIN_SRCS) -o $(MAIN_TARGET) $(LDFLAGS)

$(ENV_TARGET): $(ENV_SRCS)
	$(CXX) $(CXXFLAGS) $(ENV_FLAGS) $(ENV_SRCS) -o $(ENV_TARGET) $(LDFLAGS)

test-visual: $(VISUAL_TARGET)
	@echo "Running visual test..."
	./$(VISUAL_TARGET)

test-main: $(MAIN_TARGET)
	@echo "Running main application..."
	./$(MAIN_TARGET)

test: $(ENV_TARGET)
	@echo "Running environment test..."
	./$(ENV_TARGET) 1

test-env: test

clean:
	rm -f $(VISUAL_TARGET) $(MAIN_TARGET) $(ENV_TARGET)

.PHONY: clean all test test-visual test-main test-env