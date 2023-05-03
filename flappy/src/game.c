#include "game.h"

/*----------------------------------------------------------------------------------------------------*/
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
/*----------------------------------------------------------------------------------------------------*/

int init(int ai)
{
    // #region : AI stuff (can be modified, but will make old AIs obsolete)
    if (!ai)
    {
        hide = 0;
        epsilon = 0;
        ai_speed = 1;
    }
    else
    {
        epsilon = 0.2;  // used by the epsilon greedy method
        ai_speed = 10;  // can't be more than framePeriod
    }
    learning_rate = 1.0;  // the speed at which the weights of q will be changed (cf alpha in the lesson)
    return_rate = 1.0;  // cf gamma in the lesson
    ai_cycle = 15;
    gateBonus = 64;
    deathMalus = -32;

    xTile = 30;  // size of a tile on the xAxis
    yTile = 20;  // size of a tile on the yAxis
    // #endregion

    // #region : DO NOT CHANGE (or be very careful with it, those parameters are interlinked)
    height = 640;  // height of the playscreen
    width = 380;  // distance between two pillars
    xOrigin = 120;
    wMargin = 50;  // width margin (2 * wMargin + width = screen width)
    flappySize = 30;
    pWidth = 90;  // width of a pillar (width of a gate)
    pSpace = 150;  // height of a gate
    g = 0.3;  // gravity strength
    jPower = 6.5;  // jump strength
    x = flappySize / 2;  // x position relative to previous pillar
    y = height / 2;
    dx = 2;  // scrolling speed
    dy = 0;  // y speed initialization
    score = 0;  // number of pillars passed
    framePeriod = (int)(10 / ai_speed) + 1;  // in ms
    start = 0;
    // #endregion

    return 0;
}


int next(int jump)  // jump = 1 if flappy jumps, 0 if he waits
{
    if (start)
    {
        if (jump)
            dy = jPower;
        else
            dy += -g;

        x += dx;
        y += dy;

        if (x >= width)
        {
            x = 0;
            score += 1;
            yP1 = pillarsY[score];
            if (score + 1 < levelSize)
                yP2 = pillarsY[score + 1];
            else
                yP2 = -1;
        }
        return 0;
    }
    else
    {
        start = 1;
        yP1 = pillarsY[score];
        yP2 = pillarsY[score + 1];
        return 0;
    }
}


SDL_bool dead(SDL_Rect *flappyRect, SDL_Rect *pillarRects)
{
    if (score + 1 == levelSize || y < 0 || height <= y)
        return SDL_TRUE;
    for (int i = 0; i < 4; i++)
    {
        if (SDL_HasIntersection(flappyRect, &pillarRects[i]))
            return SDL_TRUE;
    }

    return SDL_FALSE;
}


void actualise_state(state *s)
{
    int xRow = (int)(x / xTile);
    if (xRow < 0)
        xRow = 0;
    else if (xRow > (int)(width/ xTile))
        xRow = (int)(width/ xTile);
    s->xRow = xRow;
    int yPRow;
    if (xRow < xRows / 4)
        yPRow = (int)((height - 2 * pSpace + y - yP1) / yTile);
    else
        yPRow = (int)((height - 2 * pSpace + y - yP2) / yTile);
    if (yPRow < 0)
        yPRow = 0;
    else if (yPRow > (int)((2 * height - 4 * pSpace) / yTile))
        yPRow = (int)((2 * height - 4 * pSpace) / yTile);
    s->yPRow = yPRow;
    int dyRow = (int)(dy / 2);
    if (dyRow < dyMin)
        s->dyRow = 0;
    else if (dyRow >= dyMax)
        s->dyRow = 1 + dyMax - dyMin;
    else
        s->dyRow = 1 + dyRow - dyMin;
}


double max_jump(state s)
{
    double a = q[s.xRow][s.yPRow][s.dyRow][0];
    double b = q[s.xRow][s.yPRow][s.dyRow][1];
    return (a >= b) ? 0 : 1;
}


double max_gain(state s)
{
    double a = q[s.xRow][s.yPRow][s.dyRow][0];
    double b = q[s.xRow][s.yPRow][s.dyRow][1];
    return (a >= b) ? a : b;
}


int load_level(char levelName[21])
{
    char filename[40] = "../data/levels/";
    strcat(filename, levelName);
    strcat(filename, ".txt");
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("error reading the file " GREEN "%s" RESET "\n", filename);
        return -1;
    }
    char temp[10];
    fgets(temp, 10, f);
    levelSize = atoi(temp);
    pillarsY = malloc(levelSize * sizeof(int));
    for (int i = 0; i < levelSize; i++)
    {
        if (fgets(temp, 10, f))
            pillarsY[i] = atoi(temp);
        else
        {
            printf("error reading the file " GREEN "%s" RESET " : too short for indicated size\n", filename);
            free(pillarsY);
            return -1;
        }
    }
    if (fgets(temp, 10, f))
    {
        printf("error reading the file " GREEN "%s" RESET " : too long for indicated size\n", filename);
        free(pillarsY);
        return -1;
    }
    else
        printf("successfully loaded the level from the file " GREEN "%s" RESET "\n", filename);
    fclose(f);
    return 0;
}


void close_level()
{
    free(pillarsY);
}


int play(int ai) // ai = 0 for manual, -1 for epsilon greedy without display, 1 for epsilon greedy with display
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    int statut = EXIT_FAILURE;

    // #region : will be used for display
    SDL_Color orange = {255, 127, 40, 255};
    SDL_Color green = {28, 150, 28, 255};
    SDL_Color sky = {206, 237, 247, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Rect pillarRects[4];
    SDL_Rect flappyRect;
    flappyRect.w = flappySize;
    flappyRect.h = flappySize;
    SDL_Texture *scoreTexture = NULL;
    int scoreWidth;
    int scoreHeight;
    SDL_Rect scoreRect;
    char scoreText[5];
    TTF_Font *myFont = NULL;
    // #endregion

    SDL_Event event;
    SDL_bool jump = SDL_FALSE;
    SDL_bool quit = SDL_FALSE;
    SDL_bool go = SDL_FALSE;

    // #region : SDL init stuff
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "error SDL_Init : %s\n", SDL_GetError());
        goto Quit;
    }
    if (!hide)
    {
        if (TTF_Init() != 0)
        {
            fprintf(stderr, "error TTF_Init : %s\n", SDL_GetError());
            goto Quit;
        }
        myFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf", 30);
        if (myFont == NULL)
        {
            fprintf(stderr, "error TTF_OpenFont : %s\n", SDL_GetError());
            goto Quit;
        }
        SDL_CreateWindowAndRenderer(480, 640, SDL_WINDOW_SHOWN, &window, &renderer);
        if (window == NULL)
        {
            fprintf(stderr, "error SDL_CreateWindow : %s\n", SDL_GetError());
            goto Quit;
        }
        if (renderer == NULL)
        {
            fprintf(stderr, "error SDL_CreateRenderer : %s\n", SDL_GetError());
            goto Quit;
        }
    }
    // #endregion

    // #region : AI parameters initialisation
    int cycle = 0;
    double draw;
    srand(time(NULL));
    double e0;
    double e1;
    state old_state;
    state new_state;
    int old_score = score;
    double q_value;
    if (ai)
        actualise_state(&old_state);
    // #endregion
    
    while (!go || (!dead(&flappyRect, pillarRects) && !quit))
    {
        while (SDL_PollEvent(&event))
        {
            if (!ai && event.type == SDL_QUIT)
                quit = SDL_TRUE;
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_q)  // safety quit
                {
                    statut = -1;
                    printf(" manual interruption of level\n");
                    goto Quit;
                }
                else if (event.key.keysym.sym == SDLK_SPACE && !ai)
                    jump = SDL_TRUE;
            }
        }

        cycle++;
        if (ai && cycle == ai_cycle)
        {
            cycle = 0;
            actualise_state(&new_state);
            q_value = q[old_state.xRow][old_state.yPRow][old_state.dyRow][old_state.jump];
            q[old_state.xRow][old_state.yPRow][old_state.dyRow][old_state.jump] = q_value + learning_rate * ((score - old_score) * gateBonus + return_rate * max_gain(new_state) - q_value);
            old_state = new_state;
            old_score = score;

            if (abs(ai) == 1) // epsilon_greedy
            {
                draw = rand() / (RAND_MAX + 1.0);
                if (draw < epsilon)
                {
                    if (draw < epsilon / 2)
                        jump = SDL_TRUE;
                }
                else if (max_jump(new_state))
                    jump = SDL_TRUE;
            }
            else if (abs(ai) == 2) // boltzmann_exploration
            {
                e0 = exp(q[old_state.xRow][old_state.yPRow][old_state.dyRow][0]);
                e1 = exp(q[old_state.xRow][old_state.yPRow][old_state.dyRow][1]);
                e0 /= e0 + e1;
                draw = rand() / (RAND_MAX + 1.0);
                if (draw > e0)
                    jump = SDL_TRUE;
            }

            old_state.jump = (int)jump;
        }

        next(jump);

        if (!hide)
        {
            if (SDL_SetRenderDrawColor(renderer, sky.r, sky.g, sky.b, sky.a))
            {
                fprintf(stderr, "error SDL_SetRenderDrawColor : %s\n", SDL_GetError());
                goto Quit;
            }
            if (SDL_RenderClear(renderer) != 0)
            {
                fprintf(stderr, "error SDL_RenderClear : %s", SDL_GetError());
                goto Quit;
            }
        }

        pillarRects[0].x = xOrigin - x;
        pillarRects[0].y = 0;
        pillarRects[0].w = pWidth;
        pillarRects[0].h = height - yP1 - pSpace;
        pillarRects[1].x = xOrigin - x;
        pillarRects[1].y = height - yP1;
        pillarRects[1].w = pWidth;
        pillarRects[1].h = yP1;
        if (yP2 != -1)
        {
            pillarRects[2].x = xOrigin + width - x;
            pillarRects[2].y = 0;
            pillarRects[2].w = pWidth;
            pillarRects[2].h = height - yP2 - pSpace;
            pillarRects[3].x = xOrigin + width - x;
            pillarRects[3].y = height - yP2;
            pillarRects[3].w = pWidth;
            pillarRects[3].h = yP2;
        }
        else
        {
            pillarRects[2].x = 0;
            pillarRects[2].y = 0;
            pillarRects[2].w = 0;
            pillarRects[2].h = 0;
            pillarRects[3].x = 0;
            pillarRects[3].y = 0;
            pillarRects[3].w = 0;
            pillarRects[3].h = 0;
        }
        if (!hide)
        {
            if (SDL_SetRenderDrawColor(renderer, green.r, green.g, green.b, green.a) != 0)
            {
                fprintf(stderr, "error SDL_SetRenderDrawColor : %s\n", SDL_GetError());
                goto Quit;
            }
            if (SDL_RenderFillRects(renderer, pillarRects, 4) != 0)
            {
                fprintf(stderr, "error SDL_RenderFillRects : %s\n", SDL_GetError());
                goto Quit;
            }
        }

        flappyRect.x = xOrigin - flappySize / 2;
        flappyRect.y = height - y + flappySize / 2;
        if (!hide)
        {
            if (SDL_SetRenderDrawColor(renderer, orange.r, orange.g, orange.b, orange.a) != 0)
            {
                fprintf(stderr, "error SDL_SetRenderDrawColor : %s\n", SDL_GetError());
                goto Quit;
            }
            if (SDL_RenderFillRect(renderer, &flappyRect) != 0)
            {
                fprintf(stderr, "error SDL_RenderFillRect : %s\n", SDL_GetError());
                goto Quit;
            }

            sprintf(scoreText, "%d", score);
            SDL_Surface *scoreSurface = TTF_RenderText_Solid(myFont, scoreText, black);
            if (scoreSurface == NULL)
            {
                fprintf(stderr, "error TTF_RenderText_Solid : %s\n", SDL_GetError());
                goto Quit;
            }
            scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
            if (scoreTexture == NULL)
            {
                fprintf(stderr, "error SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
                goto Quit;
            }

            scoreWidth = scoreSurface->w;
            scoreHeight = scoreSurface->h;
            SDL_FreeSurface(scoreSurface);
            scoreRect.x = 10;
            scoreRect.y = 5;
            scoreRect.w = scoreWidth;
            scoreRect.h = scoreHeight;

            if (SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect) != 0)
            {
                fprintf(stderr, "error SDL_RenderCopy : %s\n", SDL_GetError());
                goto Quit;
            }

            SDL_RenderPresent(renderer);
        }

        jump = SDL_FALSE;

        if (!go)  // if in display mode (ai >= 0), will wait for the user to press SPACE
        {
            if (ai < 0)
            {
                go = SDL_TRUE;
                jump = SDL_TRUE;
            }
            else
            {
                printf("waiting for user to press " CYAN "SPACE" RESET " ...");
                fflush(stdout);
                while (!go)
                {
                    SDL_Delay(20);
                    while (SDL_PollEvent(&event))
                    {
                        if (event.type == SDL_QUIT)
                        {
                            go = SDL_TRUE;
                            quit = SDL_TRUE;
                        }
                        else if (event.type == SDL_KEYDOWN)
                        {
                            if (event.key.keysym.sym == SDLK_q)  // safety quit
                            {
                                statut = -1;
                                printf(" manual interruption of level\n");
                                goto Quit;
                            }
                            else if (event.key.keysym.sym == SDLK_SPACE)
                            {
                                printf("\33[3D\33[K(done)\n");  // changes "... " to "(done)"
                                go = SDL_TRUE;
                                jump = SDL_TRUE;
                            }
                        }
                    }
                }
            }
            old_state.jump = 1;
        }

        if (!hide)
            SDL_Delay(framePeriod);
    }

    if (ai && dead(&flappyRect, pillarRects) && score + 1 != levelSize)
    {
        actualise_state(&new_state);
        q_value = q[old_state.xRow][old_state.yPRow][old_state.dyRow][old_state.jump];
        q[old_state.xRow][old_state.yPRow][old_state.dyRow][old_state.jump] = q_value + learning_rate * (deathMalus + return_rate * max_gain(new_state) - q_value);
    }
    if (ai >= 0)
    {
        printf("Score : %d / %d\n", score, levelSize - 1);
        if (score == levelSize - 1)
        {
            printf("Victory for flappy!\n");
        }
    }

    statut = EXIT_SUCCESS;

Quit:
    if (scoreTexture != NULL)
        SDL_DestroyTexture(scoreTexture);
    if (myFont != NULL)
        TTF_CloseFont(myFont);
    if (renderer != NULL)
        SDL_DestroyRenderer(renderer);
    if (window != NULL)
        SDL_DestroyWindow(window);
    if (!hide)
        TTF_Quit();
    SDL_Quit();

    return statut;
}
