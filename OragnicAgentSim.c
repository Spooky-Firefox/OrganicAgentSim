#include <stdlib.h>
#include <stdio.h>


/*Declaring structs*/
struct Cell
{
    int Pheromone;
};
struct Agent
{
    float PositionX;
    float PositionY;
    float Rotation;
    float RotationSpeed;
};

struct Cell GetCellAt(int x,int y,struct Cell *Array, int Width, int Height){
    /* if reqested cell is ouside boundery, return Cell with all valaus at int 0 */
    if ((x > Width-1 || x < 0)||(y > Height-1 || y < 0)){   
        struct Cell EmptyCell;
        EmptyCell.Pheromone = 0;
        return EmptyCell;
    }

    return Array[y*Width + x];
};

int SetCellValues(struct Cell CellValues, int x,int y,struct Cell *Array, int Width, int Height){
    /* if reqested cell is ouside boundery, raise error */
    if ((x > Width-1 || x < 0)||(y > Height-1 || y < 0)){   
        printf("Requsted invalid Cell, at posotion x%i, y%i", x ,y );
        return 0;
    };
    Array[y*Width + x] = CellValues;
    return 1;
};

int main() {
    

    /* Declaring variables */
    const int  Width = 1920;
    const int Height = 1080;
    const int Steps = 250;
    const int AgentAmount = 10000;
    int Disipate = 5;
    float RotationSpeedMult = 1.0;
    float RandRotationSpeed = 1.0;
    float AgentVelocity = 1.0;
    srand(10U);

    /* initialize "Array" of struct Cell
    useing calloc instead of "struct Cell CellArray[1920][1080];" to avoid stack overflowing
    format of "Array", number refers to index
    0  1  2  3
    4  5  6  7
    8  9  10 11
    */
    struct Cell* CellArray = calloc(1,Width*Height*sizeof(*CellArray));
    
    struct Cell* NewCellArray = calloc(1, Width*Height*sizeof(*NewCellArray));
    
    /* initialize of array of agent's
    struct Agent AgentArray[10000];
    useing malloc to avoid stack overflowing,
    */
    struct Agent* AgentArray = malloc(AgentAmount*sizeof(*AgentArray));

    /* Positining Agnt in the middle of the CellArray with random rotation */ 
    for (int i = 0; i < AgentAmount; i++){
        AgentArray[i].PositionY = ((float)Height-1.0) / 2.0;
        AgentArray[i].PositionX = ((float)Width-1.0) / 2.0;
        AgentArray[i].Rotation = (float)rand() / (float)RAND_MAX * 2.0 * 3.1415;
    };
    
    /* "main loop", runs for speciified amount of steps*/
    printf("positioning agents");
    for (int CurentStep = 0; CurentStep < Steps; CurentStep++){
        printf("calculating step %i \n",CurentStep);
        /* decriment each cell*/
        for(int i = 0; i < Width*Height; i++){
            CellArray[i].Pheromone = CellArray[i].Pheromone - Disipate;
            if (CellArray[i].Pheromone < 0){
                CellArray[i].Pheromone = 0;
            };
        };

        /* mean blur on each cell */
        for(int Width_i = 0; Width_i < Width-1; Width_i++){
            for (int Height_i = 0; Height_i < Height-1; Height_i++){
                struct Cell BluredCell;
                BluredCell.Pheromone = 
                    (GetCellAt(Width_i-1,Height_i+1,CellArray,Width,Height).Pheromone + GetCellAt(Width_i,Height_i+1,CellArray,Width,Height).Pheromone + GetCellAt(Width_i+1,Height_i+1,CellArray,Width,Height).Pheromone +
                    GetCellAt(Width_i-1,Height_i,CellArray,Width,Height).Pheromone + GetCellAt(Width_i,Height_i,CellArray,Width,Height).Pheromone + GetCellAt(Width_i+1,Height_i,CellArray,Width,Height).Pheromone +
                    GetCellAt(Width_i-1,Height_i-1,CellArray,Width,Height).Pheromone + GetCellAt(Width_i,Height_i-1,CellArray,Width,Height).Pheromone + GetCellAt(Width_i+1,Height_i-1,CellArray,Width,Height).Pheromone)/9;
                SetCellValues(BluredCell,Width_i,Height_i,NewCellArray,Width,Height);
            };
            
        };
        CellArray = NewCellArray;

    };
    printf("done");
    return 0;
}