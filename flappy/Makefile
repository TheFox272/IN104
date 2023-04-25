CC = gcc
CFLAGS = -Wall -Werror
INC = include/
LIBS = -lm
SRC = src/
BUILD = build/
EXEC = main


all: $(BUILD)$(EXEC)

$(BUILD)$(EXEC): $(BUILD)main.o $(BUILD)game.o $(BUILD)display.o $(BUILD)qlearning.o
	$(CC) $(CFLAGS) -g $^ -o $@ $(LIBS)

$(BUILD)main.o: $(SRC)main.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)game.o: $(SRC)game.c
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)display.o: $(SRC)display.c
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)qlearning.o: $(SRC)qlearning.c $(INC)game.h $(INC)display.h
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD)
