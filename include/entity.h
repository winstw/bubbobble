#ifndef __ENTITY__H__
#define __ENTITY__H__

#include <stdbool.h>
#include <GL/glut.h>

#define MAX_ENEMIES 9
#define DEFAULT_NUMBER_ENEMIES 4


char get_lives(); // Renvoie le nombre courant de vies du dragon.

int get_score();  // Renvoie le score courant du dragon.

void reset_score(); // Remet le score du dragon à 0.

void manage_entities(); // Fonction principale du module qui gère la liste chainée des entités (affichage, mouvements, ia, mort, collisions).

void create_entities(); // Crée la liste des entités et les initialise avec la valeur par défaut d'ennemis (DEFAULT_NUMBER_ENEMIES).

void jump_drag(); // Fait sauter le dragon.

void launch_bubble(); // Fait lancer une bulle par le dragon.

void set_direction_drag(char direction); // Détermine la direction du dragon ('R' ou 'L').

void set_moving_drag(bool moving); // Met le dragon en mouvement.

void set_difficulty(int choice); // Selectionne le niveau de difficulté. (entre 0 et NUMBER_DIFFICULTIES - 1).

void init_entities(); // Réintialise les entités (position, score, vies,...).

int get_enemies_alive(); // Renvoie le nombre d'ennemis encore en vie (pas encore mangés) dans le jeu.

void set_number_enemies(int n); // Recrée les entités du jeu avec n (entier positif) ennemis.

void clean(); // Libère toutes les allocations mémoires réalisées pour les entités et les bulles.




#endif
