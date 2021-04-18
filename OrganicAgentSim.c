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

//Declaring variables
const int Width = 1920;
const int Height = 1080;
const int Steps = 1000;
const int AgentAmount = 10000;
int Disipate = 1;
double RotationSpeedMult = 1.0f;
double RandRotationSpeed = 1.0f;
double AgentVelocity = 5.0f;
double SearchOffset = 0.5f;
double SearchDistans = 20.0f;
int SearchSize = 1;

int inttmp;
struct Cell *tmp;
int SearchPos0;
int SearchPos1;
int SearchPos2;

/*
fuinction: returns the cell att the given position, if the position is outside retun a struct with all values at 0
Arg 0(int x): the x position of the requsted cell
Arg 1(int y): the y position of the requsted cell
Arg 2(pointer to struct Cell Array): The Array of cells
Arg 3(int Width): the width of the 2d array
Arg 4(int Height): the height of the 2d array
return(struct Cell): returns the struct cell att the given position
inplace: Yes, no writing occurs
Todo:
Update:
By: Olle Ronstad
Date: 2021-04-18
*/
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

/*
fuinction: replace the cell at the position with the given cell
Arg 0(struct Cell): the cell to place att the given cordenets
Arg 1(int x): the x position to place the given cell
Arg 2(int y): the y position to place
Arg 3(pointer to struct Cell Array):
Arg 4(int Width): the width of the 2d array
Arg 5(int Height): the height of the 2d array
retunr(int):retuns 0 if the operation is sucesful, returns 1 of the reqested cell is outside the 2d array
inplace: Yes
Todo:
Update:
By: Olle Ronstad
Date: 2021-04-18
*/
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

/*
fuinction: postions the agent in the middle with random rotation
Arg 0(pointer to Struct Agent Array): the agents to position
inplace: Yes
Todo: position them 
Update:
By: Olle Ronstad
Date: 2021-04-18
*/
void PositionAgents(struct Agent *AgentArray)
{
    double doubleTmp;
    /* Positining Agnt in the middle of the CellArray with random rotation */
    for (int i = 0; i < AgentAmount; i++)
    {
        doubleTmp = ((double)rand() / (double)(RAND_MAX)) * 2.0 * M_PI;
        AgentArray[i].PositionY = ((double)Height - 1.0) / 2.0 + sin(doubleTmp)*200;
        AgentArray[i].PositionX = ((double)Width - 1.0) / 2.0 - cos(doubleTmp)*200;
        AgentArray[i].Rotation = doubleTmp;
        AgentArray[i].RotationSpeed = 0;
    };
}

/*
fuinction: decriments each cells pheromne value, sets each cell to the avrage of the neiguring cells to emulate dispersion
Arg 0(pointer to pointer to Struct Cell Array): the array with the Cells
Arg 1(pointer to pointer to Struct Cell Array): Space for the changes, will swap pointer with arg 0
inplace: Yes
Todo:
Update:
By: Olle Ronstad
Date: 2021-04-18
*/
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
                (GetCellAt(Width_i - 1, Height_i + 1, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i, Height_i + 1, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i + 1, Height_i + 1, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i - 1, Height_i, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i, Height_i, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i + 1, Height_i, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i - 1, Height_i - 1, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i, Height_i - 1, (*CellArray), Width, Height).Pheromone + GetCellAt(Width_i + 1, Height_i - 1, (*CellArray), Width, Height).Pheromone) / 9;
            SetCellValues(BluredCell, Width_i, Height_i, *TmpCellArray, Width, Height);
        };
    };
    tmp = *CellArray;
    *CellArray = *TmpCellArray;
    *TmpCellArray = tmp;
}

/*
fuinction: returns the summ of pheromne in a sqare around th given x and y position with the total zize of 2*Searchsize+1
Arg 0(int x): the x position to place the given cell
Arg 1(int y): the y position to place
Arg 2(int SearchSize): the size of the area to search
Arg 3(pointer to struct Cell Array): the array of cells to search
inplace: Yes, no writing occurs
Todo:
Update:
By: Olle Ronstad
Date: 2021-04-18
*/
int SearchArea(int x, int y, int SearchSize, struct Cell *CellArray)
{
    inttmp = 0;
    for (int Width_i = x - SearchOffset; Width_i < x + SearchOffset; Width_i++)
    {
        for (int Height_i = y - SearchOffset; Height_i < y + 1; Height_i++)
        {
            inttmp = inttmp + GetCellAt(Width_i, Height_i, CellArray, Width, Height).Pheromone;
        }
    }
    return inttmp;
}

/*
fuinction:Uppadtes the Agents position, rotation and rotation speed. to make Agents follow Phermone, Writes a phermone value on each cell "under" the agents
Arg 0(pointer to Struct Agent Array): the array of agents
Arg 0(pointer to Struct Cell Array): the array of cells
inplace: Yes
Todo:
Update:
By: Olle Ronstad
Date: 2021-04-18
*/
void UpdateAgents(struct Agent *AgentArray, struct Cell *CellArray)
{
    /*run for each agent*/
    for (int i = 0; i < AgentAmount; i++)
    {
        //uppdating each agent's rotation, doing fmod to keep doubles precision
        AgentArray[i].Rotation = (fmod((AgentArray[i].Rotation + AgentArray[i].RotationSpeed * RotationSpeedMult), (2.0 * M_PI)));

        //adding some random rotation speed to keep the agents from moving in a starigt line
        AgentArray[i].RotationSpeed = ((double)rand() / (double)(RAND_MAX)-0.5) * RandRotationSpeed;

        //get cells att search positions
        SearchPos0 = SearchArea((int)(AgentArray[i].PositionX + (cos(AgentArray[i].Rotation + SearchOffset) * SearchDistans)), (int)(AgentArray[i].PositionY - sin(AgentArray[i].Rotation + SearchOffset) * SearchDistans), SearchSize, CellArray);
        SearchPos1 = SearchArea((int)(AgentArray[i].PositionX + (cos(AgentArray[i].Rotation) * SearchDistans)), (int)(AgentArray[i].PositionY - sin(AgentArray[i].Rotation) * SearchDistans), SearchSize, CellArray);
        SearchPos2 = SearchArea((int)(AgentArray[i].PositionX + (cos(AgentArray[i].Rotation - SearchOffset) * SearchDistans)), (int)(AgentArray[i].PositionY - sin(AgentArray[i].Rotation - SearchOffset) * SearchDistans), SearchSize, CellArray);

        // determine if a trun is necsecary
        if (!(SearchPos1 > SearchPos0) && !(SearchPos1 > SearchPos2))
        {
            //trun in direction of largest phermone value
            if (SearchPos0 > SearchPos2)
            {
                AgentArray[i].RotationSpeed = AgentArray[i].RotationSpeed + RotationSpeedMult;
            }
            if (SearchPos0 < SearchPos2)
            {
                AgentArray[i].RotationSpeed = AgentArray[i].RotationSpeed - RotationSpeedMult;
            }
        }
        //Move Agent
        AgentArray[i].PositionX = AgentArray[i].PositionX + cos(AgentArray[i].Rotation) * AgentVelocity;
        AgentArray[i].PositionY = AgentArray[i].PositionY - sin(AgentArray[i].Rotation) * AgentVelocity;

        //if agent is outside boundery
        if ((AgentArray[i].PositionX > (double)(Width - 1) || AgentArray[i].PositionX < 0.0) || (AgentArray[i].PositionY > (double)(Height - 1) || AgentArray[i].PositionY < 0.0))
        {
            AgentArray[i].PositionX = AgentArray[i].PositionX + sin(AgentArray[i].Rotation) * AgentVelocity;
            AgentArray[i].PositionY = AgentArray[i].PositionY - cos(AgentArray[i].Rotation) * AgentVelocity;
            AgentArray[i].Rotation = ((double)rand() / (double)(RAND_MAX)) * 2.0 * M_PI;
        }
        struct Cell FullCell;
        FullCell.Pheromone = 255;
        if (SetCellValues(FullCell, (int)(AgentArray[i].PositionX), (int)(AgentArray[i].PositionY), CellArray, Width, Height))
        {
            fprintf(stderr, "Agent number %i\nx %f\ny %f\nrotation %f\nRotSpeed %f\n", i, AgentArray[i].PositionX, AgentArray[i].PositionY, AgentArray[i].Rotation, AgentArray[i].RotationSpeed);
        }
    };

    //printf("Rotation %f\nPosX %f\nPosY %f\n\n",AgentArray[AgentAmount-1].Rotation ,AgentArray[AgentAmount-1].PositionX,AgentArray[AgentAmount-1].PositionY);
}

/*
fuinction: generates an 3d array[width][height][colour] of bytes(char) representing colour of an image, uses LodePNG (zlib licence)
Arg 0(pointer to Struct Agent Array): the cell array to save as png
Todo: make aheatmap insted of only writing one value
Update:
By: Olle Ronstad
Date: 2021-04-18
*/
void SavePng(struct Cell *CellArray, char *Name)
{
    unsigned char *image = malloc(Width * Height * 4);
    for (int y = 0; y < Height; y++)
    {
        for (int x = 0; x < Width; x++)
        {
            image[4 * Width * y + 4 * x + 0] = 0;
            image[4 * Width * y + 4 * x + 1] = 0;
            image[4 * Width * y + 4 * x + 2] = GetCellAt(x, y, CellArray, Width, Height).Pheromone;
            image[4 * Width * y + 4 * x + 3] = 255;
        }
    }
    lodepng_encode32_file(Name, image, Width, Height);
}

int main()
{
    srand(400U);

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
        printf("Calculating step %i \n", CurentStep);
        printf("Uppdaing Cells\n");
        UpdateCells(&CellArray, &TmpCellArray);
        printf("Uppdating Agents\n");
        UpdateAgents(AgentArray, CellArray);

        sprintf(Name, "jpg/%03d.jpg", CurentStep);
        printf("Saving image\n");
        SavePng(CellArray, Name);
        strcat(Name, "\n");
        printf(Name);
    };

    printf("Done");
    free(CellArray);
    free(TmpCellArray);
    return 0;
}