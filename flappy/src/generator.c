#include "generator.h"

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
        printf("error creating the file %s\n", filename);
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




