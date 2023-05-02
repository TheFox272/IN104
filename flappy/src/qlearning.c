#include "qlearning.h"

/*----------------------------------------------------------------------------------------------------*/
#define RED "\033[1;31m"
#define RESET "\033[0m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
/*----------------------------------------------------------------------------------------------------*/

int init_q()
{
    xRows = (int)(width / xTile) + 1;
    yPRows = (int)((2 * height - 4 * pSpace) / yTile) + 1;
    dyMin = -18 / 2;
    dyMax = 7 / 2;
    dyRows = dyMax - dyMin + 2;
    nb_actions = 2;

    if (xRows * yPRows * dyRows * nb_actions > 15000000)  // more than that will produce a SEGMENTATION FAULT error
    {
        printf("error: q is too big (please adjust parameters to reduce it)\n");
        return 1;
    }
    else
    {
        // q[x][yP1][yP2][dy][action]

        q = malloc(xRows * sizeof(double ***));
        for (int a = 0 ; a < xRows ; a++)
        {
            q[a] = malloc(yPRows * sizeof(double **));
            for (int b = 0 ; b < yPRows ; b++)
            {
                q[a][b] = malloc(dyRows * sizeof(double *));
                for (int c = 0 ; c < dyRows ; c++)
                {
                    q[a][b][c] = malloc(nb_actions * sizeof(double));
                    q[a][b][c][0] = 0;
                    q[a][b][c][1] = 0;
                }
            }
        }
    }
    return 0;
}


void destroy_q()
{
    for (int a = 0 ; a < xRows ; a++)
    {
        for (int b = 0 ; b < yPRows ; b++)
        {
            for (int c = 0 ; c < dyRows ; c++)
            {
                free(q[a][b][c]);
            }
            free(q[a][b]);
        }
        free(q[a]);
    }
    free(q);
}


int trainAI(int aiModel, char aiName[26])  // aiModel = 1 for epsilon_greedy, 2 for boltzmann_exploration
{
    int statut;
    episode_done = 0;
    for (int i = 0 ; i < nb_episode ; i++)
    {
        printf("\r%.5f %%", 100 * (float)i/nb_episode);
        fflush(stdout);
        init(1);
        statut = play(-aiModel);
        if (statut != 0)
        {
            printf("error while training AI\n");
            return statut;
        }
        episode_done++;
        if ((i+1) % 5000 == 0)  // safety save every 5k iteration (useful to not lose a night's work)
        {
            printf("\n");
            if (save_q(aiName) == 0)
                printf(RED "safety:" RESET " q successfully saved as " YELLOW "%s" RESET "\n", aiName);
        }
    }
    printf("\n");
    if (save_q(aiName) == 0)
        printf("q successfully saved as " YELLOW "%s" RESET "\n", aiName);
    if (hide <= 1)
    {
        init(0);
        statut = play(aiModel);
        if (statut != 0)
        {
            printf("error while showing AI\n");
            return statut;
        }
    }
    printf("successfully trained AI " YELLOW "%s" RESET " based on model " YELLOW "%d" RESET, aiName, aiModel);
    printf(" (it is now " GREEN "%d" RESET " episodes old)\n", old_nb_episode + episode_done);
    return 0;
}


int save_q(char aiName[26])
{
    char filename[40] = "../data/q/";
    strcat(filename, aiName);
    strcat(filename, ".txt");
    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        printf("error creating the file " YELLOW "%s" RESET "\n", filename);
        return -1;
    }

    fprintf(f, "%d\n", old_nb_episode + episode_done);
    fprintf(f, "%d\n", xRows);
    fprintf(f, "%d\n", yPRows);
    fprintf(f, "%d\n", dyRows);
    for (int a = 0 ; a < xRows ; a++)
    {
        for (int b = 0 ; b < yPRows ; b++)
        {
            for (int c = 0 ; c < dyRows ; c++)
            {
                for (int d = 0 ; d < nb_actions ; d++)
                {
                    fprintf(f, "%f\n", q[a][b][c][d]);
                }
            }
        }
    }

    fclose(f);

    return 0;
}


int load_q(char aiName[26])
{
    int status = 0;
    char filename[40] = "../data/q/";
    strcat(filename, aiName);
    strcat(filename, ".txt");
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        printf("error reading the file " YELLOW "%s" RESET "\n", filename);
        return -1;
    }
    char temp[20];
    fgets(temp, 10, f);
    old_nb_episode = atoi(temp);
    fgets(temp, 10, f);
    if (xRows != atoi(temp))
    {
        printf("error: AI " YELLOW "%s" RESET " is using a different " RED "xRows" RESET " number", aiName);
        status = -2;
        goto Close;
    }
    fgets(temp, 10, f);
    if (yPRows != atoi(temp))
    {
        printf("error: AI " YELLOW "%s" RESET " is using a different " RED "yPRows" RESET " number", aiName);
        status = -2;
        goto Close;
    }
    fgets(temp, 10, f);
    if (dyRows != atoi(temp))
    {
        printf("error: AI " YELLOW "%s" RESET " is using a different " RED "dyRows" RESET " number", aiName);
        status = -2;
        goto Close;
    }
    
    for (int a = 0 ; a < xRows ; a++)
    {
        for (int b = 0 ; b < yPRows ; b++)
        {
            for (int c = 0 ; c < dyRows ; c++)
            {
                for (int d = 0 ; d < nb_actions ; d++)
                {
                    if (fgets(temp, 20, f))
                        q[a][b][c][d] = atof(temp);
                    else
                    {
                        printf("error reading the file " YELLOW "%s" RESET " : too short for indicated size\n", filename);
                        status = -1;
                        goto Close;
                    }
                }
            }
        }
    }
    if (fgets(temp, 10, f))
    {
        printf("error reading the file " YELLOW "%s" RESET " : too long for indicated size\n", filename);
        status = -1;
        goto Close;
    }
    else
        printf("successfully loaded q from the file " YELLOW "%s" RESET "\n", filename);

Close:
    fclose(f);
    if (status == -2)
        printf(" (please readjust current parameters to match the AI's, or consider it obsolete)\n");
    return status;
}




