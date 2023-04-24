CC = gcc
CFLAGS = -Wall -Werror
INC = include/
LIBS = -lm
SRC = src/
BUILD = build/
EXEC = main


all: $(BUILD)$(EXEC)

$(BUILD)$(EXEC): $(BUILD)main.o $(BUILD)mazeEnv.o $(BUILD)qlearning.o $(BUILD)colored_output.o $(BUILD)functions.o
	$(CC) $(CFLAGS)  -g $^ -o $@ $(LIBS)

$(BUILD)main.o: $(SRC)main.c $(INC)mazeEnv.h $(INC)qlearning.h 
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)functions.o: $(SRC)functions.c
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)colored_output.o: $(SRC)colored_output.c
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)mazeEnv.o:  $(SRC)mazeEnv.c $(INC)functions.h
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)dfs.o: $(SRC)dfs.c $(INC)mazeEnv.h
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@

$(BUILD)qlearning.o: $(SRC)qlearning.c $(INC)mazeEnv.h $(INC)colored_output.h
	$(CC) $(CFLAGS) -I $(INC) -g -c $< -o $@


.PHONY: clean
clean:
	rm -r $(BUILD)

# clean:
# 	find . -type f -name "*.o" -delete

# realclean: clean
# 	find . -type f -name "*.x" -delete
