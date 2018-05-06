#ifndef __TGA__SMALL__H__
#define __TGA__SMALL__H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif






// Cette fonction crée une texture à partir d'une string contenant le chemin vers une image au format TGA. Elle renvoie un GLuint, identifiant de la texture utilisable par GLUT.
GLuint loadTGATexture (const char *filename); 








#endif
