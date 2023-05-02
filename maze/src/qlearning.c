#include "qlearning.h"

/*----------------------------------------------------------------------------------------------------*/
// CONSTANTS INITIALIZATION

double learning_rate = 0.6; // the speed at which the weights of q will be changed (cf alpha in the lesson)
int number_episode = 1000; // the number of episodes that will occur to balance q
double return_rate = 1.0;  // cf gamma in the lesson
int horizon = 10000;  // the maximum number of moves the explorer will perform each episode
float epsilon; // used by the epsilon greedy method

/*----------------------------------------------------------------------------------------------------*/

// must be called to initialize /q/
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

// takes an action as an input, and returns the corresponding int
int action_to_int(action a){
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

// takes an int as an input, and returns the corresponding action
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


// find the maximal gain among all actions for the state given as input
double max_actions(int s){
    double max = q[s][0];
    for (int j=1; j<number_actions; j++){
        if (q[s][j]>max){
            max = q[s][j];
        }
    }
    return max;
}

// find an action that leads to the maximal gain among all actions for the state given as input
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

        mazeEnv_reset();
        int s = start_col + start_row*cols; //Initialization of the state
        int n = 0; //Number of actions performed in the episode
        int done = 0;

        //Initialization of the variables 
        int next_action;
        double tirage;
        struct envOutput EnvOut;
        int next_s = s;
        srand(time(NULL));


        while (n<horizon){

            if (epsilon != 1) epsilon = 0.1 + 0.9 * n / horizon;

            /*Pick an action according to the epsilon greedy algorithm : the agent chooses a random action 
            with probability epsilon, otherwise choose an action which maximises q(s, .)*/

            tirage = rand() / (RAND_MAX + 1.0);
            
            if (tirage < epsilon){
                next_action = action_to_int(env_action_sample());
            }
            else {
                next_action = best_action(s);
            }

            //Updating Q with the new state
        
            EnvOut = mazeEnv_step(int_to_action(next_action));
            next_s = EnvOut.new_col + EnvOut.new_row*cols;
            done = EnvOut.done;

            // if we reach the goal, the deplacements are now entirely random
            if (done == 1)
            {
                epsilon = 1;
            }            

            q[s][next_action] = q[s][next_action] + learning_rate*(EnvOut.reward + return_rate*max_actions(next_s) - q[s][next_action]);
            s = next_s;
            n++;
        }
        
    }

}

void botzmann_exploration(){
    for (int i = 0 ; i < number_episode ; i++){

        printf("\r%.5f %%", 100 * (float)i/number_episode);
        fflush(stdout);

        mazeEnv_reset();
        int s = start_col + start_row*cols; //Initialization of the state
        int n = 0; //Number of actions performed in the episode

        //Initialization of the variables 
        int next_action;
        double tirage;
        struct envOutput EnvOut;
        int next_s = s;
        srand(time(NULL));
        double total_exp;
        double e0;
        double e1;
        double e2;


        while (n<horizon){

            /*Pick an action according to the Botzmann exploration : he agent chooses an action randomly with a
            probability proportional to exp(q(s, a))*/
            total_exp = exp(q[s][0]) + exp(q[s][1]) + exp(q[s][2]) + exp(q[s][3]);
            e0 =  exp(q[s][0])/total_exp;
            e1 =  exp(q[s][1])/total_exp;
            e2 =  exp(q[s][2])/total_exp;

            tirage = rand() / (RAND_MAX + 1.0);
            
            if (tirage < e0){
                next_action = 0;
            }
            else if (tirage < (e0 + e1)){
                next_action = 1;
            }
            else if (tirage < (e0 + e1 + e2)){
                next_action = 2;
            }
            else {
                next_action = 3;
            }
            //Updating Q with the new state
        
            EnvOut = mazeEnv_step(int_to_action(next_action));
            next_s = EnvOut.new_col + EnvOut.new_row*cols;


            // mazeEnv_render_pos();
            
            q[s][next_action] = q[s][next_action] + learning_rate*(EnvOut.reward + return_rate*max_actions(next_s) - q[s][next_action]);
            s = next_s;
            n++;
        }
        
    }

}

void sarsa(){
    for (int i = 0 ; i < number_episode ; i++){

        printf("\r%.5f %%", 100 * (float)i/number_episode);
        fflush(stdout);
        
        //Initialization of the variables
        mazeEnv_reset();
        int next_action;
        double tirage;
        struct envOutput EnvOut;
        int next_s;
        srand(time(NULL));
        int n = 0; //Number of actions performed in the episode
        int done = 0;

        //Initialization of the state and action
        int s = start_col + start_row*cols; 
        int action = action_to_int(env_action_sample());


        while (n<horizon){

            if (epsilon != 1) epsilon = 0.1 + 0.9 * n / horizon;


            //Updating the new state
        
            EnvOut = mazeEnv_step(int_to_action(next_action));
            next_s = EnvOut.new_col + EnvOut.new_row*cols;
            done = EnvOut.done;

            /*Pick an action according to the epsilon greedy algorithm : the agent chooses a random action 
            with probability epsilon, otherwise choose an action which maximises q(s, .)*/

            tirage = rand() / (RAND_MAX + 1.0);
            
            if (tirage < epsilon){
                next_action = action_to_int(env_action_sample());
            }
            else {
                next_action = best_action(next_s);
            }


            // if we reach the goal, the deplacements are now entirely random
            if (done == 1)
            {
                epsilon = 1;
            }            

            //Updating q
            q[s][action] = q[s][action] + learning_rate*(EnvOut.reward + return_rate*q[next_s][next_action] - q[s][action]);

            //Update the strating state and action for the next round
            s = next_s;
            action = next_action;
            n++;
        }
        
    }

}

//Visualise the values of the q table
void visualise (){
    printf("\n");
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            if (mazeEnv[i][j] == 'o' || mazeEnv[i][j] == 'k') //if the case has been explored
            {
                /* If the best action of a state has a positive gain, the associated case will 
                be printed in green. Otherwise it will be printed in red */

                if (max_actions(j + i*cols) > 0.1)
                {
                    green(); 
                }
                else
                {
                    red();
                }

                // for each state, we print an arrow representing the best action
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
                //the walls are painted blue 
                blue();
                printf("%c ", mazeEnv[i][j]);
            }
            reset();
        }
        printf("\n");
    }
}

//print the q matrix
void print_q(){
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





















