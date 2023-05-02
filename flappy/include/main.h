#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "game.h"
#include "generator.h"
#include "qlearning.h"

/*----------------------------------------------------------------------------------------------------*/

double xTile;
double yTile;
double height;
double width;
double wMargin;
double xOrigin;
double flappySize;
double x;
double y;
double pWidth;
double pSpace;
double yP1;
double yP2;
double g;
double jPower;
double dx;
double dy;
int levelSize;
int score;
int framePeriod;
int* pillarsY;
int start;

double**** q;
int xRows;
int yPRows;
int dyRows;
int dyMin;
int dyMax;
int dyMin;
int nb_actions;

int hide;
int nb_episode;
int old_nb_episode;
int episode_done;
double learning_rate;
double return_rate;
double epsilon;
int ai_cycle;
double ai_speed;
double gateBonus;
double deathMalus;

#endif