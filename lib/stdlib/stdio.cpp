#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "../../include/stdio.h"
#include "../../include/stdutil.h"
#include "../../include/string.h"
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
 
size_t trow;
size_t tcolumn;
uint8_t tcolor;
uint16_t* tbuffer;
 
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
	return fg | bg << 4;
}
 
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}
 
void tinitialize(void) {
	trow = 0;
	tcolumn = 0;
	tcolor = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	tbuffer = (uint16_t*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			tbuffer[index] = vga_entry(' ', tcolor);
		}
	}
}
 
void tsetcolor(uint8_t color) {
	tcolor = color;
}
 
void tputentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	tbuffer[index] = vga_entry(c, color);
}
 
void putchr(char c) {
	if (c == '\n') {tcolumn = 0; if (trow + 1 != VGA_HEIGHT) trow++; else trow = 0;}
	else {
		tputentryat(c, tcolor, tcolumn, trow);
		if (++tcolumn == VGA_WIDTH) {
			tcolumn = 0;
			if (++trow == VGA_HEIGHT)
				trow = 0;
		}
	}
}
 
void twrite(const char* data, size_t size) {
	for (size_t i = 0; i < size; i++)
		putchr(data[i]);
}
 
void twritestr(const char* data) {
	twrite(data, strlen(data));
}

void setc (uint32_t n) {
	tsetcolor(skim32to8(n));
}

void setfc (uint8_t n) {
	tcolor = (tcolor & 0xF0) + (n & 0xF);
}

void setbc (uint8_t n) {
	tcolor = ((n & 0xF) * 0x10) + (tcolor & 0xF);
}

void print_str (const char* str) {
	twritestr(str);
}

void print_ch (char ch) {
	putchr(ch);
}

size_t getCol () {
	return tcolumn;
}

size_t getRow () {
	return trow;
}

void tgoto (size_t r, size_t c) {
	trow = r;
	tcolumn = c;
	
	return;
}

extern "C" uint32_t printf (const char* str, ...) {
	if(!str) return 0;
	va_list	args;
	va_start (args, str);
	size_t i;
	for (i=0; i<strlen(str);i++) {
		switch (str[i]) {
			case '%':
				switch (str[i+1]) {
					/*** characters ***/
					case 'c': {
						int c = va_arg (args, int);
						print_ch (c);
						i++;		// go to next character
						break;
					}
					/*** address of ***/
					/*case 's': {
						const char* c = (const char*) va_arg (args, int);
						//char str[64]; const char strp = str;
						//*strp = (const char*) c;
						print_str (c);
						i++;		// go to next character
						break;
					}*/			//broken...
					/*** integers ***/
					case 'd':
					case 'i': {
						int c = va_arg (args, int);
						char strsrc[32]={0}; const char* str = strsrc;
						str = itos (c, 10);
						print_str (str);
						i++;		// go to next character
						break;
					}
					/*** display in hex ***/
					case 'x': {
						int c = va_arg (args, int);
						char strsrc[32]={0}; const char* str = strsrc;
						str = itos (c, 16);
						print_str (str);
						i++;		// go to next character
						break;
					}

					/*** custom: change color - %c[x] OR %c[xy] ***/
					case 'C': {
						int c = 0;
						if (str[i+4] == ']') c = str[i+3] - '0';
						else c = ((str[i+3] - '0')*10) + str[i+4] - '0';
						setc(c);
						i += 4;
						if (str[i+1] == ']') i++;
						break;
					}
					
					/*** custom: long number (uint64_t) ***/
					case 'l': {
						uint64_t c = va_arg (args, uint64_t);
						char strsrc[100] = {0}; const char* str = strsrc;
						str = itos(c, 10);
						print_str (str);
						i++;		// go to next character
						break;
					}
					default:
						va_end (args);
						return 1;
				}
				break;
			default:
				print_ch (str[i]);
				break;
		}
	}
	va_end (args);
	return i;
}

void clr() {
	tgoto(0,0);
	for (int i = 0; i < 80*25; i++) printf(" ");
	tgoto(0,0);
}
