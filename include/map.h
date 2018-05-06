#ifndef __MAP__H_
#define __MAP__H_
#include <stdbool.h>
#define CELL_SIZE 24//24

#define MAP_HEIGHT 30//30
#define MAP_WIDTH 30//30

#define WINDOW_WIDTH MAP_WIDTH*CELL_SIZE
#define WINDOW_HEIGHT MAP_HEIGHT*CELL_SIZE



int get_level(); // Renvoie simplement le niveau courant
int next_level(); // Passe au niveau suivant (+ chargement carte et texture)
void reset_level(); // Remet le jeu au premier niveau (+ chargement carte et texture)


void draw_map(); // Dessine la carte à l'écran.

/*
  Les trois fonctions suivantes font le pont entre la matrice de la carte courante et les coordonnées en pixels de la fenetre.
  Elles permettent de déterminer si une coordonnée correspond à un élément de la carte.
  Les paramètres entiers doivent être compris entre : 0 et WINDOW_WIDTH pour 'x', 0 et WINDOW_HEIGHT pour 'y'.
 */
bool isWall(int x, int y);  // Renvoie vrai si la coordonnée correspond à un mur extérieur
bool isPlatform(int x, int y); // Renvoie vrai si la coordonnée correspond à une plateforme
bool isSolid(int x, int y); // Renvoie vrai si mur extérieur OU plateforme




  
#endif
