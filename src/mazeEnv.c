
#include "mazeEnv.h"
#include "functions.h"


// must be called to initialize /mazeEnv/ (already used in /mazeEnv_make/)
void alloc_mazeEnv()
{
    mazeEnv = malloc(rows * sizeof(char *));

    for (int i = 0; i < rows; i++)
    {
        mazeEnv[i] = malloc(cols * sizeof(char *));
    }
}

// makes the maze /mazeEnv/ from a textfile
void mazeEnv_make(char *file_name)
{
    // #region : reads the number of rows and columns and stores it in /rows/ and /cols/
    char c;
    char rows_s[3] = {'\0'}; // will store the number of rows (3 or less digit number)
    char cols_s[3] = {'\0'}; // will store the number of columns (3 or less digit number)
    int rows_i = 0;
    int cols_i = 0;
    int swap = 0;

    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("Pointeur nul");
    }

    if (file)
    {
        /* lire la premiere ligne avant EOF */
        while ((c = getc(file)) != EOF)
        {
            if (c == '\n')
            {
                break;
            }
            else if (c == ',')
            {
                swap = 1;
            }
            else if (!swap)
            {
                rows_s[rows_i] = c;
                rows_i++;
            }
            else
            {
                cols_s[cols_i] = c;
                cols_i++;
            }
        }
    }

    /* convertir le string en nombre */
    rows = atoi(rows_s);
    cols = atoi(cols_s);
    // #endregion

    alloc_mazeEnv(); // intinializes the maze

    // #region : stores the maze in /mazeEnv/
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            c = getc(file);

            if (c == '\n')
            {
                c = getc(file);
            }
            else if (c == 's')
            {
                start_row = i;
                start_col = j;
            }
            else if (c == 'g')
            {
                goal_row = i;
                goal_col = j;
            }

            mazeEnv[i][j] = c;
        }
    }
    // #endregion

    fclose(file);
}

// display the maze
void mazeEnv_render()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%c ", mazeEnv[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// display the maze with an 'o' on current position
void mazeEnv_render_pos()
{
    mazeEnv[state_row][state_col] = 'k';
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%c ", mazeEnv[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// resets current position to the starting position
void mazeEnv_reset()
{
    state_row = start_row;
    state_col = start_col;
}

// faire une action &observer récompense et où on se trouve
envOutput mazeEnv_step(action a)
{
    int reward = 0;
    int done = 0;
    envOutput stepOut;  // cf /mazeEnv.h/
    mazeEnv[state_row][state_col] = 'o';
    if (a == up)
    {
        //state_row = max(0, state_row - 1);  // won't go up if it can't anymore
        if (mazeEnv[state_row - 1][state_col] != '+' && state_row - 1 >= 0)
        {
            state_row -= 1;
        }
        reward = -1; //added
    }
    else if (a == down)
    {
        //state_row = min(rows, state_row + 1);  // won't go down if it can't anymore
        if (mazeEnv[state_row + 1][state_col] != '+' && state_row + 1 < rows)
        {
            state_row += 1;
        }
        reward = -1;  // added 
    }
    else if (a == right)
    {
        //state_col = min(cols, state_col + 1);  // won't go right if it can't anymore
        if (mazeEnv[state_row][state_col + 1] != '+' && state_col + 1 < cols)
        {
            state_col += 1;
        }
        reward = -1; //added
    }
    else if (a == left)
    {
        //state_col = max(0, state_col - 1);  // won't go left if it can't anymore
        if (mazeEnv[state_row][state_col - 1] != '+' && state_col - 1 >= 0)
        {
            state_col -= 1;
        }
        reward = -1; //added
    }

    if ((state_row == goal_row) && (state_col == goal_col))  // if current position is goal position
    {
        reward = 10000;
        done = 1;
    }

    stepOut.reward = reward;  // reward of the action
    stepOut.done = done;  // done only if on the goal (to use later if other cases can end the explorer)
    stepOut.new_col = state_col;  // on which column the explorer ends after this action
    stepOut.new_row = state_row;  // on which row the explorer ends after this action
    
    

    return stepOut;
}

// returns a random action among up, down, left and right
action env_action_sample()
{
    return (enum action)(rand() % number_actions);
}

// must be called to initialize /visited/ (already used in /init_visited/)
void alloc_visited()
{
    visited = malloc(rows * sizeof(int *));
    int i;
    for (i = 0; i < rows; ++i)
    {
        visited[i] = malloc(cols * sizeof(int *));
    }
}

// initializes /visited/ with the walls and the goal (omniscient towards those)
void init_visited()
{
    alloc_visited();

    int i, j;
    for (i = 0; i < rows; ++i)
    {
        for (j = 0; j < cols; ++j)
        {
            if (mazeEnv[i][j] == '+')
            {
                visited[i][j] = wall;
            }
            else if (mazeEnv[i][j] == 'g')
            {
                visited[i][j] = goal;
            }
            else
            {
                visited[i][j] = unknown;
            }
        }
    }
}

// adds '.' on /mazeEnv/ where they are crumbs on /visited/
void add_crumbs()
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (visited[i][j] == crumb)
            {
                mazeEnv[i][j] = '.';
            }
        }
    }
    mazeEnv[start_row][start_col] = 's';
}


void destroy_maze(){
    for (int i = 0; i < rows; i++)
    {
        free(mazeEnv[i]);
    }
    free (mazeEnv);

    // for (int i = 0; i < rows; ++i)
    // {
    //     free( visited[i]);
    // }
    // free (visited);
}