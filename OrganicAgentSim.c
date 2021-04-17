#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "lodepng.h"

#define M_PI 3.14159265358979323846

//Declaring structs
struct Cell
{
    int Pheromone;
};
struct Agent
{
    double PositionX;
    double PositionY;
    double Rotation;
    double RotationSpeed;
};

//Declaring variables (if other setting is chosen in comand line these will be overwriten
const int Width = 1920;
const int Height = 1080;
const int Steps = 1000;
const int AgentAmount = 1000;
int Disipate = 1;
double RotationSpeedMult = 1.0f;
double RandRotationSpeed = 0.1f;
double AgentVelocity = 3.0f;       //one cell per step
double SearchOffset = 0.785f;      //45 degres
struct Cell *tmp;
struct Cell SearchPos0 ;
struct Cell SearchPos1 ;
struct Cell SearchPos2 ;


struct Cell GetCellAt(int x, int y, struct Cell *Array, int Width, int Height)
{
    // if reqested cell is ouside boundery, return Cell with all valaus at int 0
    if ((x > Width - 1 || x < 0) || (y > Height - 1 || y < 0))
    {
        struct Cell EmptyCell;
        EmptyCell.Pheromone = 0;
        return EmptyCell;
    }

    return Array[y * Width + x];
};

int SetCellValues(struct Cell CellValues, int x, int y, struct Cell *Array, int Width, int Height)
{
    /* if reqested cell is ouside boundery, raise error */
    if ((x > Width - 1 || x < 0) || (y > Height - 1 || y < 0))
    {
        fprintf(stderr, "Requsted invalid Cell, at posotion x%i, y%i\n", x, y);
        return 1;
    };
    Array[y * Width + x] = CellValues;
    return 0;
};

void PositionAgents(struct Agent *AgentArray)
{
    /* Positining Agnt in the middle of the CellArray with random rotation */
    for (int i = 0; i < AgentAmount; i++)
    {
        AgentArray[i].PositionY = ((double)Height - 1.0) / 2.0;
        AgentArray[i].PositionX = ((double)Width - 1.0) / 2.0;
        AgentArray[i].Rotation = ((double)rand() / (double)(RAND_MAX)) * 2.0 * M_PI;
        AgentArray[i].RotationSpeed = 0.1;
    };
}

void UpdateCells(struct Cell **CellArray, struct Cell **TmpCellArray)
{

    /* decriment each cell and keep it from being negative*/
    for (int i = 0; i < Width * Height; i++)
    {
        (*CellArray)[i].Pheromone = (*CellArray)[i].Pheromone - Disipate;
        if ((*CellArray)[i].Pheromone < 0)
        {
            (*CellArray)[i].Pheromone = 0;
        };
    };

    /* mean blur on each cell, todo rewrite as funtcion*/
    for (int Width_i = 0; Width_i < Width - 1; Width_i++)
    {
        for (int Height_i = 0; Height_i < Height - 1; Height_i++)
        {
            struct Cell BluredCell;
            BluredCell.Pheromone =
                (GetCellAt(Width_i - 1, Height_i + 1, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i, Height_i + 1, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i + 1, Height_i + 1, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i - 1, Height_i, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i, Height_i, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i + 1, Height_i, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i - 1, Height_i - 1, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i, Height_i - 1, (*CellArray), Width, Height).Pheromone 
                + GetCellAt(Width_i + 1, Height_i - 1, (*CellArray), Width, Height).Pheromone) 
                / 9;
            SetCellValues(BluredCell, Width_i, Height_i, *TmpCellArray, Width, Height);
        };
    };
    tmp = *CellArray;
    *CellArray = *TmpCellArray;
    *TmpCellArray = tmp;
}

void UpdateAgents(struct Agent *AgentArray, struct Cell *CellArray)
{
    /*run for each agent*/
    for (int i = 0; i < AgentAmount; i++)
    {
        //uppdating each agent's rotation, doing fmod to keep doubles precision
        AgentArray[i].Rotation = (fmod((AgentArray[i].Rotation + AgentArray[i].RotationSpeed * RotationSpeedMult), (2 * M_PI)));

        //adding some random rotation speed to keep the agents from moving in a starigt line
        AgentArray[i].RotationSpeed = ((double)rand() / (double)(RAND_MAX)-0.5) * RandRotationSpeed;

        //get cells att search positions 
        SearchPos0 = GetCellAt((int)(AgentArray[i].PositionX+(cos(AgentArray[i].Rotation + SearchOffset))),(int)(AgentArray[i].PositionY-sin(AgentArray[i].Rotation + SearchOffset)),CellArray, Width, Height);
        SearchPos1 = GetCellAt((int)(AgentArray[i].PositionX+(cos(AgentArray[i].Rotation))),(int)(AgentArray[i].PositionY-sin(AgentArray[i].Rotation)),CellArray, Width, Height);
        SearchPos0 = GetCellAt((int)(AgentArray[i].PositionX+(cos(AgentArray[i].Rotation - SearchOffset))),(int)(AgentArray[i].PositionY-sin(AgentArray[i].Rotation - SearchOffset)),CellArray, Width, Height);

        // determine if a trun is necsecary
        if (!(SearchPos1.Pheromone > SearchPos0.Pheromone)&&!(SearchPos0.Pheromone > SearchPos2.Pheromone))
        {
            //trun in direction of largest phermone value
            if (SearchPos0.Pheromone > SearchPos2.Pheromone)
            {
                AgentArray[i].RotationSpeed = AgentArray[i].RotationSpeed + RotationSpeedMult;
            }
            if (SearchPos0.Pheromone < SearchPos2.Pheromone)
            {
                AgentArray[i].RotationSpeed = AgentArray[i].RotationSpeed - RotationSpeedMult;
            }
        }
        //Move Agent
        AgentArray[i].PositionX = AgentArray[i].PositionX - sin(AgentArray[i].Rotation) * AgentVelocity;
        AgentArray[i].PositionY = AgentArray[i].PositionY + cos(AgentArray[i].Rotation) * AgentVelocity;

        //if agent is outside boundery
        if ((AgentArray[i].PositionX > (double)(Width - 1) || AgentArray[i].PositionX < 0.0) || (AgentArray[i].PositionY > (double)(Height - 1) || AgentArray[i].PositionY < 0.0)){
            AgentArray[i].PositionX = AgentArray[i].PositionX + sin(AgentArray[i].Rotation) * AgentVelocity;
            AgentArray[i].PositionY = AgentArray[i].PositionY - cos(AgentArray[i].Rotation) * AgentVelocity;
            AgentArray[i].Rotation = ((double)rand() / (double)(RAND_MAX)) * 2.0 * M_PI;
        }
        struct Cell FullCell;
        FullCell.Pheromone = 255;
        if (SetCellValues(FullCell, (int)(AgentArray[i].PositionX), (int)(AgentArray[i].PositionY), CellArray, Width, Height)){
            fprintf(stderr,"Agent number %i\nx %f\ny %f\nrotation %f\nRotSpeed %f\n",i,AgentArray[i].PositionX,AgentArray[i].PositionY,AgentArray[i].Rotation,AgentArray[i].RotationSpeed);
        }
    };
    
    //printf("Rotation %f\nPosX %f\nPosY %f\n\n",AgentArray[AgentAmount-1].Rotation ,AgentArray[AgentAmount-1].PositionX,AgentArray[AgentAmount-1].PositionY);
}

void SavePng(struct Cell *CellArray, char *Name){
    unsigned char* image = malloc(Width * Height * 4);
    for(int y = 0; y < Height; y++) {
        for(int x = 0; x < Width; x++) {
            image[4 * Width * y + 4 * x + 0] = GetCellAt(x,y,CellArray,Width,Height).Pheromone;
            image[4 * Width * y + 4 * x + 1] = 0;
            image[4 * Width * y + 4 * x + 2] = 0;
            image[4 * Width * y + 4 * x + 3] = 255;
        }
    }
    lodepng_encode32_file(Name,image,Width,Height);
}
int main()
{
    srand(10U);

    /* initialize "Array" of struct Cell
    useing calloc instead of "struct Cell CellArray[1920][1080];" to avoid stack overflowing
    format of "Array", number refers to index
    0  1  2  3
    4  5  6  7
    8  9  10 11
    */
    struct Cell *CellArray = calloc(Width * Height, sizeof(*CellArray));
    struct Cell *TmpCellArray = calloc(Width * Height, sizeof(*TmpCellArray));

    /* initialize of array of agent's
    struct Agent AgentArray[10000];
    useing malloc to avoid stack overflowing,
    */
    struct Agent *AgentArray = calloc(AgentAmount, sizeof(*AgentArray));

    char Name[15];
    printf("positioning agents\n");
    PositionAgents(AgentArray);

    /* "main loop", runs for speciified amount of steps*/

    for (int CurentStep = 0; CurentStep < Steps; CurentStep++)
    {
        //printf("calculating step %i \n", CurentStep);

        UpdateCells(&CellArray, &TmpCellArray);

        UpdateAgents(AgentArray, CellArray);
        
        sprintf(Name,"jpg/%03d.jpg",CurentStep);
        SavePng(CellArray,Name);
    };
    
    printf("done");
    free(CellArray);
    free(TmpCellArray);
    return 0;
}