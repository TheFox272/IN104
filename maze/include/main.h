#ifndef MAIN_H
#define MAIN_H

#include "mazeEnv.h"
#include "qlearning.h"

/*----------------------------------------------------------------------------------------------------*/

char** mazeEnv;
int** visited;
int rows;
int cols;
int start_row;
int start_col;
int state_row;
int state_col;
int goal_row;
int goal_col;

double** q;




#endif
