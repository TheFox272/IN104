
#include "qlearning.h"
#include "mazeEnv.h"
#include <colored_output.h>


double learning_rate = 0.7;
int number_episode = 1000000;
double return_rate = 0.999;
int horizon = 100;
float epsilon = 0.5;


void init_q()
{
    q = malloc(rows * cols * sizeof(double *));
    for (int i = 0; i < rows * cols; i++)
    {
        q[i] = malloc(number_actions * sizeof(double));
        for (int j = 0; j<number_actions; j++){
            q[i][j]= 0;
        }
    }
    
}

//transform each actions into a specific int 
int actions_to_int(action a){
    if (a == up)
    {
        return 0;
    }
    else if (a == down)
    {
        return 1;
    }
    else if (a == right)
    {
        return 2;
    }
    else 
    {
        return 3;
    }
}

//Do the opposite transformation 
action int_to_action(int i){
    if (i==0){
        return up;
    }
    else if (i ==1){
        return down;
    }
    else if (i==2){
        return right;
    }
    else {
        return left;
    }
}


//Find the maximal possible outcome for a specific position

double max_actions(int s){
    double max = q[s][0];
    for (int j=1; j<number_actions; j++){
        if (q[s][j]>max){
            max = q[s][j];
        }
    }
    return max;
}

// Find the action that maximise the outcome for a specific position

int best_action(int s){
    double max = q[s][0];
    int id_max = 0;
    for (int j=1; j<number_actions; j++){
        if (q[s][j]>max){
            max = q[s][j];
            id_max = j;
        }
    }
    return id_max;
}

void epsilon_greedy(){
    for (int i = 0 ; i < number_episode ; i++){

        printf("\r%.5f %%", 100 * (float)i/number_episode);
        fflush(stdout);
        // printf("episode : %d \n", i);

        int s = start_col + start_row*cols; //Initialization of the state
        int n = 0; //Number of actions performed in the episode
        int done = 0;

        //Initialization of the variables 
        int next_action;
        float tirage;
        struct envOutput EnvOut;
        int next_s = s;
        srand(time(NULL));


        //Pick an action unless the goal is reached 
        while (n<horizon && done == 0){

            //Pick an action according to the epsilon greedy algorithm

            tirage = rand() / (RAND_MAX + 1.0);
            
            if (tirage < epsilon){
                next_action = actions_to_int(env_action_sample());
            }
            else {
                next_action = best_action(s);
            }

            // printf("action : %d \n", n);
            // printf("%d \n", next_action);

            //Updating Q with the new state
        
            EnvOut = mazeEnv_step(int_to_action(next_action));
            next_s = EnvOut.new_col + EnvOut.new_row*cols;
            done = EnvOut.done;

            // mazeEnv_render_pos();
            

            q[s][next_action] = q[s][next_action] + learning_rate*(EnvOut.reward + return_rate*max_actions(next_s) - q[s][next_action]);
            s = next_s;
            n++;
        }
        
    }

}


void visualise (){
    printf("\n");
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (mazeEnv[i][j] == 'o' || mazeEnv[i][j] == 'k')
            {
                if (max_actions(j + i*cols) > 0)
                {
                    green();
                }
                else
                {
                    red();
                }
                switch (best_action(j + i*cols))
                {
                case 0:
                    printf("^ ");
                    break;
                case 1:
                    printf("v ");
                    break;
                case 2:
                    printf("> ");
                    break;
                case 3:
                    printf("< ");
                    break;
                }
            }
            else
            {
                blue();
                printf("%c ", mazeEnv[i][j]);
            }
            reset();
        }
        printf("\n");
    }
}

void print_q (){
    for (int i = 0; i < rows * cols; i++){
        printf("%d : up %lf down %lf right %lf left %lf \n", i, q[i][0], q[i][1], q[i][2], q[i][3]);
    }
}

void destroy_q(){
    for (int i = 0; i < rows * cols; i++)
    {
        free( q[i] );
    }
    free (q);
}





















