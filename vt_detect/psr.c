#include "psr.h"
#include "bf_lib/bf_math.h"
#include <string.h>

#define GRID_SIZE 40
typedef struct{
    char grid[GRID_SIZE][GRID_SIZE];
}det_grid_t;

static void clr_grid(det_grid_t *grid)
{
    unsigned int i = 0;

    for(i = 0;i < GRID_SIZE;i++)
        memset(grid->grid[i], 0, GRID_SIZE*sizeof(char));
}

int sum_grid(det_grid_t *grid)
{
    int sum = 0;
    unsigned int i = 0;
    unsigned int j = 0;
    for(i = 0;i < GRID_SIZE;i++)
        for(j = 0;j < GRID_SIZE;j++){
            if(1 == grid->grid[i][j]) sum++;
        }
    return sum;
}
int calc_grid(int *data, size_t size, unsigned int delay)
{
    det_grid_t grid;
    clr_grid(&grid);

    int max = 0, min = 0;
    bf_max_min(data, size, &max, &min);
    int i = 0;
    if (max - min > 1600) return -1; 
    else if (max-min > 0.001)
        for(i = 0;i < size - delay;i++){
            unsigned int x = (unsigned int)((data[i] - min)*(GRID_SIZE-1)/(max - min));
            unsigned int y = (unsigned int)((data[i+delay] - min)*(GRID_SIZE-1)/(max - min));
            grid.grid[x][y] = 1;
        }

    return sum_grid(&grid);
}

