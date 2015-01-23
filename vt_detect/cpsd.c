#include <stdio.h>
#include <stdlib.h>/*abs*/
#include <math.h> /*sqrt*/
#include <string.h>
#include "psr.h"
#include "bf_lib/bf_math.h"
#define GRID_SIZE 40
typedef struct{
    unsigned int  grid[GRID_SIZE][GRID_SIZE];
}det_grid_t;
static void clr_grid(det_grid_t *grid)
{
    unsigned int i = 0;

    for(i = 0;i < GRID_SIZE;i++)
        memset(grid->grid[i], 0, GRID_SIZE*sizeof(unsigned int));
}

void gen_pm(int *data, size_t size, unsigned int delay, det_grid_t *grid)
{
    clr_grid(grid);

    int max = 0, min = 0;
    bf_max_min(data, size, &max, &min);
    int i = 0;
    if (max-min > 0.001)
        for(i = 0;i < size - delay;i++){
            unsigned int x = (unsigned int)((data[i] - min)*(GRID_SIZE-1)/(double)(max - min)+0.5);
            unsigned int y = (unsigned int)((data[i+delay] - min)*(GRID_SIZE-1)/(double)(max - min)+0.5);
            grid->grid[x][y] ++ ;
        }
}

unsigned int cpsd(int *data, size_t size, unsigned int delay) 
{
    static det_grid_t cur_grid, prev_grid; 
    static unsigned int index = 0;
    static double dev = 0.0, mean = 0.0, mean_pwr = 0.0;
        
    gen_pm(data, size, delay, &cur_grid);
    if (!index){
        index++;
        prev_grid = cur_grid;
        return 0;
    }
    unsigned int sum = 0;
    unsigned int x = 0, y=0; 
    for (x = 0;x < GRID_SIZE; x++)
        for(y = 0; y < GRID_SIZE; y++){
            sum += abs(cur_grid.grid[x][y] - prev_grid.grid[x][y]);
        }   
    mean = mean*((double)(index-1)/index) + sum*(1.0/index);
    mean_pwr = mean_pwr*((double)(index-1)/index) + sum*sum*(1.0/index);
    dev = mean_pwr - mean*mean; 
    prev_grid = cur_grid;
    index ++;
        printf("low:%f dif:%d high:%f\n", mean-3*sqrt(dev), sum, mean+3*sqrt(dev));
    return sum;
}
