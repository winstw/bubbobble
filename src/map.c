

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "entity.h"

#include "map.h"
#include "tga_small.h"
#define NUMBER_LEVELS 3

/* -------------------- */

/* http://www.idevgames.com/forums/thread-3374.html */


/* /\* --------------------------------------- *\/ */

char* levels_tab[] = {"levels/map.txt", "levels/map2.txt", "levels/map3.txt"};
static int level = 0;


void draw_wall(int x, int y);        // dessine une case de mur, reçoit en argument les coordonnées du coin haut-gauche de la case
void init_wall_tex(void);
void init_level(int level);



 GLuint texWall = 0;


char current_map[MAP_HEIGHT][MAP_WIDTH] = {0};


int next_level(){
  if (level < NUMBER_LEVELS -1)
    level++;
  init_level(level);
  return level;
}

int get_level(){


  return level+1;
}

/* int get_level(){ */
/*   return level; */
/* } */

  
void reset_level(){
  level = 0;
  init_level(level);
}

void init_level(int level){
  read_map(levels_tab[level]);
}

void init_wall_tex(void)
{
  texWall = loadTGATexture ("images/wall.tga");
  /* if (!texWall) */
  /*   fprintf(stderr, "Probleme chargement texWall"); */
  /*   exit (EXIT_FAILURE); */
    
}



/* void print_map() */
/* { */
/*   for (int i=0; i < MAP_HEIGHT ; i++){ */
/*    for (int j = 0; j < MAP_WIDTH; j++) */
/*      printf("%c ", current_map[i][j]); */
/*    printf("\n"); */
/*   } */
/* } */


void read_map(char * file){
FILE *map_file = fopen(file, "r");

//char line[100];
 char c;

int i = 0;
int j = 0;
 while ((c = getc(map_file)) != EOF){
   if (c == '\n'){
     i++;
     j = 0;
   }
   else {
     current_map[i][j] = c;
     j++;
   }
  }
 
 }


void draw_wall(int x, int y){        // dessine une case de mur, reçoit en argument les coordonnées du coin haut-gauche de la case
  


  glPushMatrix();
  glTranslatef(x, y, 0);

/* -------------------------------------------------   */

//  fprintf(stderr, "id texWall : %i\n", texWall);

      glEnable(GL_TEXTURE_2D);
  glBindTexture (GL_TEXTURE_2D, texWall);
  
   
  glBegin(GL_QUADS);

  glTexCoord2f(0, 0);
  glVertex2f(0,  0);
  
  
  glTexCoord2f(1, 0);
  glVertex2f(CELL_SIZE, 0);
  
  
  glTexCoord2f(1, 1);
  glVertex2f(CELL_SIZE,CELL_SIZE);
    

  glTexCoord2f(0, 1);
  glVertex2f(0, CELL_SIZE);
  glEnd();

  glPopMatrix();
       glDisable(GL_TEXTURE_2D);

  
}

  
void draw_map(){

  //  fprintf(stderr, "id texWall : %i\n", texWall);

 

  /* texWall = loadTGATexture ("images/wall.tga"); */
  /* if (!texWall) */
  /*   exit (EXIT_FAILURE); */
  

 for (int i=0; i < MAP_HEIGHT ; i++){
   for (int j = 0; j < MAP_WIDTH; j++){
     if (current_map[i][j] == '#')
       draw_wall( j * CELL_SIZE, i * CELL_SIZE);
     else if (current_map[i][j] == '=')
       draw_wall( j * CELL_SIZE, i * CELL_SIZE);
   }


   /* glDisable(GL_TEXTURE_2D); */
   
        /*   // initialiser le bubble avec sa position à la lecture de la map (bonne idee ?) */

     /* else if (map[i][j] == 'b'){ */
     /*   bubptr->x = j * CELL_SIZE; */
     /*   bubptr->y = i * CELL_SIZE; */
     /* } */
   
	 
     /* else if(map[i][j] == '*') */
       /* draw */
 }
}

bool isWall(int x, int y){

   
  if (current_map[y / CELL_SIZE][x/CELL_SIZE] == '#')
    return true;
  return false;
}

bool isPlatform(int x, int y){
  if (current_map[y / CELL_SIZE][x/CELL_SIZE] == '=')
    return true;
  return false;
}

bool isSolid(int x, int y){
  return isWall(x, y) || isPlatform(x, y);
}



