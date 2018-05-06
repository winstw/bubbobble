#ifndef __GRAPHICS__H__
#define __GRAPHICS__H__

void init(void);
void display(void);
void handleResize(int w, int h);
void special_keys_down(int key, int x, int y);
void special_keys_up(int key, int x, int y);
void keyboard_down(unsigned char key, int x, int y);
void keyboard_up(unsigned char key, int x, int y);
void timer_game(int time);
void game_over(void);






#endif
