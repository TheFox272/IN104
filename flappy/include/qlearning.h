#ifndef QLEARNING_H
#define QLEARNING_H

#include <string.h>
#include <math.h>

#include "game.h"

/*----------------------------------------------------------------------------------------------------*/

extern double xTile;
extern double yTile;
extern double height;
extern double width;
extern double wMargin;
extern double x;
extern double y;
extern double pWidth;
extern double pSpace;
extern double yP1;
extern double yP2;
extern double g;
extern double jPower;
extern double dx;
extern double dy;
extern int levelSize;
extern int score;
extern int framePeriod;

extern double**** q;
extern int xRows;
extern int yPRows;
extern int dyRows;
extern int dyMin;
extern int dyMax;
extern int dyMin;
extern int nb_actions;

extern int hide;
extern int nb_episode;
extern int old_nb_episode;
extern int episode_done;
extern double learning_rate;
extern double return_rate;
extern double epsilon;
extern int ai_cycle;
extern double ai_speed;
extern double gateBonus;
extern double deathMalus;



int init_q();

void destroy_q();

int trainAI(int, char[26]);

int save_q(char[26]);

int load_q(char[26]);

#endif