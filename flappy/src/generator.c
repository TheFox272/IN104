#include "generator.h"

/*----------------------------------------------------------------------------------------------------*/
#define RESET "\033[0m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
/*----------------------------------------------------------------------------------------------------*/

int generate_level(char name[21], int size)
{
    if (size < 2)
    {
        printf("error: invalid size for a level (must be an int over 1)\n");
        return -1;
    }

    char filename[40] = "../data/levels/";
    strcat(filename, name);
    strcat(filename, ".txt");
    FILE *f = fopen(filename, "w");
    if (f == NULL)
    {
        printf("error creating the file " GREEN "%s" RESET "\n", filename);
        return -1;
    }

    fprintf(f, "%d\n", size + 1);
    fprintf(f, "%d\n", (int)(height/2 - pSpace/2));
    for (int i = 1 ; i < size+1 ; i++)
    {
        fprintf(f, "%d\n", (int)rand() % (int)(height - pSpace));
    }

    fclose(f);

    return 0;
}




