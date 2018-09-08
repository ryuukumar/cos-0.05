#include <stddef.h>
#include <stdint.h>

#include <sys/drivers/cursor.h>
#include <mmain.h>

int crow, ccol;

void enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3E0) & 0xE0) | cursor_end);
}

void disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void update_cursor(int row, int col) {
	uint16_t pos = row * 80 + col;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
	crow = row; ccol = col;
}

int get_cursor_row() {
	return crow;
}

int get_cursor_col() {
	return ccol;
}
