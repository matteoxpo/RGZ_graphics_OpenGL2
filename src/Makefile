.PHONY: all, clean, rebuild

CC = gcc
CFLAGS = -c
GLFLAGS = -lglut -lGL -lGLU -lm -lSOIL

BUILD_PATH = ../build/
SOURCES =  drop.c 
OBJECTS = $(SOURCES:.c=.o)

EXECUTABLE = MAIN

all: $(OBJECTS)
	$(CC) $(BUILD_PATH)$(OBJECTS) -o $(BUILD_PATH)$(EXECUTABLE) $(GLFLAGS)
	$(BUILD_PATH)$(EXECUTABLE)


.PHONY: rebuild
rebuild: clean all

.PHONY: re
re: rebuild

%.o: %.c
	$(CC) $(CFLAGS) $< -o $(BUILD_PATH)$@

.PHONY: clean
clean:
	-rm  $(BUILD_PATH)*