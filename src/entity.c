#include <stdio.h>
#include <stdbool.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <time.h>
#include "graphics.h"
#include "entity.h"
#include "map.h"
#include "tga_small.h"
#include <math.h>
#define DRAGON_START_X 4*CELL_SIZE
#define DRAGON_START_Y (MAP_HEIGHT-1)*CELL_SIZE
#define ENEMY_START_X WINDOW_WIDTH / 2
#define ENEMY_START_Y 4*CELL_SIZE
#define NUMBER_ENEMIES 4

#define BUBBLE_LIFESPAN 100
#define NUMBER_LIVES 3
#define NEW_LIFE_DELAY 30  // le nombre de tours entre le moment où le joueur est replacé après la "mort" et avant qu'il puisse être atteint par les ennemis

#define DEATH_STAGES 100 // length of the agony of the entity.

#define AGONY DEATH_STAGES
#define REPLACED NEW_LIFE_DELAY
#define ITEMIZED 2
#define DEAD -1
#define ALIVE 0


#define PI 3.14
//#define ENTITY_SIZE 40
#define ENTITY_SIZE 40
//#define JUMP 30
#define JUMP 30

#define MOVE_V CELL_SIZE/4
#define MOVE_H CELL_SIZE/8

#define BUBBLE_V CELL_SIZE/4
#define BUBBLE_H CELL_SIZE/8

typedef struct Bubble{
  int x;
  int y;
  int lifespan;
  char direction;
  bool empty;
  struct Bubble *next;
}Bubble, *BubbleList;


/* ------------------------------ */

typedef struct Entity{
  int x;
  int y;
  char direction;
  bool moving;
  int jump_stage;// permet de savoir si le personnage est en train de sauter, et si ouidrag, ou il en est dans le saut
  bool drag;
  int state; // si il meurt : devient plus grand que 0 (permet de gerer l'animation de la mort du personnage
  GLuint texture_id;
  int score;
  int lives;
  Bubble *bubbledTo; // pointeur vers la bulle dans laquelle l'entité est enfermée.
  BubbleList bubbles;
  struct Entity *next;
  struct Entity *previous;
  
  
}Entity, *EntityList;

/* ------------------------------- */

static EntityList list_first = NULL;
static EntityList list_last = NULL;


static int enemies_alive = 0;

GLuint tex_bad_dead;
GLuint tex_bad;

/* -------------------------------- */


static EntityList entities;

/*-----------------------------------*/

EntityList new_list(void);
bool is_empty_list(void);

EntityList add_end_list(int x, int y, EntityList l);
EntityList add_front_list(int x, int y, EntityList l, bool isDragon, GLuint texture_id, char direction);
EntityList remove_front_list(EntityList l);
/* void display_entities_alive(EntityList l); */
void randomize_entity(Entity *e);



void gravity(Entity *e);
/* void gravity_drag(); */


bool isOnSolidGround(Entity* e);


void display_bubble(Bubble *b);


bool touch_entity(Entity *e, Entity *e2);

void kill_dragon(void);
  

bool touch_any_entity(Entity *e, EntityList l);
void display_entity(Entity *e);
/* void display_drag(void); */

void move_entity(Entity *e);
/* void move_drag(); */

void jump_entity(Entity *e);




void place_dragon(void);
void add_bubble(Entity *e);
void display_move_bubbles(Bubble *b);
void move_bubble(Bubble *b);

void test_collisions(Entity* e);


void init_entity(Entity* e);

/* Entity *dragptr = NULL; */



int getEnemiesAlive(void){
  return enemies_alive;
}
  
EntityList new_list(void){
  return NULL;
    }


void create_entities(){
  srand(time(NULL));
  entities = new_list();

  

 tex_bad = loadTGATexture("images/enemy.tga");
  GLuint tex_drag = loadTGATexture("images/drag.tga");
  tex_bad_dead = loadTGATexture("images/banana.tga");
    /* if (!tex_bad){ */

  /*   fprintf(stderr, "probleme chargement texture\n"); */
  /*       exit(EXIT_FAILURE); */
  /* } */


 for (int i = NUMBER_ENEMIES; i > 0; i--){
   
   entities = add_front_list(ENEMY_START_X + i * ENTITY_SIZE , ENEMY_START_Y, entities, false, tex_bad, 'L');
 
 } entities = add_front_list(DRAGON_START_X, DRAGON_START_Y, entities, true, tex_drag, 'R');
 init_entities();

 /* entities_pointer = entities; */

}


bool is_empty_list(void){
  /* necessaire ? */
if (list_first == NULL && list_last == NULL)
  return true;
return false;
}

#if 0
EntityList add_end_list(int x, int y, EntityList l){
  Entity *new_entity;
  new_entity = malloc(sizeof(*new_entity));
  if (new_entity = NULL){
    printf("Memory allocation error");
    exit(EXIT_FAILURE);
  }
  new_entity -> next = NULL;
  new_entity -> x = x;
  new_entity -> y = y;
    
  list_last = new_entity;
  if (list_first == NULL)
    list_first == new_entity;
  nb_entity ++;
  return list_first;
 
}

#endif



char get_random_direction(void){
   srand(time(NULL));

  char direction;
  int prob = rand()%100;
  if (prob < 50){
    direction = 'R';
  } else direction = 'L';

  
  return direction;
}

EntityList add_front_list(int x, int y, EntityList l, bool isDragon, GLuint texture_id, char dir){
  Entity *new_entity;
  new_entity = malloc(sizeof(*new_entity));
  if (new_entity == NULL){
    printf("Memory allocation error");
    exit(EXIT_FAILURE);
  }


  // new_entity -> x = x; 
  //  new_entity -> y = y; 
  /* //  new_entity -> direction = dir; */
  /* new_entity -> jump_stage = 0; */
  /* new_entity -> moving = true; */
        new_entity -> state = DEAD;
 new_entity -> drag = isDragon;
  /* new_entity -> state = 0; */
 new_entity -> texture_id = texture_id; 
   new_entity -> next = l; 
   // new_entity -> bubbles = NULL; 
   // new_entity -> bubbledTo = NULL;

 // if (isDragon){
  /*   new_entity-> score = 0; */
  /*   new_entity-> lives = NUMBER_LIVES; */
  /*   new_entity -> direction = 'R'; */
  /*   new_entity -> moving = false; */
  /* } else { */
  /*   enemies_alive += 1; */
  /* } */
  /* if (list_last == NULL) */
  /*   list_last = new_entity; */
  
  /* list_first = new_entity; */
  /* //  enemies_alive++; */
  return new_entity;
}


void init_entities(){
  Entity *l = entities;
  while (l != NULL){
    init_entity(l);
    l = l->next;
  }
}

void init_entity(Entity* e){


  e -> direction = 'R';
  e -> jump_stage = 0;
  e -> moving = true;
  //e -> drag = isDragon;

  //  e -> texture_id = texture_id;
  //  e -> next = l;
  e -> bubbles = NULL;
  e -> bubbledTo = NULL;

  if (e->drag){
    
    
    if (e-> state != ALIVE){
      e -> state = ALIVE;
      e-> score = 0;
    }
      e -> x = DRAGON_START_X;
      e -> y = DRAGON_START_Y;

      
      if (e -> lives == 0)
	e-> lives = NUMBER_LIVES; // evite de rajouter des vies entre les niveaux
    e -> direction = 'R';
    e -> moving = false;
  } else {
    e -> texture_id = tex_bad;
    e -> x = ENEMY_START_X + ENTITY_SIZE * enemies_alive ; // décale les ennemis
    e -> y = ENEMY_START_Y;


    if (e->state == DEAD){
      e->state = ALIVE;
      enemies_alive += 1;
    }
  }
}

EntityList remove_front_list(EntityList l){
  Entity *temp = NULL;
  if (is_empty_list)
    return NULL;
  if (list_last == list_first){ // only one element in list
    free(list_first);
    list_last = NULL;
    list_first = NULL;
    //nb_entity = 0;
  }
  else {
    temp = list_first;
    list_first = list_first -> next;
    free(temp);
    //    nb_entity--;
  }
     
    return list_first;
  
}




void randomize_entity(Entity *e){
  int set_jump;
  set_jump = rand() % 100;
  if (set_jump == 1)
    jump_entity(e);
}
      
    


void set_direction_drag(char direction){
  entities->direction = direction;

    }



void set_moving_drag(bool moving){
  entities->moving = moving;
}
/* ---------------------- */
/* ne sert plus a rien si drag est dans la liste */
void display_drag(){
  display_entity(entities);
  
}


void place_dragon(void){
  entities -> y = DRAGON_START_Y;
  entities -> x = DRAGON_START_X;



}








void display_entities(void){
  Entity *l = entities;
  while (l != NULL){
    if ((l->state != DEAD)){
      display_entity(l);
      gravity(l);
      move_entity(l);
      if (!(l->drag)){
	test_collisions(l);
	if (l->state == ALIVE)
	  randomize_entity(l);
      }
      
    }
    
    l = l-> next;
  }
}

void display_entity(Entity *e){


  if (e->bubbles != NULL){
    display_move_bubbles (e->bubbles);
    //	 display_bubble(e->bubbles);
      }
    


    
  int invert_texture;           // pour que le personnage soit orienté vers là où il se déplace 
    if (e->direction == 'L')
      invert_texture = 1;
    else invert_texture = 0;


    
    glPushMatrix();

  


    glTranslatef(e->x, e->y - (ENTITY_SIZE/2),0); // coord = au milieu en bas de l'entité


#if 0

    //original
    
    if (e-> state > ITEMIZED){                    // animation de la mort du dragon : tourne sur lui-même
      if (e->state > REPLACED){     // jusqu'à quand on arrive à NEW_LIFE_DELAY et qu'il est remis en place 
	glRotatef((e->state) *20, 0, 0, 1);
      }
      (e-> state)-= 1;
    }
    
    if (e-> state == ITEMIZED){ // SI PAS DRAGON, DEAD_STAGE reste à 2
      if (e-> drag)
	(e->state) = ALIVE;
    }
      
      if (e-> state == REPLACED){  // on remet le dragon en place avant sa "nouvelle vie", permet d'éviter qu'il ne meure si un ennemi était à sa place de naissance.
	if (e-> drag)
	  place_dragon();
	else  e-> texture_id = tex_bad_dead;
      }


#else
if (e-> state > ALIVE){                    // animation de la mort du dragon : tourne sur lui-même
      if (e->state > REPLACED){     // jusqu'à quand on arrive à NEW_LIFE_DELAY et qu'il est remis en place 
	glRotatef((e->state) *20, 0, 0, 1);
	(e-> state)-= 1;
      }
    
    
    
//   if (e-> state == ITEMIZED){ // SI PAS DRAGON, DEAD_STAGE reste à 2
  //    if (e-> drag)
//	(e->state) = ALIVE;
//  }
      
      else if (e-> state == REPLACED){  // on remet le dragon en place avant sa "nouvelle vie", permet d'éviter qu'il ne meure si un ennemi était à sa place de naissance.
	if (e-> drag){
	  place_dragon();
	  e-> state --;
	}
	else  e-> texture_id = tex_bad_dead;
      }

      else e-> state --;
 }

#endif



    glColor3f(1.0F, 1.0F,1.0F); 

    
    glEnable(GL_TEXTURE_2D);
    
    glBindTexture (GL_TEXTURE_2D, e->texture_id);
    
    glBegin(GL_QUADS);

  
    glTexCoord2f(invert_texture, 1);
    glVertex2f(-ENTITY_SIZE/2,ENTITY_SIZE/2) ;
  
    glTexCoord2f(1 - invert_texture , 1);
    glVertex2f(ENTITY_SIZE/2,ENTITY_SIZE/2) ;
  
    glTexCoord2f(1 - invert_texture, 0);
    glVertex2f(ENTITY_SIZE/2, -ENTITY_SIZE/2) ;
  
    glTexCoord2f(invert_texture, 0);
    glVertex2f(-ENTITY_SIZE/2,-ENTITY_SIZE/2) ;
 

  
   glEnd() ;
    glPopMatrix();
        glDisable(GL_TEXTURE_2D);


	



	
}
 
void jump_drag(){
if (entities -> jump_stage == 0 && entities -> state == ALIVE)
  jump_entity(entities);
}



void jump_entity(Entity *e){
  if (isOnSolidGround(e)){
  //  if ((isWall(e->x, e->y) && isWall(e->x, e->y+CELL_SIZE-1))/* || (e->bubbledTo != NULL)*/){ // Jumps only if on solid ground. It avoids to let the dragon "fly" by jumping continually.
    //    e->bubbledTo = NULL;
    e -> jump_stage = JUMP;
  } 
}
      
    
/* void move_drag(){ */
/*   move_entity(entities); */
/* } */

bool touch_entity(Entity *e, Entity *e2){
  return (((e2->x + ENTITY_SIZE/2 > e -> x) &&  (e2->x - ENTITY_SIZE/2 < e -> x))
	   
	  && ((e2->y + ENTITY_SIZE/2 >  e -> y) && (e2->y  < e -> y + ENTITY_SIZE/2)));
    
    
    
}



bool touch_bubble(Entity *e, Bubble *b){


  

  return ((b->x + ENTITY_SIZE/2 > e -> x) &&  (b->x - ENTITY_SIZE/2 < e -> x))
	   
    && ((b->y + ENTITY_SIZE >=  e -> y) && (b->y   <= e -> y)); // ici e->y et pas b->y

}

Bubble* touched_by_bubble(Entity *e){
  Bubble* bubble_chaser = entities->bubbles;
  while (bubble_chaser != NULL && (!touch_bubble(e, bubble_chaser) || bubble_chaser->lifespan == 0 || e->state != ALIVE || (!bubble_chaser->empty && e->bubbledTo != bubble_chaser)))
    bubble_chaser = bubble_chaser -> next;

  if (bubble_chaser != NULL)
    bubble_chaser -> empty = false;
  return bubble_chaser;
    
}



bool touch_any_entity(Entity *e, EntityList l){
  bool touch = false;
  while(l != NULL){
    if (e != l && l->state != DEAD){
      if (touch_entity(e, l))
	touch = true;



    }
    
    l = l->next;
  }
   return touch;
}

void get_bonus(Entity* e){
  e->state == DEAD; // ennemi disparait completement
  entities -> score += e->score;
}


void kill_dragon(void){
  

  
  //  entities -> dead = true;
    //faire tourner dragon
  entities-> state = AGONY;
  (entities -> lives)--;
  if ((entities -> lives) == 0){
    
    game_over();
    /* init_entities(); */
    //    entities -> score = 0;
    //    enemies_alive = 0;

    init_entities();
    clean_bubbles(entities);
    entities -> lives = NUMBER_LIVES;
    
  }
}

void kill_enemy(Entity* e){
  e->bubbledTo->lifespan = 0;
  e->bubbledTo = NULL;
  e->jump_stage = JUMP;
  e->state = AGONY;
  e->score = 100;
}
			









void move_entity(Entity *e){
  //  if (!(e->drag) && e->state == ALIVE)
      if ( e->state == ALIVE)
	e->bubbledTo = touched_by_bubble(e);
  
      if (e->bubbledTo == NULL) {
	if (e->state == ALIVE ) { // Ne déplace pas l'entité si elle est dans une bulle : c'est la bulle qui se déplace et l'entité prend ses coordonnées.
	  if (e->moving && e-> state < NEW_LIFE_DELAY){ // empeche de pouvoir deplacer le perso pendant qu'il meurt .. 10 est le délai une fois qu'on la replacé avant qu'il puisse de nouveau être atteint.
	    if (e->direction == 'R'){
	      //	  if ((isWall(e-> x + ENTITY_SIZE/2, e->y -1) || touch_any_entity(e, entities)) && !(e->drag)){
	      if (isWall((e-> x + ENTITY_SIZE/2) +2, e->y -1)){
		// si ennemi atteint mur, il repart dans l'autre sens
		if (!e->drag)
		  e->direction = 'L';
		//e->x -= 10;
	      }
	  
	      else    e->x += MOVE_H;
	    }
		    	    
	
	    else if (e->direction == 'L'){
	      if (isWall((e-> x - ENTITY_SIZE/2) -2, e-> y -1)){
		if (!e->drag)
		  e->direction = 'R';
	      }
	      else{
		e-> x -= MOVE_H;
	      }
	    }
	  }
	}
  }

	  
/* #if 0 */
/* 	  //	  	  if (!isWall(e-> x - ENTITY_SIZE/2, e-> y -1)){ */
/* 	  if (!isWall(e-> x - ENTITY_SIZE/2, e-> y -1)){ */
/* 	    //	    if (!touch_any_entity(e, entities) || e->drag) */
/* 	      	e->x -=2; */
/* 	  } */
/* 	  else if (!(e-> drag)){ */
/* 	    e-> x += 10; */
/* 	    e->direction ='R'; */
/* 	  } */
/* 	} */


/*       } */
/*     } */
/*   } */
/* #endif   */
  else { 
 
    e-> x = e-> bubbledTo -> x;
    e-> y = e-> bubbledTo -> y + ENTITY_SIZE/2; //+ENTITY_SIZE/2
  }

}





void test_collisions(Entity* e){
                           
  if (touch_entity(e, entities)){ // si enemi touche dragon 
    if (e->bubbledTo == NULL){  // si ennemi pas dans bulle
      if( entities->state == ALIVE){ // si ennemi pas dans bulle touche le dragon
	if (e->state == ALIVE)       // si ennemi en vie ==> dragon meurt
	  kill_dragon();
	else if (e->state == REPLACED){ // si ennemi est transforme en ITEM bonus ==> dragon chope les points. //original ITEMIZED
	  get_bonus(e);
	  e->state = DEAD;
	  enemies_alive -= 1;
	}
      }
	      
	      
    } else /* if (e->state == ALIVE)*/{   // si dans bulle, c'est lui qui meurt!
      kill_enemy(e);
		 
    }

  }
}
	    

    
    

/* void gravity_drag(){ */
/*   gravity(dragptr); */

/* } */

bool isOnSolidGround(Entity* e){
  bool wall_under_right, wall_under_left, on_top_wall_left, on_top_wall_right; // those booleans to check if we are "in the air" or "on the ground"

  int x_corner_bottom_left = e->x - ENTITY_SIZE/2;
  int x_corner_bottom_right = e->x + ENTITY_SIZE/2;
  wall_under_right = isSolid(x_corner_bottom_left, e->y);                // check if the left side of the entity is on solid surface
  on_top_wall_right = isSolid(x_corner_bottom_left, e->y + CELL_SIZE - 1); // check that we are not "inside" the wall but on top of it. Avoids to get stuck in the wall
  wall_under_left = isSolid(x_corner_bottom_right, e->y);               // same for the right side
  on_top_wall_left = isSolid(x_corner_bottom_right, e->y + CELL_SIZE - 1);  //

  
  
  return  (wall_under_right && on_top_wall_right) || (wall_under_left && on_top_wall_left);
}
    
void gravity(Entity *e){
  /* This function which runs continuously checks if a jump have been initiated
     and effectively perform it by updating entity's position if necessary. 
    If not, it checks if entity is on a solid ground. Entity falls otherwise.
  */

  
  // To pass through the "holes" in border walls
  if (e-> y < CELL_SIZE)
    e-> y = WINDOW_HEIGHT ;//-CELL_SIZE;
  else if (e-> y > WINDOW_HEIGHT){
    e->y = CELL_SIZE+ MOVE_V;
  }


  
  if(e->jump_stage > 0){
    if (!isWall(e->x, e->y - ENTITY_SIZE) 
	&& !isWall(e->x - ENTITY_SIZE/2, e->y - ENTITY_SIZE)
	&& !isWall(e->x + ENTITY_SIZE/2, e->y-ENTITY_SIZE)){
    e->y -= MOVE_V;
    e->jump_stage--;
    } else e-> jump_stage = 0;
  }
  // Gravity doesn't apply during the jump, avoids to substract and add the same thing
		       
  else if (!isOnSolidGround(e))
     e->y += MOVE_V;


}

    

char get_lives(){
  
  return ('0' + entities->lives);
 }


int get_score(){

   return entities-> score;
}

void launch_bubble(){

   if (entities->direction == 'R'){   
     if (!isWall(entities->x + ENTITY_SIZE, entities->y -1) && !isWall(entities->x +ENTITY_SIZE*1.5, entities->y -1)) // Evite de lancer une bulle quand trop pres du mur
	 add_bubble(entities);
   }else if (entities->direction == 'L')
     if (!isWall(entities->x - ENTITY_SIZE, entities->y -1) && !isWall(entities->x -ENTITY_SIZE*1.5, entities->y -1)) // Meme chose de l'autre cote
	 add_bubble(entities);
}




void add_bubble(Entity *e){

  Bubble *new_bubble;
  new_bubble = malloc(sizeof(*new_bubble));
  if (new_bubble == NULL){
    printf("Memory allocation error");
    exit(EXIT_FAILURE);
  }
  new_bubble -> next = e->bubbles;
  new_bubble -> y = e->y - ENTITY_SIZE/2;
  
  int bubble_x = e->x;

  
  if (e-> direction == 'R'){
    bubble_x += ENTITY_SIZE;
  }
  else {
    bubble_x -= ENTITY_SIZE;
  
  }

  new_bubble->x = bubble_x;
  new_bubble -> lifespan = BUBBLE_LIFESPAN;
  new_bubble -> direction = e-> direction;
  new_bubble -> empty = true;
  e->bubbles = new_bubble;
}
   
   


 void display_move_bubbles(Bubble *b){
 
 
   while (b != NULL){
     if (b->lifespan > 0){
       display_bubble(b);
       move_bubble(b);
       }
     b = b-> next;     // on va voir la suivante
   }
}
 
   

 
 
void move_bubble(Bubble *b){
   if (b -> lifespan > BUBBLE_LIFESPAN / 2){
     if (b-> direction == 'L'){
       if (!isWall (b->x-ENTITY_SIZE/2 - 5, b->y)){
	   b->x -= 1 ;
	 }
     }
     else if (!isWall (b->x+ENTITY_SIZE/2 + 5, b->y)){
       b-> x += BUBBLE_H;
     }
     
     b->lifespan --;
   }
   else if (b-> lifespan > 0){
     //     b-> y -= 5;
     if (!isWall(b->x, b->y - ENTITY_SIZE/2))
       b -> y -= BUBBLE_V;
     b->lifespan --;
       }

     // To pass through the "holes" in border walls
  if (b-> y < CELL_SIZE)
    b-> y = WINDOW_HEIGHT ;//-CELL_SIZE;
  else if (b-> y > WINDOW_HEIGHT){
    b->y = CELL_SIZE+ BUBBLE_V;
  }

}


 void delete_bubble(){
   
 }

 
void display_bubble(Bubble *b){

  /* retrouver la source */
  glPushMatrix();
  glTranslatef(b->x, b->y, 0);
    
   
  float x,y;
  float radius = ENTITY_SIZE/2;
  glBegin(GL_LINES);
  glColor3f(1.0f,1.0f,1.0f);
     
  x = (float)radius * cos(359 * PI/180.0f);
  y = (float)radius * sin(359 * PI/180.0f);
  for(int j = 0; j < 360; j++)

    {
      glVertex2f(x,y);
      x = (float)radius * cos(j * PI/180.0f);
      y = (float)radius * sin(j * PI/180.0f);
      glVertex2f(x,y);
    }
  glEnd();
  glPopMatrix();
}
 
