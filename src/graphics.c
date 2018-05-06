#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


#include <stdio.h>
#include <string.h>
#include "entity.h"
#include "map.h"
#include "graphics.h"

#define BUTTON_WIDTH 300

#define BUTTON_HEIGHT WINDOW_HEIGHT / ((MAIN_MENU_ITEMS * 1.2) + 0.2) // 1/5 DE HAUTEUR DE BOUTON ENTRE CHACUN ET ENTRE BOUTON ET BORDS
#define DEFAULT_FONT GLUT_BITMAP_HELVETICA_18
#define FONT_HEIGHT 18

#define TIMER_FUNC_DELAY 25
#define NEW_LEVEL_DELAY 30
#define GAME_OVER_DELAY 100
#define TIMER_SCREEN_DELAY 100

#define MAIN_MENU_ITEMS 6
#define NUMBER_DIFFICULTIES 2



typedef enum{
  MENU,
  PLAYING,
  GAME_OVER,
  GAME_WON,
  INSTRUCTIONS,
  ENEMIES,
  DIFFICULTY,
  SHORTCUTS,
  CHOOSE_LEVEL,
  PAUSE
}State;


typedef struct{
  int difficulty_choice;
  int menu_choice;
  State game_state;
  int game_slide;
  int level_delay;
  int screen_delay;
  int score;
  int number_enemies;
}Store;


void menu_game(int menu_choice);
void game(bool pause);
void game_over_screen(void);

void init_graphics_store();

void display_status(void);
//void play(void);
void game_over(void);



int display_string(void *font, char *string, int pos_x, int pos_y);
int display_centered_string(char *string, int pos_y);
int length_string(char *string);
void timer_screen(char *string);
void game_won(void);
void shortcuts_screen(void);
void instructions_screen(void);
void menu_selector(int choice);
void difficulty_screen(int selector);
void enemies_screen(int selector);



static Store store; // C'est dans cette structure globale que nous stockerons toutes les variables d'état du jeu.



void init_graphics_store(){
  /*
    Initialisation de toutes les variables d'état
    nécessaires au jeu (contenues dans le "store")
  */
  store.game_state = MENU;                      
  store.game_slide = WINDOW_HEIGHT;
  store.level_delay = NEW_LEVEL_DELAY;
  store.screen_delay = TIMER_SCREEN_DELAY;

  store.difficulty_choice = 0;
  store.menu_choice = 0;
  store.number_enemies = DEFAULT_NUMBER_ENEMIES;
  store.score = 0;
  
}
		

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
  
  /* Cette fonction sert à initialiser le jeu, elle sert à appeler
     toutes les fonctions qui ne doivent être exécutées qu'une seule fois
  */
  
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable (GL_DEPTH_TEST);
  
  init_graphics_store();
  reset_level();

  set_difficulty(store.difficulty_choice);
  set_number_enemies(store.number_enemies);
  create_entities();
  //  init_wall_tex();
  
}

void display(void){
  
  /* Cette fonction est la boucle principale du programme, relancée en
     permanence par render_game(). En fonction de la variable
     store.game_state, elle donne le contrôle à une partie du programme ou
     une autre.
  */

  glClearColor(0.0F, 0.0F,0.0F,0.0F) ;
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT) ;


  switch (store.game_state){  // boucle de jeu
  case PLAYING:
    game(false);
    break;
  case PAUSE:
    game(true);
    break;
  case MENU:
    menu_game(store.menu_choice);
    break;
  case GAME_OVER:
    timer_screen("GAME OVER");
    //	game_over_screen();
    break;
  case GAME_WON:
    timer_screen("CONGRATULATIONS, YOU FINISHED THE GAME!");
    break;
  case INSTRUCTIONS:
    instructions_screen();
    break;
  case SHORTCUTS:
    shortcuts_screen();
    break;
  case DIFFICULTY:
    difficulty_screen(store.difficulty_choice);
    break;
  case ENEMIES:
    enemies_screen(store.number_enemies);
    break;
  default:
    menu_game(store.menu_choice);
    break;

  }
  glFlush() ;
}


void difficulty_screen(int selector){
  /*  Affichage de l'écran de choix de la difficulté (choix stocké dans le store).
   */
  int pos_y = WINDOW_HEIGHT/3;
  glColor3f(0.0F, 0.0F,1.0F);
  display_centered_string(" CHOISISSEZ UN NIVEAU DE DIFFICULTE : ", pos_y);

  char *difficulty_levels[20] = { "FACILE",
				  "DIFFICILE"
  };
  for (int i = 0; i < 2; i++){
    if (i == selector){
      glColor3f(1.0F, 0.0F,0.0F);
    }else   glColor3f(0.0F, 0.0F,1.0F);
    pos_y += 30;
    display_centered_string(difficulty_levels[i], pos_y);     
  }
  
}
void enemies_screen(int selector){
  /*  Affichage de l'écran de choix de la difficulté (choix stocké dans le store).
   */
  int pos_y = WINDOW_HEIGHT/3;
  int pos_x = WINDOW_WIDTH/4;
  glColor3f(0.0F, 0.0F,1.0F);
  display_centered_string(" NOMBRE D'ENNEMIS : ", pos_y);

  /* char *difficulty_levels[20] = { "1", */
  /* 			      "DIFFICILE" */
  /* }; */
  pos_y += 30;
  for (int i = 1; i <= MAX_ENEMIES ; i++){
    if (i == selector){
      glColor3f(1.0F, 0.0F,0.0F);
    }else   glColor3f(0.0F, 0.0F,1.0F);
    pos_x += 30;
    char nombre[2];
    sprintf(nombre, "%c", '0' + i);
    display_string(DEFAULT_FONT, nombre, pos_x, pos_y);     
  }
  
}



void display_level(void){
  
  /* Affiche à l'écran le niveau à chaque passage vers un supérieur
   */


  glColor3f(1.0F, 1.0F,1.0F);
    
  char level[sizeof(int) * 4 + 1];
  sprintf(level, "%d", get_level());
  int pos_y = WINDOW_HEIGHT / 6;
  int pos_x =  display_centered_string(" LEVEL ", pos_y);

 
  pos_x =  display_string(DEFAULT_FONT, level, pos_x, pos_y);

  
}




void game(bool pause){
  /* cette fonction contient le "jeu" à proprement parler, c'est ici
     que l'on appelle à chaque tour de display() les différentes
     fonctions du jeu : afficher la carte, les personnages, afficher
     le statut.
  */


  if (get_level() > 0){
    
    if (get_enemies_alive() == 0){ // Ennemis tous morts et "mangés" ==> niveau suivant
    
      init_entities();
      next_level();
      store.level_delay = NEW_LEVEL_DELAY;
    }

  
    if(store.game_slide > 0){ // Permet l'effet de glissement au début de la partie
      store.game_slide -= 10;
    }


    if (store.level_delay > 0){ // Affichage du niveau
      display_level();
      store.level_delay -= 1;
    }
  
    glColor3f(1.0F, 1.0F,1.0F);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_MODELVIEW); 
    glLoadIdentity();

  
    glTranslatef(0, store.game_slide, 0);
    draw_map(); 


    if (!pause){
      display_status();
      manage_entities();
    }
    else {
      glColor3f(1, 1, 1);
      display_centered_string("PAUSE - 'P' POUR REPRENDRE", WINDOW_HEIGHT/4); 
      display_centered_string("ESC - QUITTER LA PARTIE", WINDOW_HEIGHT/4 + FONT_HEIGHT * 1.2);
    }

  
  } else game_won();

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
  
 
  display_string(DEFAULT_FONT, "VIES : ", 0, FONT_HEIGHT); //18 est la hauteur de la police
  glutBitmapCharacter(DEFAULT_FONT, get_lives());
 
  pos_x = display_centered_string(" SCORE : ", FONT_HEIGHT);
  pos_x =  display_string(DEFAULT_FONT, score, pos_x, FONT_HEIGHT);

  pos_x =  display_string(DEFAULT_FONT, " LEVEL : ", WINDOW_WIDTH - 120, FONT_HEIGHT);
  pos_x =  display_string(DEFAULT_FONT, level, pos_x, FONT_HEIGHT);

}


void instructions_screen(){
  /*
   * Affiche l'ecran avec les touches du jeu
   */
  glColor3f(0.0F, 0.0F,1.0F);
  int pos_y = WINDOW_HEIGHT/4;
  display_centered_string("DANS CE JEU, VOUS INCARNEZ UN DRAGON.", pos_y);
  pos_y += 50;
  display_centered_string("IL PEUT SAUTER, SE DEPLACER ET LANCER DES BULLES.", pos_y);
  pos_y += 50;

  display_centered_string("POUR CHAQUE NIVEAU, LE BUT EST DE MANGER TOUS LES ENNEMIS.", pos_y);
  pos_y += 30;
  display_centered_string("POUR CE FAIRE, VOUS DEVEZ LES CAPTURER DANS UNE BULLE,", pos_y);
  pos_y += 30;
  display_centered_string("PUIS FAIRE ECLATER LA BULLE EN LA TOUCHANT. ", pos_y);
  pos_y += 50;
  display_centered_string("VOUS DEVEZ ALORS MANGER LES ENNEMIS AINSI TRANSFORMES EN FRUIT.", pos_y);
  pos_y += 50;
  display_centered_string("ATTENTION SI VOUS TOUCHEZ UN ENNEMI HORS D'UNE BULLE, ",pos_y);
  pos_y += 30;
  display_centered_string("C'EST LUI QUI VOUS TUERA! VOUS POSSEDEZ AU DEPART TROIS VIES.", pos_y);
  pos_y += 50;
  display_centered_string("VOUS AVEZ EGALEMENT LA POSSIBILITE DE RENTRER ", pos_y);
  pos_y += 30;
  display_centered_string("DANS UNE DE VOS BULLES POUR VOUS DEPLACER.", pos_y);
  pos_y += 30;

  display_centered_string("", pos_y);

  pos_y += 100;
  
  display_centered_string("REVENIR AU MENU : ESC", pos_y);
  
}



void shortcuts_screen(){
  /* Affiche l'écran des raccourcis clavier
   */
  
  glColor3f(0.0F, 0.0F,1.0F);
  int pos_y = WINDOW_HEIGHT/3;
  display_centered_string("TOUCHES CLAVIER", pos_y);
  pos_y += 30;
  display_centered_string("LANCER UNE BULLE : F", pos_y);
  pos_y += 30;
  display_centered_string("SAUTER : ESPACE", pos_y);
  pos_y += 30;
  display_centered_string("DEPLACEMENT : GAUCHE/DROITE", pos_y);
  pos_y += 30;
  display_centered_string("PAUSE ET MENU : P", pos_y);

  pos_y += 100;
  
  display_centered_string("REVENIR AU MENU : ESC", pos_y);
  
}



void timer_screen(char *msg){
  /*  Affiche un écran quand le joueur a perdu une partie
   */
  if (store.screen_delay == 0)
    store.game_state = MENU;



  glColor3f(0.0F, 0.0F,1.0F);
  display_centered_string(msg, WINDOW_HEIGHT/3); //18 est la hauteur de la police
  // ici ajouter l'affichage du score
  char string_score[sizeof(int) * 4 + 1];
  sprintf(string_score, "SCORE : %d", store.score);
  
  display_centered_string(string_score, WINDOW_HEIGHT/3 + FONT_HEIGHT * 2);
  store.screen_delay--;

}




void menu_game(int choice){
  /* Affiche le menu du jeu, c'est par ici qu'on commence, et qu'on
     revient après chaque partie.
  */
  int pos_y = BUTTON_HEIGHT * 0.2;
  glColor3f(0.0F, 1.0F,1.0F);
    
  for (int i = 0; i < MAIN_MENU_ITEMS ; i ++){

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
  pos_y = BUTTON_HEIGHT * 0.8;

  char *button_messages[6] = {"NOUVELLE PARTIE",
			      "NOMBRE D'ENNEMIS",
			      "INSTRUCTIONS",
			      "DIFFICULTE",
			      "TOUCHES",
			      "QUITTER"
  };

  
  for (int i = 0; i < MAIN_MENU_ITEMS; i++){
    if (i == choice){
      glColor3f(1.0F, 0.0F,0.0F);
    }else   glColor3f(0.0F, 0.0F,1.0F);

    display_centered_string(button_messages[i], pos_y);  
    pos_y += BUTTON_HEIGHT* 1.2;
    
  }

}


void handleResize(int w, int h) {
  /*
    Gère le rapport entre l'affichage et les coordonnées 
  */
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D (0, w, h, 0) ;

}


void menu_up(int *menu_choice, int number_choices){
  /*
    Gère le bouclage entre différents éléments d'un menu vers le haut 
    (arrivé en haut, on recommence en bas).
  */
  if (*menu_choice > 0)
    *menu_choice = *menu_choice - 1;
  else *menu_choice = number_choices -1;


  
}
void menu_down(int *menu_choice, int number_choices){
  /*
    Gère le bouclage entre différents éléments d'un menu vers le bas 
    (arrivé en bas, on recommence en haut).
  */
  *menu_choice = (*menu_choice + 1) % number_choices;
}
	     


void special_keys_down(int key, int x, int y){
  /* Enregistre les touches NON-ASCII enfoncées.
   */

  /* https://www.opengl.org/resources/libraries/glut/spec3/node49.html */
  switch(key){
    
  case GLUT_KEY_RIGHT:
    if (store.game_state == ENEMIES)
      menu_down(&store.number_enemies, MAX_ENEMIES+1);
    else if (store.game_state == PLAYING){
      set_direction_drag('R');
      set_moving_drag(true);
    }


    break;
  case GLUT_KEY_LEFT:
    if (store.game_state == ENEMIES)
      menu_up(&store.number_enemies, MAX_ENEMIES+1);
    else if (store.game_state == PLAYING){
      set_direction_drag('L');
      set_moving_drag(true);
    }

    break;
  case GLUT_KEY_UP:
    if (store.game_state == MENU)
      menu_up(&store.menu_choice, MAIN_MENU_ITEMS);
    else if (store.game_state == DIFFICULTY)
      menu_up(&store.difficulty_choice, NUMBER_DIFFICULTIES);
    break;
  case GLUT_KEY_DOWN:
    if (store.game_state == MENU)
      menu_down(&store.menu_choice, MAIN_MENU_ITEMS);
    else if (store.game_state == DIFFICULTY)
      menu_down(&store.difficulty_choice, NUMBER_DIFFICULTIES);
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
  /* Fonction  qui enregistre les touches ASCII enfoncées.
   */
  switch(key){

  case '\n':
    if (store.game_state == GAME_OVER)
      store.game_state = MENU;
    break;
  case ' ':
    jump_drag();
    break;

  case 'p':
    if (store.game_state == MENU)
      store.game_state = PLAYING;
    else if (store.game_state == PLAYING)
      store.game_state = PAUSE;
    else if (store.game_state == PAUSE)
      store.game_state = PLAYING;
	
    break;

  case 'i':
    if (store.game_state == MENU)
      store.game_state = INSTRUCTIONS;
    break;

    
  case 'r':
    if (store.game_state == MENU)
      store.game_state = SHORTCUTS;
    break;
    
  case 'f':
    launch_bubble();
    break;
    
  case 27:
    if (store.game_state == MENU)
      exit(0);
    if (store.game_state == GAME_OVER || store.game_state == SHORTCUTS || store.game_state == INSTRUCTIONS)
      store.game_state = MENU;
    if (store.game_state == PAUSE){
      game_over();
    }
    break;
  case 13:

    if (store.game_state == MENU)
      menu_selector(store.menu_choice);
    else if (store.game_state == DIFFICULTY){
      set_difficulty(store.difficulty_choice);
      store.game_state = MENU;
    }
    else if (store.game_state == ENEMIES){
      set_number_enemies(store.number_enemies);
      store.game_state = MENU;
    }

    break;
    
    
  }
}

void keyboard_up(unsigned char key, int x, int y){
  /* Fonction  qui enregistre les touches ASCII relachées.


     http://www.dei.isep.ipp.pt/~matos/cg/docs/manual/glutKeyboardUpFunc.3GLUT.html
  */
  switch(key){
  }
    

}


void menu_selector(int choice){
  /* Execute l'action désirée en fonction du choix sur le menu principal
   */
  switch (store.menu_choice){
  case 0 : 
    store.game_state =PLAYING;
    break;
  case 1 :
    store.game_state = ENEMIES;
    break;
  case 2 :
    store.game_state = INSTRUCTIONS;
    break;
  case 3 :
    store.game_state = DIFFICULTY;
    break;
  case 4 :
    store.game_state = SHORTCUTS;
    break;
  case 5 :
    clean();
    exit(0);
    break;
  }
}




void timer_game(int time){  
  /* Cette fonction est celle qui gère le "timing" du programme, le
     délai entre chaque itération de display().
  */
  

  glutPostRedisplay();
  glutTimerFunc(TIMER_FUNC_DELAY, timer_game, 0);

}


void game_won(void){
  /* Gère le cas où le joueur a terminé tous les niveaux du jeu
   */
  store.game_state = GAME_WON;
  store.game_slide = WINDOW_HEIGHT;
  store.screen_delay = TIMER_SCREEN_DELAY;
  reset_level();

}



void game_over(void){
  /* Fonction appelée quand le joueur a perdu une partie.  C'est ici
     qu'on nettoie et qu'on réinitialise les entités pour une prochaine partie.
  */

  store.game_state = GAME_OVER;

   
  store.game_slide = WINDOW_HEIGHT;
  store.screen_delay = TIMER_SCREEN_DELAY;

  store.score = get_score();
  reset_level();
  init_entities();
  reset_score();

}
