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
        ai_speed = 1;
    }
    else
        ai_speed = 10;  // can't be more than framePeriod
    
    epsilon_max = 0.2;
    learning_rate_max = 1.0;  // decay per 100 episodes
    return_rate = 0.99;  // cf gamma in the lesson
    ai_cycle = 15;
    gateBonus = 16;
    deathMalus = -8;
    xTile = 25;  // size of a tile on the xAxis
    yTile = 10;  // size of a tile on the yAxis
    // #endregion

    stuckTime = 0;

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
    dx = 2;  // scrolling speed
    maxScore = 0;
    framePeriod = (int)(10 / ai_speed) + 1;  // in ms
    // #endregion

    init_pos(ai);

    return 0;
}


int init_pos(int ai)
{
    x = pWidth + flappySize / 2; // x position relative to previous pillar
    y = height / 2;
    dy = 0;
    score = 0;  // number of pillars passed
    start = 0;
    if (ai)
    {
        epsilon = 100 / (1000 + 4000 * maxScore);  // used by the epsilon greedy method
        learning_rate = 0.4;  // the speed at which the weights of q will be changed (cf alpha in the lesson)
    }
    else
    {
        epsilon = 0;
        learning_rate = 0;
    }
    
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
            if (score >= maxScore)
                maxScore = score;
            yP1 = pillarsY[score];
            if (score + 1 < levelSize)
                yP2 = pillarsY[score + 1];
            else
                yP2 = -1;
        }
    }
    else
    {
        start = 1;
        yP1 = pillarsY[score];
        yP2 = pillarsY[score + 1];
    }
    
    return 0;
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


void actualise_state(state *s, double x, double y)
{
    int xRow = (int)(x / xTile);
    if (xRow < 0)
        xRow = 0;
    else if (xRow > (int)(width / xTile))
        xRow = (int)(width / xTile);
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
    int dyRow = (int)(dy / 1);
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
    int status = 0;
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
            status = -2;
            goto Close;
        }
    }
    if (fgets(temp, 10, f))
    {
        printf("error reading the file " GREEN "%s" RESET " : too long for indicated size\n", filename);
        free(pillarsY);
        status = -2;
        goto Close;
    }
    else
        printf("successfully loaded the level from the file " GREEN "%s" RESET "\n", filename);
Close:
    fclose(f);
    return status;
}


void close_level()
{
    free(pillarsY);
}


int play(int ai)  // ai = 0 for manual, -1 for epsilon greedy without display, 1 for epsilon greedy with display
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    int statut = EXIT_FAILURE;

    // #region : will be used for display
    SDL_Color orange = {255, 127, 40, 255};
    SDL_Color green = {28, 150, 28, 255};
    SDL_Color sky = {206, 237, 247, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color magenta = {255, 0, 255, 255};
    SDL_Color blue = {0, 0, 255, 255};
    SDL_Rect pillarRects[4];
    SDL_Rect flappyRect;
    flappyRect.w = flappySize;
    flappyRect.h = flappySize;
    TTF_Font *scoreFont = NULL;
    TTF_Font *markFont = NULL;
    SDL_Surface *markSurface = NULL;
    SDL_Texture *scoreTexture = NULL;
    SDL_Texture *markTexture = NULL;
    SDL_Rect scoreRect;
    SDL_Rect markRect;
    char scoreText[5];
    char markText[5] = "^";
    // #endregion

    SDL_Event event;
    SDL_bool jump = SDL_FALSE;
    SDL_bool quit = SDL_FALSE;
    int wait = 1;

    // #region : SDL init stuff
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "error SDL_Init : %s\n", SDL_GetError());
        goto Quit;
    }
    if (hide <= 0)
    {
        if (TTF_Init() != 0)
        {
            fprintf(stderr, "error TTF_Init : %s\n", SDL_GetError());
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

        // #region : TTF stuff
        scoreFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf", 30);
        if (scoreFont == NULL)
        {
            fprintf(stderr, "error TTF_OpenFont : %s\n", SDL_GetError());
            goto Quit;
        }
        markFont = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSerif-Bold.ttf", 30);
        if (markFont == NULL)
        {
            fprintf(stderr, "error TTF_OpenFont : %s\n", SDL_GetError());
            goto Quit;
        }
        // #endregion

    }
    // #endregion

    // #region : AI parameters initialisation
    double cycle = 0;
    double draw;
    srand(time(NULL));
    double e0;
    double e1;
    state old_state;
    state new_state;
    int old_score = score;
    double q_value;
    if (ai)
        actualise_state(&old_state, x, y);
        
    // #endregion

    while (wait || (!dead(&flappyRect, pillarRects) && !quit))
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
                else if (event.key.keysym.sym == SDLK_p)  // pause
                    wait = 2;
                else if (event.key.keysym.sym == SDLK_SPACE && !ai)
                    jump = SDL_TRUE;
            }
        }

        if (ai)
        {
            cycle += fmax(1 * dx / xTile, abs(dy) / yTile) * (ai_cycle - 9);
            if (cycle >= ai_cycle && dy < 1)
            {
                cycle = 0;
                actualise_state(&new_state, x, y);
                q_value = q[old_state.xRow][old_state.yPRow][old_state.dyRow][old_state.jump];
                q[old_state.xRow][old_state.yPRow][old_state.dyRow][old_state.jump] = q_value + learning_rate * ((score - old_score) * gateBonus + return_rate * max_gain(new_state) - q_value);
                old_state = new_state;
                old_score = score;

                if (abs(ai) == 1)  // epsilon_greedy
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
                else if (abs(ai) == 2)  // boltzmann_exploration
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
        }

        next(jump);
        if (ai && epsilon != epsilon_max && score >= maxScore)
        {
            epsilon = epsilon_max;
            learning_rate = learning_rate_max;
            // printf("%d\n", maxScore);
        }

        if (hide <= 0)
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
        if (hide <= 0)
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
        flappyRect.y = height - y - flappySize / 2;
        if (hide <= 0)
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
            SDL_Surface *scoreSurface = TTF_RenderText_Solid(scoreFont, scoreText, black);
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

            scoreRect.x = 10;
            scoreRect.y = 5;
            scoreRect.w = scoreSurface->w;
            scoreRect.h = scoreSurface->h;
            SDL_FreeSurface(scoreSurface);

            if (SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect) != 0)
            {
                fprintf(stderr, "error SDL_RenderCopy : %s\n", SDL_GetError());
                goto Quit;
            }

            if (hide < 0)
            {
                state virtual_state;
                for (int i = 0; i < width; i += xTile)
                {
                    for (int j = 0; j < height; j += yTile)
                    {
                        actualise_state(&virtual_state, i, j);
                        double sum_gain = 0;
                        SDL_bool jumpy = SDL_FALSE;
                        for (int k = 0 ; k < dyRows ; k++)
                        {
                            double a = q[virtual_state.xRow][virtual_state.yPRow][k][1];
                            double b = q[virtual_state.xRow][virtual_state.yPRow][k][0];
                            double jump_gain = a - b;
                            if (jump_gain > 0)
                            {
                                sum_gain += jump_gain * pow(dyMin + k - 1, 3);
                                if (!jumpy)
                                    jumpy = SDL_TRUE;
                            }
                        }

                        if (jumpy)
                        {
                            if (abs(sum_gain) < gateBonus * 1.5)
                                markSurface = TTF_RenderText_Solid(markFont, markText, magenta);
                            else if (sum_gain > 0)
                                markSurface = TTF_RenderText_Solid(markFont, markText, red);
                            else
                                markSurface = TTF_RenderText_Solid(markFont, markText, blue);
                            if (markSurface == NULL)
                            {
                                fprintf(stderr, "error TTF_RenderText_Solid : %s\n", SDL_GetError());
                                goto Quit;
                            }
                            markTexture = SDL_CreateTextureFromSurface(renderer, markSurface);
                            if (markTexture == NULL)
                            {
                                fprintf(stderr, "error SDL_CreateTextureFromSurface : %s\n", SDL_GetError());
                                goto Quit;
                            }
                            markRect.w = markSurface->w;
                            markRect.h = markSurface->h;
                            markRect.x = xOrigin + i - x;
                            markRect.y = height - j;
                            if (SDL_RenderCopy(renderer, markTexture, NULL, &markRect) != 0)
                            {
                                fprintf(stderr, "error SDL_RenderCopy : %s\n", SDL_GetError());
                                goto Quit;
                            }
                        }
                    }
                }
                if (hide == -2)
                    SDL_Delay(4 * framePeriod);
            }

            SDL_RenderPresent(renderer);
        }

        jump = SDL_FALSE;

        if (wait)  // if in display mode (ai >= 0), will wait for the user to press SPACE
        {
            if (ai < 0 && wait == 1)  // wait == 1 as safety, shouldn't happen
            {
                wait = 0;
                jump = SDL_TRUE;
            }
            else
            {
                if (wait == 1)
                    printf("waiting for user to press " CYAN "SPACE" RESET " ...");
                else
                    printf("paused, waiting for user to press " CYAN "p" RESET " ...");
                fflush(stdout);
                while (wait)
                {
                    SDL_Delay(20);
                    while (SDL_PollEvent(&event))
                    {
                        if (event.type == SDL_QUIT)
                        {
                            wait = 0;
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
                            else if (event.key.keysym.sym == SDLK_p && wait == 2)  // unpause
                            {
                                printf("\33[40D\33[K");  // erase the pause line
                                fflush(stdout);
                                wait = 0;
                            }
                            else if (event.key.keysym.sym == SDLK_SPACE)
                            {
                                printf("\33[3D\33[K(done)\n");  // changes "... " to "(done)"
                                wait = 0;
                                jump = SDL_TRUE;
                            }
                        }
                    }
                }
            }
            old_state.jump = 1;
        }

        if (hide <= 0)
            SDL_Delay(framePeriod);
    }

    if (ai && dead(&flappyRect, pillarRects) && score + 1 != levelSize)
    {
        actualise_state(&new_state, x, y);
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
    // printf(" %d %d\n", score, maxScore);
    if (score >= maxScore)
    {
        maxScore = score;
        stuckTime = 0;
    }
    else
    {
        stuckTime++;
        if (stuckTime > 80)
        {
            maxScore = score;
            stuckTime = 0;
        }
    }
        

    statut = EXIT_SUCCESS;

Quit:
    if (markSurface != NULL)
        SDL_FreeSurface(markSurface);
    if (scoreTexture != NULL)
        SDL_DestroyTexture(scoreTexture);
    if (scoreFont != NULL)
        TTF_CloseFont(scoreFont);
    if (renderer != NULL)
        SDL_DestroyRenderer(renderer);
    if (window != NULL)
        SDL_DestroyWindow(window);
    if (hide <= 0)
        TTF_Quit();
    SDL_Quit();

    return statut;
}
