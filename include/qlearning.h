#ifndef QLEARNING_H
#define QLEARNING_H

#include <dfs.h>

extern char** mazeEnv;
extern int** visited;
extern int rows;
extern int cols;
extern int start_row;
extern int start_col;
extern int state_row;
extern int state_col;
extern int goal_row;
extern int goal_col;

extern double** q;

void init_q();

int actions_to_int(action a);

action int_to_action(int i);

double max_actions(int s);

int best_action(int s);

void epsilon_greedy();

void botzmann_exploration();

void visualise();

void print_q();

void destroy_q();









#endif /* QLEARNING_H */
