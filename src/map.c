

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include "entity.h"

#include "map.h"
#include "tga_small.h"
#define NUMBER_LEVELS 5




/* -------------------- */
/*
SOURCES : 
Niveaux du jeu : http://www.adamdawes.com/retrogaming/bbguide/index.html

Chargement textures :  http://www.idevgames.com/forums/thread-3374.html */


/* /\* --------------------------------------- *\/ */

char* levels_tab[] = {"levels/map.txt",
		      "levels/map2.txt",
		      "levels/map3.txt",
                      "levels/map4.txt",
                      "levels/map5.txt"};
char* textures_tab[] = {"images/level1.tga",
			"images/level2.tga",
			"images/level3.tga",
			"images/level4.tga",
			"images/level5.tga",
			"images/level6.tga",
			"images/level7.tga",
			"images/level8.tga",
			"images/level9.tga",
			"images/level10.tga"};
			

static int level = 0;


void draw_wall(int x, int y);        // dessine une case de mur, reçoit en argument les coordonnées du coin haut-gauche de la case
//void init_wall_tex(void);
void init_level(int level);



 GLuint texWall = 0;


char current_map[MAP_HEIGHT][MAP_WIDTH] = {0};


int next_level(){
  /* Passe au niveau suivant  */
  if (level < NUMBER_LEVELS -1)
    level++;
  init_level(level);
  return level;
}

int get_level(){
  /* Renvoie le numero du niveau courant */

  return level+1;
}


  
void reset_level(){
  level = 0;
  init_level(level);
}

void init_level(int level){
  read_map(levels_tab[level]);
  texWall = loadTGATexture(textures_tab[level]);
}


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



