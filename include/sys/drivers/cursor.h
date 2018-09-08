#ifndef CURSOR_H
#define CURSOR_H

extern void enable_cursor (uint8_t, uint8_t);
extern void disable_cursor (void);
extern void update_cursor (int, int);

extern int get_cursor_row();
extern int get_cursor_col();

#endif
