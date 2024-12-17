
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = game
SOURCES = main.c
HEADERS = main.h
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Building $(TARGET)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Build complete! Run with ./$(TARGET)"

%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET) $(OBJECTS)
	@echo "Clean complete."

rebuild: clean all

run: all
	@echo "Running the game..."
	./$(TARGET)
