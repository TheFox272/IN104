# Reinforcement Learning Project (IN104)


## Presentation

This is a repository dedicated to our Reinforcement Learning (RL) project of ENSTA's IN104 course. It is made of 2 parts :
- [x] An introduction to the concept of RL : maze resolution
- [ ] A personal project using RL : *to determine*

Our RL methods revolve around the **Q-learning** algorithm. Up to now, we implemented 2 versions of it :
- ε-greedy
- Boltzmann exploration


## Repository structure

This repository is made of several folders:
- **./data** is where you'll find all the data files (raw .txt files, .png image files, ...)
- **./include** is where you'll find all the headers (.h files)
- **./src** is where you'll find all the C code (.c files)


## Building and running

To build the executable (currently only the maze one is available) and run it, use the following instructions :
1. Make sure you're in the **IN104** folder using **pwd** in the cmd. If not, move to it using **cd**
2. Use the Makefile which will create a **./build** folder and store the executable **main** as well as all the files linked to its creation (.o files)
```
make
```
3. To run the code, go to the **./build** folder and run the **main** file
```
cd build
./main
```
4. Once you're done, make sure to clean everything : go back to **IN104** and use the Makefile clean option (this will delete the whole **./build** folder)
```
cd ..
make clean
```




