CC = clang++
UNAME_S := $(shell uname -s)

TARGET = window_position
SRC = window_position.cpp

ifeq ($(UNAME_S),Darwin)
    # macOS
    CFLAGS = -framework OpenGL -framework GLUT
else ifeq ($(UNAME_S),Linux)
    # Linux
    CFLAGS = -lGL -lGLU -lglut
else
    # Windows (assuming MinGW/MSYS)
    CFLAGS = -lopengl32 -lglu32 -lfreeglut
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all clean 