#ifndef __ENTITY__H__
#define __ENTITY__H__

#include <stdbool.h>
#include <GL/glut.h>







char get_lives();
int get_score();



void display_entities();
void create_entities();


void jump_drag(void);
void launch_bubble();


void set_direction_drag(char direction);
void set_moving_drag(bool moving);

void init_entities();


int getEnemiesAlive();
#endif
