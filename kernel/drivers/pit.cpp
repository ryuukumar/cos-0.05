#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <sys/drivers/timer.h>
#include <sys/regs.h>
#include "../memory/mmain.h"

extern "C" void irq_install_handler(int irq, void (*handler)(struct regs *r));

time_det gtime = {0};
timer_t timer[20] = {0};

uint64_t get_ticks() {
	return (((((gtime.hr*60)+gtime.min)*60)+gtime.sec)*21)+gtime.tick;
}

void pit_handler(struct regs *r) {	
	gtime.tick++;
	if (gtime.tick == 21) {gtime.tick = 0; gtime.sec++;}
	if (gtime.sec == 60) {gtime.sec = 0; gtime.min++;}
	if (gtime.min == 60) {gtime.min = 0; gtime.hr++;}
	for (int i = 0; i < 20; i++) {
		if (timer[i].time != 0) timer[i].time--;
		if (timer[i].time == 0 && timer[i].func != NULL) {timer[i].func(); timer[i].func = NULL;}
	}
}

void pit_install() {
	irq_install_handler(0, pit_handler);
}

// help the below function
bool waiting_timer = false;
void timer_wait_help () {waiting_timer = true;}

// timer which forces everything but interrupts to wait
void timer_wait (int secs) {
	start_timer(secs*21, timer_wait_help);
	while(!waiting_timer) {printf("");};
	waiting_timer = false;
	return;
}

// timer which runs in the background
int start_timer (int time, void (*func) (void)) {
	for (int i = 0; i < 20; i++) {
		if (timer[i].time == 0) {
			timer[i] = {time, func};
			return i;
		}
	}
	return -1;
}

// sends time in hours, minutes and seconds
time_det get_time () {	
	return gtime;
}

// play sound using built in speaker
static void play_sound(uint32_t nFrequence) {
	uint32_t Div;
	uint8_t tmp;

	//Set the PIT to the desired frequency
 	Div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
	//And play the sound using the PC speaker
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
}
 
// make it shutup
static void nosound() {
 	uint8_t tmp = inb(0x61) & 0xFC; 
 	outb(0x61, tmp);
}
 
// make a beep
void beep() {
	play_sound(1000);
	timer_wait(5);
	nosound();
	//set_PIT_2(old_frequency);
}
