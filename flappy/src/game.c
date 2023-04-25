#include "game.h"

/*----------------------------------------------------------------------------------------------------*/

void init()
{
    // units are arbitrary, but 1 should convert to 1 character on display
    height = 100;  // height of the playscreen
    width = 60;  // distance between two pillars
    x = 0;  // position relative to previous
    y = height / 2;
    ySpace = 20;  // space between top and bottom pillars (height of a gate)
    y11 = y - ySpace / 2;
    y12 = y + ySpace / 2;
    y21 = y - ySpace / 2;  // to define randomly
    y22 = y + ySpace / 2;  // to define randomly
    xSpeed = 1;  // in units per frame
    dx = 0;
    dy = 0;
    score = 0;  // number of pillars passed
}


// to be continued ...

