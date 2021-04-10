#include <stdlib.h>
#include <stdio.h>


/*Declaring structs*/
struct Cell
{
    int pheromone;
};
struct Agent
{
    float PositionX;
    float PositionY;
    float Rotation;
    float RotationSpeed;
};

struct Cell GetCellAt(int x,int y,struct Cell *Array, int Width){
    return Array[y*Width + x];
};



int main() {
    

    /*Declaring variables*/
    const unsigned int  Width = 1920;
    const unsigned int Height = 1080;
    const unsigned int Steps = 250;
    const int AgentAmount = 10000;
    int Disipate = 5;
    float RotationSpeedMult = 1.0;
    float RandRotationSpeed = 1.0;
    float AgentVelocity = 1.0;
    srand(10U);

    /*initialize "Array" of struct Cell
    useing malloc to avoid stack overflowing instead of "struct Cell CellArray[1920][1080];"
    format of "Array", number refers to index
    0  1  2  3
    4  5  6  7
    8  9  10 11
    */
    struct Cell* CellArray = malloc(Width*Height*sizeof(*CellArray));

    /*initialize of array of agent's
    struct Agent AgentArray[10000];
    useing malloc to avoid stack overflowing,
    */
    struct Agent* AgentArray = malloc(AgentAmount*sizeof(*AgentArray));

    /* Positining Agnt in the middle of the CellArray with random rotation*/ 
    for (int i = 0; i < AgentAmount; i++)
    {
        AgentArray[i].PositionY = (float)Height / 2.0;
        AgentArray[i].PositionX = (float)Width / 2.0;
        AgentArray[i].Rotation = (float)rand() / (float)RAND_MAX * 2.0 * 3.1415;
        printf("%f\n",AgentArray[i].Rotation);
    };
    

    return 0;
}