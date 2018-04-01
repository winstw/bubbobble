#include <stdio.h>
#include <GL/glut.h>
#include <string.h>
#include "entity.h"
#include "map.h"
#include "graphics.h"
#define GAME_OVER_DELAY 100
#define BUTTON_WIDTH 300
#define BUTTON_HEIGHT WINDOW_HEIGHT/7.4 
#define TIMER_DELAY 25
#define NEW_LEVEL_DELAY 30
//#define DEFAULT_FONT GLUT_BITMAP_HELVETICA_18
#define DEFAULT_FONT GLUT_BITMAP_TIMES_ROMAN_24
#define DEFAULT_FONT_HEIGHT 18
#define TIMER_SCREEN_DELAY 100



typedef enum{
  MENU,
  PLAYING,
  GAME_OVER,
  INSTRUCTIONS,
  SHORTCUTS,
  CHOOSE_LEVEL,
  PAUSE
}State;
  


static State game_state = MENU;                      
static int game_slide = WINDOW_HEIGHT;// MAP_HEIGHT*CELL_SIZE;
 static int level_delay = NEW_LEVEL_DELAY;
//static int game_over_delay = GAME_OVER_DELAY;
static int screen_delay = TIMER_SCREEN_DELAY;


void pause();
void display_status(void);
void play(void);
void menu_game(void);
void game(bool pause);
void game_over(void);
void game_over_screen(void);
int display_string(void *font, char *string, int pos_x, int pos_y);
int length_string(char *string);
void timer_screen(char *string);


int display_centered_string(char *string, int pos_y){
  
  
  /* Affiche un texte centré horizontalement à l'écran 
     dans la police par défaut DEFAULT_FONT
  */
  
 return  display_string(DEFAULT_FONT, string, WINDOW_WIDTH/2 - length_string(string)/2, pos_y);
}


int length_string(char *string){
  
  /* Retourne la longueur d'un chaine dans la police par default DEFAULT_FONT
   */

  
  int longueur = 0;
  for (int i= 0; string[i] != 0; i++){
    longueur += glutBitmapWidth(DEFAULT_FONT, string[i]);
  }

  return longueur;
}

int display_string(void *font, char *string, int pos_x, int pos_y){

  /* Affiche un texte a l'ecran 
   */
  
  int pos_x_original = pos_x;
  for (int i = 0; string[i] != '\0'; i++){
    glRasterPos2i(pos_x, pos_y);
    glutBitmapCharacter(font, string[i]);
    pos_x = pos_x + glutBitmapWidth(font, string[i]);
    if (string[i] == '\n'){
      pos_y += glutBitmapWidth(font, 'A')+5;
      pos_x = pos_x_original;
    }
		    
  }
  return pos_x;
     


}

void init(){
  
  /* cette fonction sert à initialiser le jeu, elle sert à appeler
     toutes les fonctions qui ne doivent être exécutées qu'une seule fois
  */
  
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable (GL_DEPTH_TEST);

  reset_level();
  create_entities();
  //  init_wall_tex();
  

}

void display(void){
  /* Cette fonction est la boucle principale du programme, relancée en
     permanence par render_game(). En fonction de la variable
     game_state, elle donne le contrôle à une partie du programme ou
     une autre.
  */

  glClearColor(0.0F,0.0F,0.0F,0.0F) ;
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT) ;


  switch (game_state){  // boucle de jeu
      case PLAYING:
	game(false);
	break;
      case PAUSE:
	game(true);
	break;
      case MENU:
	menu_game();
	break;
      case GAME_OVER:
	timer_screen("GAME OVER");
	//	game_over_screen();
      	break;
      case INSTRUCTIONS:
	//timer_screen(INSTRUCTIONS);
	break;
  default:
    menu_game();
    break;

  }
    glFlush() ;
}




void display_level(void){
  /* Affiche à l'écran le niveau à chaque passage vers un supérieur
   */
 
  char level[sizeof(int) * 4 + 1];
  sprintf(level, "%d", get_level());
  int pos_x = WINDOW_WIDTH / 2;
  pos_x =  display_string(DEFAULT_FONT, " LEVEL ", pos_x, 300);

 
 pos_x =  display_string(DEFAULT_FONT, level, pos_x, 300);

  
}




void game(bool pause){
  /* cette fonction contient le "jeu" à proprement parler, c'est ici
     que l'on appelle à chaque tour de display() les différentes
     fonctions du jeu : afficher la carte, les personnages, afficher
     le statut.
  */


  if (getEnemiesAlive() == 0){ // Ennemis tous morts et "mangés" ==> niveau suivant
    
    init_entities();
    next_level();
    level_delay = NEW_LEVEL_DELAY;
  }


  
  if(game_slide > 0){ // Permet l'effet de glissement au début de la partie
    game_slide -= 10;
  }


  
  glColor3f(1.0F, 1.0F,1.0F);
  glEnable(GL_TEXTURE_2D);

  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();

  if (level_delay > 0){ // Affichage du niveau à chaque nouveau
    display_level();
    level_delay -= 1;
  }

  
  glTranslatef(0, game_slide, 0);
  draw_map(); 


  if (!pause){
    display_status();
    display_entities();
  }
  else {
    glColor3f(1, 1, 1);
    display_centered_string("PAUSE - 'P' POUR REPRENDRE", 100); //18 est la hauteur de la police
  }

}



void display_status(void){
  /* sous fonction nécessaire à game() qui affiche les informations
     sur le jeu en cours : SCORE, VIES RESTANTES, NIVEAU.
  */
  
  int pos_x;

  char score[sizeof(int) * 4 + 1];
  sprintf(score, "%d", get_score());

  char level[sizeof(int) * 4 + 1];
  sprintf(level, "%d", get_level());
  
 
  display_string(DEFAULT_FONT, "VIES : ", 0, DEFAULT_FONT_HEIGHT); //18 est la hauteur de la police
  glutBitmapCharacter(DEFAULT_FONT, get_lives());
 
  pos_x = display_centered_string(" SCORE : ", DEFAULT_FONT_HEIGHT);
  pos_x =  display_string(DEFAULT_FONT, score, pos_x, DEFAULT_FONT_HEIGHT);

  pos_x =  display_string(DEFAULT_FONT, " LEVEL : ", WINDOW_WIDTH - 120, DEFAULT_FONT_HEIGHT);
  pos_x =  display_string(DEFAULT_FONT, level, pos_x, DEFAULT_FONT_HEIGHT);

}








void timer_screen(char *msg){
  /*  Affiche un écran quand le joueur a perdu une partie
   */
  if (screen_delay == 0)
    game_state = MENU;


  
  glColor3f(0.0F, 0.0F,1.0F);
  display_centered_string(msg, WINDOW_HEIGHT/3); //18 est la hauteur de la police
  screen_delay--;

}




void menu_game(){
  /* Affiche le menu du jeu, c'est par ici qu'on commence, et qu'on
     revient après chaque partie.
  */
  int pos_y = BUTTON_HEIGHT * 0.2;
    glColor3f(0.0F, 1.0F,1.0F);
    
  for (int i = 0; i < 6 ; i ++){
    glPushMatrix();
    glTranslatef(WINDOW_WIDTH/2 - BUTTON_WIDTH/2, pos_y, 0);

    glBegin(GL_QUADS);
    glVertex2f(0,  0);
    glVertex2f(BUTTON_WIDTH, 0);
    glVertex2f(BUTTON_WIDTH,BUTTON_HEIGHT);
    glVertex2f(0, BUTTON_HEIGHT);
    glEnd();
    glPopMatrix();
    pos_y += BUTTON_HEIGHT* 1.2;

  }

  glColor3f(0.0F, 0.0F,1.0F);
  pos_y = BUTTON_HEIGHT * 0.8;

  char *button_messages[6] = {"PRESS 'P' TO PLAY GAME",
			"INSTRUCTIONS",
			"CHOOSE DIFFICULTY",
			"SHORTCUTS",
			"RULES",
			"PRESS ESC TO EXIT",
			
  };

  
  for (int i = 0; i < 6; i++){

    display_centered_string(button_messages[i], pos_y);  
    pos_y += BUTTON_HEIGHT* 1.2;
    
  }

}


void handleResize(int w, int h) {
  /*
    gère les coordonnées
  */
  	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D (0, w, h, 0) ;

}


void special_keys_down(int key, int x, int y){
  /* Enregistre les touches NON-ASCII enfoncées.
   */

  /* https://www.opengl.org/resources/libraries/glut/spec3/node49.html */
  switch(key){
    
  case GLUT_KEY_RIGHT:
    set_direction_drag('R');
    set_moving_drag(true);
    /* dragptr->x += 5; */

    break;
  case GLUT_KEY_LEFT:
    set_direction_drag('L');
    set_moving_drag(true); 

    break;              
  }
  
}

void special_keys_up(int key, int x, int y){
  /* Enregistre les touches NON-ASCII relâchées.
doc : http://www.dei.isep.ipp.pt/~matos/cg/docs/manual/glutKeyboardUpFunc.3GLUT.html
  */

  switch (key){

  case GLUT_KEY_RIGHT:
    set_moving_drag(false);
    break;

  case GLUT_KEY_LEFT:
    set_moving_drag(false);
    break;
  }
}



void keyboard_down(unsigned char key, int x, int y){
  /* Fonction propre à glut, qui enregistre les touches ASCII enfoncées.
   */
  switch(key){

  case '\n':
    if (game_state == GAME_OVER)
      game_state = MENU;
    break;
       case ' ':
    jump_drag();
    break;

  case 'p':
    if (game_state == MENU)
      game_state = PLAYING;
    else if (game_state == PLAYING)
      game_state = PAUSE;
    else if (game_state == PAUSE)
      game_state = PLAYING;
	
    break;

  case 'f':
    launch_bubble();
    break;
    
  case 27:
    if (game_state == MENU)
        exit(0);
    if (game_state == GAME_OVER)
      game_state = MENU;

    
  }
}

void keyboard_up(unsigned char key, int x, int y){
  /* Fonction propre à glut, qui enregistre les touches ASCII relachées.


  http://www.dei.isep.ipp.pt/~matos/cg/docs/manual/glutKeyboardUpFunc.3GLUT.html
 */
  switch(key){
  }
    

}





void timer_game(int time){/* opengldoc.pdf*/
  /* Cette fonction est celle qui gère le "timing" du programme, le
     délai entre chaque itération de display().
  */
  

    glutPostRedisplay();
    glutTimerFunc(TIMER_DELAY, timer_game, 0);

}



 void game_over(void){
   /* Fonction appelée quand le joueur a perdu une partie.  C'est ici
      qu'on nettoie et qu'on réinitialise les entités pour une prochaine partie.
    */

   game_state = GAME_OVER;

   
   game_slide = WINDOW_HEIGHT;
   screen_delay = TIMER_SCREEN_DELAY;

   reset_level();

 }
