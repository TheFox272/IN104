
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

double**** q;
double x;
double y;
double yP1;
double yP2;

double gateBonus;
double deathMalus;
double epsilon;
double epsilon_max;
double learning_rate;
double learning_rate_max;

int play(int);

int generate_level(char[21], int);
int load_level(char[21]);
int close_level();

int init_q();
void destroy_q();

int trainAI(int, char[26]);

int save_q(char[26]);
int save_q_history(char[26]);
int load_q(char[26]);