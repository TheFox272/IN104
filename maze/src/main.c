#include "main.h"

/*----------------------------------------------------------------------------------------------------*/

int main()
{
    mazeEnv_make("../data/maze.txt");
    mazeEnv_reset();
    // init_visited();

    printf("%d, %d \n", rows, cols);
    printf("number of actions :  %d \n", number_actions);  // should return 4 since they are 4 actions : up, down, left, right
    mazeEnv_render_pos();  // displays the maze at start (once loaded from the txt file)
    init_q();
    sarsa();
    // print_q();
    visualise();
    // dfs(start_row,start_col);
    // add_crumbs();
    // mazeEnv_render(); // displays the maze once /dfs/ looked for a path to the goal
    destroy_maze();
    destroy_q();
    return 0;

}

















