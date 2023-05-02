CC = gcc
CFLAGS = -Wall -Werror
INC = include/
LIBS = -lm -lSDL2 -lSDL2_ttf
SRC = src/
BUILD = build/
EXEC = main


all: $(BUILD)$(EXEC)

$(BUILD)$(EXEC): $(BUILD)main.o $(BUILD)game.o $(BUILD)generator.o $(BUILD)qlearning.o
	$(CC) $(CFLAGS) -g $^ -o $@ $(LIBS)

$(BUILD)main.o: $(SRC)main.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)game.o: $(SRC)game.c
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)generator.o: $(SRC)generator.c
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)qlearning.o: $(SRC)qlearning.c
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD)
