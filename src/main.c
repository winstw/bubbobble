#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include "keyboard.h"
#include "entity.h"
#include "graphics.h"
#include "map.h"
#include "tga_small.h"




int main(int argc, char *argv[]){

     /* init_wall_tex();  */



  
 

 
  glutInit(&argc,argv);
  
  glutInitDisplayMode(GLUT_SINGLE|GLUT_RGBA);

  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT); 
  glutInitWindowPosition(50,10);



  glutCreateWindow("BUBBLE BOBBLE");

  glutDisplayFunc(display);
  glutReshapeFunc(handleResize);
  glutSpecialFunc(special_keys_down);
  glutSpecialUpFunc(special_keys_up);
  glutKeyboardFunc(keyboard_down);
  glutKeyboardUpFunc(keyboard_up);
  glutTimerFunc(25, timer_game, 0);

  init(); 


  
  glutMainLoop();
   /* remove_front_list(bad_guys); */
  return 0;
}

  

