#include "main.h"

/*----------------------------------------------------------------------------------------------------*/
#define RED "\033[1;31m"
#define RESET "\033[0m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
/*----------------------------------------------------------------------------------------------------*/

int ls(char * path)
{
    DIR * dp = opendir(path);
    if(!dp) 
    {
        printf("error while reading ../data directory");
        return -1;
    }
    struct dirent * ep;
    printf("%s :\n", path);
    while((ep = readdir(dp)))
        if(strncmp(ep->d_name, ".", 1))
            printf(GREEN "\t%s\n" RESET, ep->d_name);
    closedir(dp);
    return 0;
}


int main(int argc, char** argv)
{
    old_nb_episode = 0;
    if (argc == 1)
    {
        printf("error: please give an argument to main\n");
        printf("-> " YELLOW "main g args" RESET " to generate a level\n");
        printf("-> " YELLOW "main p args" RESET " to play manually\n");
        printf("-> " YELLOW "main c args" RESET " to create an AI\n");
        printf("-> " YELLOW "main t args" RESET " to train an existing AI\n");
        printf("-> " YELLOW "main l args" RESET " to load an AI and show it\n");
        return -1;
    }
    else if (strcmp(argv[1], "g") == 0)  // g for generate
    {
        if (argc >= 4)
        {
            init(0);
            char levelName[40] = "../data/levels/";
            strcat(levelName, argv[2]);
            strcat(levelName, ".txt");
            if (access(levelName, F_OK) == 0)
            {
                printf("error: cannot overwrite " GREEN "%s" RESET " in g mode (do it manually)\n", argv[2]);
                return -1;
            }
            if (generate_level(argv[2], atoi(argv[3])) == 0)
                printf("Level " GREEN "%s" RESET " generated successfully\n", argv[2]);
            else
                return -1;
        }
        else
        {
            printf("error: too few arguments to generate a level\n");
            printf("Use " YELLOW "main g levelName size" RESET " where :\n");
            printf("-> " YELLOW "levelName" RESET " is the name of the level to generate (must not already be existing)\n");
            printf("-> " YELLOW "size" RESET " is the size of the level to generate\n");
            return -1;
        }
    }
    else if (strcmp(argv[1], "p") == 0)  // p for play
    {
        if (argc >= 3)
        {
            init(0);
            if (load_level(argv[2]) != 0)
                return -1;
            play(0);
            close_level();
        }   
        else
        {
            printf("error: too few arguments to play manually\n");
            printf("Use " YELLOW "main p levelName" RESET " where :\n");
            printf("-> " YELLOW "levelName" RESET " is the name of the level to play (see list below).");
            printf(" If they is no existing level, maybe use " YELLOW "main g" RESET " first\n");
            ls("../data/levels");
            return -1;
        }
    }
    else if (strcmp(argv[1], "c") == 0 || strcmp(argv[1], "t") == 0)  // c for create ai, t for train ai
    {
        int t_mode = strcmp(argv[1], "t") == 0;
        if (argc >= 6)
        {
            init(1);
            if (load_level(argv[3]) != 0)
                return -1;
            if (init_q() == 0)
                printf("successfully initiated q\n");
            else
                return -1;
            char aiName[40] = "../data/q/";
            strcat(aiName, argv[2]);
            strcat(aiName, ".txt");
            if (t_mode)
            {
                if (load_q(argv[2]) != 0)
                {
                    printf("error while loading q\n");
                    goto Abort_c;
                }
            }
            else if (access(aiName, F_OK) == 0)
            {
                printf("error: cannot overwrite " GREEN "%s" RESET " in c mode (do it manually)\n", argv[2]);
                goto Abort_c;
            }
            if (atoi(argv[4]) != 1 && atoi(argv[4]) != 2)
            {
                printf("error: AI model " YELLOW "%s" RESET " does not exist\n", argv[4]);
                goto Abort_c;
            }
            nb_episode = atoi(argv[5]);
            if ((argc >= 7 && strcmp(argv[6], "hh") == 0) || nb_episode > 1000)  // hh for hard hide
            {
                hide = 2;
                if ((argc < 7 || strcmp(argv[6], "hh") != 0) && nb_episode > 1000)
                    printf("warning: " YELLOW "nb_episode" RESET " is too high to show the training -> " YELLOW "(hide)" RESET " set to " YELLOW "hh" RESET "\n");
            }
            else if (argc >= 7 && strcmp(argv[6], "h") == 0)  // h for hide
                hide = 1;
            else
            {
                hide = 0;
                printf("showing AI training ... (press " CYAN "Q" RESET " frantically to abort)\n");
            }
            trainAI(atoi(argv[4]), argv[2]);
Abort_c:
            destroy_q();
            close_level();
        }
        else
        {
            if (t_mode)
            {
                printf("error: too few arguments to train an AI\n");
                printf("Use " YELLOW "main t aiName levelName aiModel nb_episode (hide)" RESET " where :\n");
                printf("-> " YELLOW "aiName" RESET " is the name of the AI to train (must already be existing, see list below).");
                printf(" If they is no existing AI, maybe use " YELLOW "main c" RESET " first\n");
                ls("../data/q");
            }
            else
            {
                printf("error: too few arguments to create an AI\n");
                printf("Use " YELLOW "main c aiName levelName aiModel nb_episode (hide)" RESET " where :\n");
                printf("-> " YELLOW "aiName" RESET " is the name of the ai to create (must not already be existing)\n");
            }
            printf("-> " YELLOW "levelName" RESET " is the name of the level that the AI will train on (see list below).");
            printf(" If they is no existing level, maybe use " YELLOW "main g" RESET " first\n");
            ls("../data/levels");
            printf("-> " YELLOW "aiModel" RESET " is the model that the trainer will use (1 for epsilon_greedy, 2 for boltzmann_exploration)\n");
            printf("-> " YELLOW "nb_episode" RESET " is the number of episodes that the AI will train on\n");
            printf("-> " YELLOW "(hide)" RESET " is an optional parameter.");
            printf(" Set it to " YELLOW "h" RESET " to hide the training and show the result.");
            printf(" Set it to " YELLOW "hh" RESET " to hide the training (useful to run it in the background)\n");
            return -1;
        }
    }
    else if (strcmp(argv[1], "l") == 0)  // l for load AI
    {
        if (argc >= 4)
        {
            init(0);
            if (load_level(argv[3]) != 0)
                return -1;
            if (init_q() == 0)
                printf("successfully initiated q\n");
            else
                return -1;
            if (load_q(argv[2]) != 0)
            {
                printf("error while loading q\n");
                goto Abort_l;
            }
            if (argc >= 5)
            {
                if (strcmp(argv[4], "s") == 0)
                    hide = -1;
                else if (strcmp(argv[4], "ss") == 0)
                    hide = -2;
            }
            play(1);
Abort_l:
            destroy_q();
            close_level();
        }
        else
        {
            printf("error: too few arguments to load and show an AI\n");
            printf("Use " YELLOW "main l aiName levelName" RESET " where :\n");
            printf("-> " YELLOW "aiName" RESET " is the name of the AI to load (see list below).");
            printf(" If they is no existing AI, maybe use " YELLOW "main c" RESET " first\n");
            ls("../data/q");
            printf("-> " YELLOW "levelName" RESET " is the name of the level that the AI will play on (see list below).");
            printf(" If they is no existing level, maybe use " YELLOW "main g" RESET " first\n");
            ls("../data/levels");
            printf("-> " YELLOW "(show)" RESET " is an optional parameter.");
            printf(" Set it to " YELLOW "s" RESET " to show the marks of prefered jumping spots.");
            printf(" Set it to " YELLOW "ss" RESET " do the previous thing and slow down time\n");
            return -1;
        }
    }
    else
        printf("... nothing happened. You can use " YELLOW "./main" RESET " to see the available options\n");
    return 0;
}



