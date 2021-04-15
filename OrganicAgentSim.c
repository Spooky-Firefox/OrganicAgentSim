#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/*Declaring structs*/
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

/* Declaring variables (if other setting is chosen in comand line these will be overwriten*/
const int Width = 1920;
const int Height = 1080;
const int Steps = 250;
const int AgentAmount = 10000;
int Disipate = 5;
float RotationSpeedMult = 1.0;
float RandRotationSpeed = 1.0;
float AgentVelocity = 1.0;

struct Cell GetCellAt(int x, int y, struct Cell *Array, int Width, int Height)
{
    /* if reqested cell is ouside boundery, return Cell with all valaus at int 0 */
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
        printf("Requsted invalid Cell, at posotion x%i, y%i", x, y);
        return 0;
    };
    Array[y * Width + x] = CellValues;
    return 1;
};

void PositionAgents(struct Agent *AgentArray)
{
    /* Positining Agnt in the middle of the CellArray with random rotation */
    for (int i = 0; i < AgentAmount; i++)
    {
        AgentArray[i].PositionY = ((double)Height - 1.0) / 2.0;
        AgentArray[i].PositionX = ((double)Width - 1.0) / 2.0;
        AgentArray[i].Rotation = (double)rand() / (double)RAND_MAX * 2.0 * 3.1415;
        AgentArray[i].RotationSpeed = 0;
    };
}

void UpdateCells(struct Cell *CellArray){
    struct Cell *NewCellArray = calloc(Width * Height, sizeof(*NewCellArray));
    /* decriment each cell and keep it from being negative*/
        for (int i = 0; i < Width * Height; i++)
        {
            CellArray[i].Pheromone = CellArray[i].Pheromone - Disipate;
            if (CellArray[i].Pheromone < 0)
            {
                CellArray[i].Pheromone = 0;
            };
        };

        /* mean blur on each cell, todo rewrite as funtcion*/
        for (int Width_i = 0; Width_i < Width - 1; Width_i++)
        {
            for (int Height_i = 0; Height_i < Height - 1; Height_i++)
            {
                struct Cell BluredCell;
                BluredCell.Pheromone =
                    (GetCellAt(Width_i - 1, Height_i + 1, CellArray, Width, Height).Pheromone + GetCellAt(Width_i, Height_i + 1, CellArray, Width, Height).Pheromone + GetCellAt(Width_i + 1, Height_i + 1, CellArray, Width, Height).Pheromone +
                     GetCellAt(Width_i - 1, Height_i, CellArray, Width, Height).Pheromone + GetCellAt(Width_i, Height_i, CellArray, Width, Height).Pheromone + GetCellAt(Width_i + 1, Height_i, CellArray, Width, Height).Pheromone +
                     GetCellAt(Width_i - 1, Height_i - 1, CellArray, Width, Height).Pheromone + GetCellAt(Width_i, Height_i - 1, CellArray, Width, Height).Pheromone + GetCellAt(Width_i + 1, Height_i - 1, CellArray, Width, Height).Pheromone) /
                    9;
                SetCellValues(BluredCell, Width_i, Height_i, NewCellArray, Width, Height);
            };
        };
        CellArray = NewCellArray;
    free(NewCellArray);
}

void UpdateAgents(struct Agent *AgentArray){
    /*run for each agent*/
    for (int i = 0; i < AgentAmount; i++)
    {
        /*uppdating each agent's rotation, doing fmod to keep doubles precision*/
        AgentArray[i].Rotation = fmod((AgentArray[i].Rotation + AgentArray[i].RotationSpeed * RotationSpeedMult), (2 * 3.1415));
    };
}
int main()
{
    /*set seed for random*/
    srand(10U);

    /* initialize "Array" of struct Cell
    useing calloc instead of "struct Cell CellArray[1920][1080];" to avoid stack overflowing
    format of "Array", number refers to index
    0  1  2  3
    4  5  6  7
    8  9  10 11
    */
    struct Cell *CellArray = calloc(Width * Height, sizeof(*CellArray));

    /* initialize of array of agent's
    struct Agent AgentArray[10000];
    useing malloc to avoid stack overflowing,
    */
    struct Agent *AgentArray = calloc(AgentAmount, sizeof(*AgentArray));


    printf("positioning agents");
    PositionAgents(AgentArray);

    /* "main loop", runs for speciified amount of steps*/
    
    for (int CurentStep = 0; CurentStep < Steps; CurentStep++)
    {
        printf("calculating step %i \n", CurentStep);
        
        UpdateCells(CellArray);

        UpdateAgents(AgentArray);
        
    };
    printf("done");
    return 0;
}