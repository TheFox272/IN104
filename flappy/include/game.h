#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/*----------------------------------------------------------------------------------------------------*/

extern double xTile;
extern double yTile;
extern double height;
extern double width;
extern double wMargin;
extern double xOrigin;
extern double flappySize;
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
extern int maxScore;
extern int framePeriod;
extern int* pillarsY;
extern int start;

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
extern double epsilon;
extern double epsilon_max;
extern double learning_rate;
extern double learning_rate_max;
extern double return_rate;
extern int ai_cycle;
extern double ai_speed;
extern double gateBonus;
extern double deathMalus;
extern int stuckTime;

typedef struct
{
    int xRow;
    int yPRow;
    int dyRow;
    int jump;
} state;

int init(int);

int init_pos(int);

int next(int);

int load_level(char[21]);

void close_level();

int play(int);

#endif