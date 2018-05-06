#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdio.h>
#include <stdbool.h>
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

#define BUBBLE_LIFESPAN 100
#define NUMBER_LIVES 3

#define NEW_LIFE_DELAY 30  // le nombre de tours entre le moment où le joueur est replacé après la "mort" et avant qu'il puisse être atteint par les ennemis
#define DEATH_STAGES 100 // length of the agony of the entity.

#define AGONY DEATH_STAGES       
#define REPLACED NEW_LIFE_DELAY
#define ITEMIZED 2
#define DEAD -1
#define ALIVE 0


#define START_POINTS 10000
#define POINTS_DECREASE 5
#define MIN_POINTS 200

#define PI 3.14

#define ENTITY_SIZE 40

#define JUMP 30

#define MOVE_V CELL_SIZE/4
#define MOVE_H CELL_SIZE/8

#define BUBBLE_V CELL_SIZE/4

#define BUBBLE_H 1// CELL_SIZE/8


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
  int jump_stage;// permet de savoir si le personnage est en train de sauter, et si oui, ou il en est dans le saut
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

typedef struct{
  GLuint tex_bad_dead;
  GLuint tex_bad;
  EntityList entities;
  Entity *dragon; // simple pointeur vers le dragon (idem que entities, mais permet de s'y retrouver)
  int enemies_alive;
  int number_enemies;
  int difficulty;
}Store;

/* -------------------------------- */

static Store store; // C'est ici que nous rangerons toutes les variables globales du module











EntityList add_entity(EntityList l, bool isDragon, GLuint texture_id);


void randomize_entity(Entity *e);
void gravity(Entity *e);
bool isOnSolidGround(Entity* e);

void init_entity(Entity* e);
bool touch_entity(Entity *e, Entity *e2);
void kill_dragon(void);

bool touch_any_entity(Entity *e, EntityList l);
void display_entity(Entity *e);
void move_entity(Entity *e);
void jump_entity(Entity *e);
void score_down(Entity *e);

void place_dragon(void);
void add_bubble(Entity *e);
void display_move_bubbles(Bubble *b);
void move_bubble(Bubble *b);
void display_bubble(Bubble *b);
void clean_bubbles(Entity *e);
void clean_entities(EntityList l);

void test_collisions(Entity* e);
void entity_jump_ia(Entity *e);
void init_entity_store(void);
void set_direction_ia(Entity *e);





void set_number_enemies(int n){
  /* Recrée les entités du jeu avec n (entier positif) ennemis.
   */
  store.number_enemies = n;
  clean_entities(store.entities);
  create_entities();
}


int get_enemies_alive(void){
  /* Fonction d'interface qui renvoit le nombre d'ennemis encore en vie
   */
  return store.enemies_alive;
}
  


void create_entities(){
  /* Cree la liste chainee contenant le dragon et les ennemis
   */
  if (store.number_enemies < 1 && store.number_enemies > 20)
    store.number_enemies = DEFAULT_NUMBER_ENEMIES;
			
  store.enemies_alive = 0;
  store.entities = NULL;

  store.tex_bad = loadTGATexture("images/enemy.tga");
  GLuint tex_drag = loadTGATexture("images/drag.tga");
  store.tex_bad_dead = loadTGATexture("images/banana.tga");


  for (int i = store.number_enemies; i > 0; i--){
   
    store.entities = add_entity(store.entities, false, store.tex_bad);
 
  }
  store.entities = add_entity(store.entities, true, tex_drag); // ajoute le dragon
  store.dragon = store.entities;
  init_entities();

}




char get_random_direction(int enemy_number){
  /* Renvoie une direction aléatoire.
   */

  char direction;
  if (enemy_number % 2 == 0){
    direction = 'R';
  } else direction = 'L';
 
  return direction;
}


EntityList add_entity(EntityList l, bool isDragon, GLuint texture_id){
  /* Ajoute une entité (dragon ou ennemi) à la liste 
   */
  Entity *new_entity;
  new_entity = malloc(sizeof(*new_entity));
  if (new_entity == NULL){
    printf("Memory allocation error");
    exit(EXIT_FAILURE);
  }

  new_entity -> score = 0;
  new_entity -> state = DEAD;
  new_entity -> drag = isDragon;
  new_entity -> bubbles = NULL;
  new_entity -> bubbledTo = NULL;
  new_entity -> texture_id = texture_id; 
  new_entity -> next = l; 

  return new_entity;
}


void init_entities(){
  /* Appelle init_entity() pour toutes les entités de la liste
   */
  
  Entity *l = store.entities -> next;
  while (l != NULL){
    init_entity(l);
    l = l->next;
  }
  init_entity(store.dragon); // termine par le dragon, evite le risque de supprimer une bulle pointee par un ennemi.
  
}

void init_entity(Entity* e){
  /* Remet l'entite dans son etat initial entre les niveaux ou parties
   */
  static int enemy_number = 0;


  e -> direction = get_random_direction(enemy_number);
  e -> jump_stage = 0;
  e -> moving = true;
  //e -> drag = isDragon;

  //  e -> texture_id = texture_id;
  //  e -> next = l;

  e -> bubbledTo = NULL;

  if (e->drag){
    e -> direction = 'R';
    if (e -> bubbles != NULL){
      clean_bubbles(e);
      e -> bubbles = NULL;
    }

    if (e-> state != ALIVE){
      e -> state = ALIVE;

    }
    e -> x = DRAGON_START_X;
    e -> y = DRAGON_START_Y;

      
    if (e -> lives == 0)
      e-> lives = NUMBER_LIVES; // evite de rajouter des vies entre les niveaux
    e -> direction = 'R';
    e -> moving = false;
  } else {
    enemy_number = (enemy_number + 1) % store.number_enemies;
    e -> texture_id = store.tex_bad;
    e -> x = ENEMY_START_X + ENTITY_SIZE * enemy_number ; // décale les ennemis
    e -> y = ENEMY_START_Y;
    e -> score = START_POINTS;

    if (e->state == DEAD){

      store.enemies_alive += 1;
    }
    e->state = ALIVE;
    
  }
}

void set_difficulty(int choice){
  store.difficulty = choice;
}

void selected_ia(Entity *e){
  if (store.difficulty == 0){
    randomize_entity(e);
  }
  else if (store.difficulty == 1)
    {
      entity_jump_ia(e);
      set_direction_ia(e);
    }
}

void set_direction_ia(Entity *e){
  if  (store.dragon->state == ALIVE && e-> y == store.dragon->y){
    if (e->direction == 'R'  &&
	e-> x > store.dragon->x)
  
      e->direction = 'L';
    else if (e->direction == 'L' &&
	     e-> x < store.dragon->x)

      e->direction = 'R';
  }
}


void randomize_entity(Entity *e){
  /* Fait sauter l'entité de façon aléatoire 
   */
  int set_jump;
  set_jump = rand() % 100;
  if (set_jump == 1)
    jump_entity(e);
}
      


bool any_platform_over(int x, int y){
  /*
   * Determine si il y a une platforme au dessus d'une position
   */

  bool any_platform = false;
  for (int an_y = y-1; !any_platform && an_y > y -100; an_y--){
    if (isPlatform(x, an_y)){
      any_platform = true;
    }
  }
  return any_platform;
  
}

void entity_jump_ia(Entity *e){
  /* Fait sauter l'entité si en dessous d'une plateforme et Bubble est 
     au dessus
  */
  if (any_platform_over(e->x, e->y) &&
      store.dragon->y + 2*ENTITY_SIZE < e->y &&
      e->x - store.dragon->x < 100){
    // if (entities->y + ENTITY_SIZE < e->y){
    jump_entity(e);
  }
  
}


void set_direction_drag(char direction){
  /* Fonction d'interface qui assigne la direction du dragon
   */
  store.dragon->direction = direction;

}



void set_moving_drag(bool moving){
  /* Fonction d'interface qui met le dragon en mouvement
   */
  store.dragon->moving = moving;
}
/* ---------------------- */


void place_dragon(void){
  /* Replace le dragon à sa position initiale.
   */
  
  store.dragon -> y = DRAGON_START_Y;
  store.dragon -> x = DRAGON_START_X;

}

void score_down(Entity *e){
  /*
    Gère la baisse de la "valeur" d'un ennemi tout au long d'un niveau : 
    plus on tue l'ennemi tard, moins il vaudra de points.
  */
  if (e->score > MIN_POINTS){ // 
    if (e->state == ALIVE)
      e->score -= POINTS_DECREASE;
  }
}


void manage_entities(void){
  /* Cette fonction appelle toutes les fonctions necessaires a l'affichage et au deplacement 
     des entites ainsi que les interactions entre elles.
  */
  Entity *l = store.entities;
  while (l != NULL){
    if ((l->state != DEAD)){
      display_entity(l);
      gravity(l);
      move_entity(l);


      if (!(l->drag)){
	score_down(l);
	test_collisions(l);
	if (l->state == ALIVE)
	  selected_ia(l);
      }
      
    }
    
    l = l-> next;
  }
}



void display_entity(Entity *e){
  /* Fonction qui gère l'affichage d'une entite, en fonction de son etat 
     contenu dans la variable "state", et de sa direction.
  */

  if (e->bubbles != NULL){             // Appel de l'affichage des bulles
    display_move_bubbles (e->bubbles);
  }
    
  int invert_texture;           // pour que le personnage soit orienté vers là où il se déplace 
  if (e->direction == 'L')
    invert_texture = 1;
  else invert_texture = 0;


    
  glPushMatrix();

  glTranslatef(e->x, e->y - (ENTITY_SIZE/2),0); // coord = au milieu en bas de l'entité



  if (e-> state > ALIVE){                    // animation de la mort du dragon : tourne sur lui-même
    if (e->state > REPLACED){                // jusqu à NEW_LIFE_DELAY où il est remis en place 
      glRotatef((e->state) *20, 0, 0, 1);
      (e-> state)-= 1;
    }
    
    
    
      
    else if (e-> state == REPLACED){  // REPLACED a deux utilisations differentes selon le type d'entite : dragon ou ennemi
      if (e-> drag){                  // on remet le dragon en place avant sa "nouvelle vie", permet d'éviter qu'il ne meure si un ennemi était à sa place de naissance.
	place_dragon();
	e-> state --;                 // Le dragon ne reste pas toujours en etat REPLACED, il ne fait qu'y passer, contrairement a l'ennemi mort 
      }
      else  e-> texture_id = store.tex_bad_dead;  // Pour les ennemis, cela correspond au moment où il est transformé en ITEM bonus.
      // Pas de décrément puisqu'il reste dans cet etat tant qu'il n'a pas été "mangé".
    }

    else e-> state --;
  }


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
  /* Fonction d'interface qui fait sauter le dragon 
     destinée à être liée à la touche clavier ad hoc.
  */ 
  if (store.dragon -> jump_stage == 0 && store.dragon -> state == ALIVE)
    jump_entity(store.entities);
}



void jump_entity(Entity *e){
  /*  Initialise le saut d'une entité
   */
  if (isOnSolidGround(e)){ // Jumps only if on solid ground. It avoids to let the dragon "fly" by jumping continually.
    e -> jump_stage = JUMP;
  } 
}
      
    




bool touch_entity(Entity *e, Entity *e2){
  /* Determine si deux entites se heurtent
   */
  
  return (((e2->x + ENTITY_SIZE/2 > e -> x) &&  (e2->x - ENTITY_SIZE/2 < e -> x))
	   
	  && ((e2->y + ENTITY_SIZE/2 >  e -> y) && (e2->y  < e -> y + ENTITY_SIZE/2)));
}



bool touch_bubble(Entity *e, Bubble *b){
  /* Determine si les coordonnees d'une entite correspondent a celles d'une bulle
   */
  return ((b->x + ENTITY_SIZE/2 > e -> x) &&  (b->x - ENTITY_SIZE/2 < e -> x))
	   
    && ((b->y + ENTITY_SIZE >=  e -> y) && (b->y   <= e -> y));

}


Bubble* touched_by_bubble(Entity *e){
  /* Cette fonction appelée a chaque tour de boucle principale du jeu
     verifie si une entité est touchée par une bulle et dans les conditions pour
     "rentrer" dedans, auquel cas elle renvoie un pointeur vers cette bulle, qui 
     sera assigné à l'entité
     
  */
  Bubble* bubble_chaser = store.dragon->bubbles;
  while (bubble_chaser != NULL && (!touch_bubble(e, bubble_chaser)   // on continue si la bulle ne touche pas l'entite
				   || bubble_chaser->lifespan == 0   // ou si la bulle est "morte" (invisible)
				   || e->state != ALIVE              // ou si l'entité est morte ou en train de mourir
				   || (!bubble_chaser->empty && e->bubbledTo != bubble_chaser))) // ou si la bulle a deja un "passager" qui n'est pas l'entité courante
    bubble_chaser = bubble_chaser -> next;

  if (bubble_chaser != NULL)  // si on assigne la bulle a cette entite, elle n'est plus vide
    bubble_chaser -> empty = false;
  return bubble_chaser;
    
}



bool touch_any_entity(Entity *e, EntityList l){
  /* Détermine si une entité touche une autre entité dans une liste
   */
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

void eat_enemy(Entity* e){
  /* Action lorsque le dragon attrape un ennemi transformé en ITEM bonus : 
     l'ennemi disparaît, et le dragon attrape les points.
  */
  e->state = DEAD; // ennemi disparait completement
  store.dragon -> score += e->score;
  store.enemies_alive -= 1;
}


void kill_dragon(void){
  /* Dragon perd une vie
   */

  //faire tourner dragon
  store.dragon -> state = AGONY;
  (store.dragon -> lives)--;
  if ((store.dragon -> lives) == 0){
    
    game_over();
    store.dragon -> lives = NUMBER_LIVES;
    
  }
}

void kill_enemy(Entity* e){
  /* Lance l'agonie d'un ennemi
   */
  e->bubbledTo->lifespan = 0;
  e->bubbledTo = NULL;
  e->jump_stage = JUMP;
  e->state = AGONY;
  //  e->score = 100;
}
			


void move_entity(Entity *e){
  /* Gère le déplacement d'une entité à chaque tour de boucle du jeu
   */
  
  //  if (!(e->drag) && e->state == ALIVE)
  if ( e->state == ALIVE)
    e->bubbledTo = touched_by_bubble(e);
  
  if (e->bubbledTo == NULL) {// Ne déplace pas l'entité si elle est dans une bulle : c'est la bulle qui se déplace et l'entité prend ses coordonnées.
    if (e->state == ALIVE ) { 
      if (e->moving && e-> state < NEW_LIFE_DELAY){ // empeche de pouvoir deplacer le perso pendant qu'il meurt ..
	//NEW_LIFE_DELAY est le délai une fois qu'on la replacé avant qu'il puisse de nouveau être atteint.
	if (e->direction == 'R'){
	  //	  if ((isWall(e-> x + ENTITY_SIZE/2, e->y -1) || touch_any_entity(e, entities)) && !(e->drag)){
	  if (isWall((e-> x + ENTITY_SIZE/2) +2, e->y -1)){
	    // si ennemi atteint mur, il repart dans l'autre sens
	    if (!e->drag)
	      e->direction = 'L';
	  }
	  
	  else    {e->x += MOVE_H;

	  }
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

	  
  else { 
 
    e-> x = e-> bubbledTo -> x;
    e-> y = e-> bubbledTo -> y + ENTITY_SIZE/2; //+ENTITY_SIZE/2
  }

}





void test_collisions(Entity* e){

  /* Teste et traite les différentes collisions possibles entre entre dragon et ennemis
     
   */
                           
  if (touch_entity(e, store.dragon)){ // si enemi touche dragon 
    if (e->bubbledTo == NULL){  // si ennemi pas dans bulle
      if( store.dragon->state == ALIVE){ // si ennemi pas dans bulle touche le dragon
	if (e->state == ALIVE)       // si ennemi en vie ==> dragon meurt
	  kill_dragon();
	else if (e->state == REPLACED){ // si ennemi est transforme en ITEM bonus ==> dragon chope les points.
	  eat_enemy(e);


	}
      }
	      
	      
    } else {   // si ennemi dans bulle, c'est lui qui meurt!
      kill_enemy(e);
		 
    }

  }
}
	    

    
    





bool isOnSolidGround(Entity* e){
  /* This function checks if we are "in the air" or "on the ground"
   */

  int x_corner_bottom_left = e->x - ENTITY_SIZE/2;
  int x_corner_bottom_right = e->x + ENTITY_SIZE/2;

  
  bool wall_under_right = isSolid(x_corner_bottom_left, e->y);                // check if the left side of the entity is on solid surface
  bool on_top_wall_right = isSolid(x_corner_bottom_left, e->y + CELL_SIZE - 5); // check that we are not "inside" the wall but on top of it. Avoids to get stuck in the wall
  bool wall_under_left = isSolid(x_corner_bottom_right, e->y);               // same for the right side
  bool on_top_wall_left = isSolid(x_corner_bottom_right, e->y + CELL_SIZE - 5);  //

  bool on_top_center = isSolid(e-> x, e->y + CELL_SIZE-5);
  bool wall_under_center = isSolid(e->x, e->y);
  
  return  (wall_under_right && on_top_wall_right) || (wall_under_left && on_top_wall_left) || (on_top_center && wall_under_center);
}
    
void gravity(Entity *e){
  /* This function which runs continuously checks if a jump have been initiated
     and effectively perform it by updating entity's position if necessary. 
     If not, it checks if entity is on a solid ground. Entity falls otherwise.
  */

  if (e-> bubbledTo == NULL){ // No gravity if in a bubble

    
    // To pass through the "holes" in border walls
    if (e-> y < CELL_SIZE)
      e-> y = WINDOW_HEIGHT ;//-CELL_SIZE;
    else if (e-> y > WINDOW_HEIGHT){
      e->y = CELL_SIZE+ BUBBLE_V;
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

  }else (e-> y = e -> bubbledTo-> y + ENTITY_SIZE/2);


}

    

char get_lives(){
  /* Fonction d'interface qui retourne le nombre de vies restantes du dragon
   */
  
  return ('0' + store.dragon->lives);
}


int get_score(){
  /* Fonction d'interface qui retourne le score du dragon
   */
  
  return store.dragon-> score;
}

void reset_score(){
  store.dragon-> score = 0;
}

void launch_bubble(){
  /* Lance une bulle
   */
  if (store.dragon->direction == 'R'){   
    if (!isWall(store.dragon->x + ENTITY_SIZE, store.dragon->y -1) && !isWall(store.dragon->x +ENTITY_SIZE*1.5, store.dragon->y -1)) // Evite de lancer une bulle quand trop pres du mur
      add_bubble(store.dragon);
  }else if (store.dragon->direction == 'L')
    if (!isWall(store.dragon->x - ENTITY_SIZE, store.dragon->y -1) && !isWall(store.dragon->x -ENTITY_SIZE*1.5, store.dragon->y -1)) // Meme chose de l'autre cote
      add_bubble(store.entities);
}




void add_bubble(Entity *e){
  /* Ajoute une bulle à la liste de bulles d'une entite
   */

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
  /* Appelle l'affichage et le déplacement de toutes les bulles d'une liste de bulles
   */
 
  while (b != NULL){
    if (b->lifespan > 0){
      display_bubble(b);
      move_bubble(b);
    }
    b = b-> next;     // on va voir la suivante
  }
}
 
   

 
 
void move_bubble(Bubble *b){
  /* Gère le déplacement d'une bulle. Durée déterminée par BUBBLE_LIFESPAN 
     Première moitié du déplacement horizontal puis vertical.
  */
  if (b -> lifespan > BUBBLE_LIFESPAN / 2){ // MOUVEMENT HORIZONTAL
    if (b-> direction == 'L'){
      if (!isWall (b->x-ENTITY_SIZE/2 - 5, b->y)){
	b->x -= BUBBLE_H ;
      }
    }
    else if (!isWall (b->x+ENTITY_SIZE/2 + 5, b->y)){
      b-> x += BUBBLE_H;
    }
     
    b->lifespan --;
  }
  else if (b-> lifespan > 0){ // MONTEE DE LA BULLE

     
    if (!isWall(b->x, b->y - ENTITY_SIZE/2))
      b -> y -= BUBBLE_V;

     
    // Pour traverser les "trous" dans les murs exterieurs
    if (b-> y < CELL_SIZE)
      b-> y = WINDOW_HEIGHT ;//-CELL_SIZE;
    else if (b-> y > WINDOW_HEIGHT){
      b->y = CELL_SIZE;
    }

    b->lifespan --;
  }
}


void clean_bubbles(Entity *e){
  /* Supprime toutes les bulles attachées à une entité
   */
  Bubble *l = e -> bubbles;
  
  while (l != NULL){
    fprintf(stderr, "Cleaning bubble : %p\n", l);
    Bubble *t = l->next;
    l = l-> next;
    free(t);
  }
}


void clean_entities(EntityList l){
  /* Supprime toutes les bulles attachées aux entités
   */

  
  while (l != NULL){
    fprintf(stderr, "Cleaning entity : %p\n", l);
    if (l->drag)
      clean_bubbles(l);
    Entity *t = l->next;
    l = l-> next;
    free(t);
  }
}

void clean(){
  clean_entities(store.entities);
}

 
void display_bubble(Bubble *b){
  /* 

   Pour le dessin des ronds en GLUT, source : https://cboard.cprogramming.com/c-programming/57934-drawing-circle-using-glut.html
   */
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
 
