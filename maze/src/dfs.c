#include "dfs.h"

/*----------------------------------------------------------------------------------------------------*/

// Depth First Search :
// - if it find a way to the goal : adds crumbs on /visited/ positions that leads to the goal and returns 1
// - otherwise (no path found to the goal) : returns 0
int dfs(int row, int col){
    int* current = &visited[row][col];

    if(*current == goal){
        return 1;
    }else if(*current == unknown){
        *current = known;

        if (dfs(row, col-1)){
            *current = crumb;
            return 1;
        } else if (dfs(row+1, col)){
            *current = crumb;
            return 1;
        }  else if (dfs(row, col+1)){
            *current = crumb;
            return 1;
        }  else if (dfs(row-1, col)){
            *current = crumb;
            return 1;
        }

    }

    return 0;
}


// // here is the main, keep that in mind when compiling
// int main(){
//    mazeEnv_make("../data/maze.txt");
//    init_visited();

//    printf("%d, %d \n", rows, cols);
//    printf("number of actions :  %d \n", number_actions);  // should return 4 since they are 4 actions : up, down, left, right
//    mazeEnv_render_pos();  // displays the maze at start (once loaded from the txt file)
//    dfs(start_row,start_col);
//    add_crumbs();
//    mazeEnv_render();  // displays the maze once /dfs/ looked for a path to the goal
//    return 0;
// }

