#ifndef __MAP__H_
#define __MAP__H_
#include <stdbool.h>
#define CELL_SIZE 24//24

#define MAP_HEIGHT 30//30
#define MAP_WIDTH 30//30

#define WINDOW_WIDTH MAP_WIDTH*CELL_SIZE
#define WINDOW_HEIGHT MAP_HEIGHT*CELL_SIZE


/* typedef struct{ */
/*   int x; */
/*   int y; */
    
/*   int right; */
/*   int left; */
/*   int bottom; */
/*   int up; */
    


/* }Node; */


int get_level();
void reset_level();
int next_level();

void init_wall_tex(void);
void read_map();


/* void print_map(); */

    
void draw_map();

bool isWall(int x, int y);
bool isPlatform(int x, int y);
bool isSolid(int x, int y);




  
#endif
