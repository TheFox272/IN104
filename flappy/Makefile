CC = gcc
CFLAGS = -Wall -Werror
INC = include/
LIBS = -lm
SRC = src/
BUILD = build/
EXEC = main


all: $(BUILD)$(EXEC)

$(BUILD)$(EXEC): $(BUILD)main.o
	$(CC) $(CFLAGS)  -g $^ -o $@ $(LIBS)

$(BUILD)main.o: $(SRC)main.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD)
