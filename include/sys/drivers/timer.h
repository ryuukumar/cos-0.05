#ifndef TIMER_H
#define TIMER_H

typedef struct timer_t {
	uint64_t time;
	void (*func)(void);
};

typedef struct time_det {
	uint32_t hr;
	uint8_t min;
	uint8_t sec;
	uint8_t tick;
};

extern uint64_t get_ticks();
extern void timer_wait (int);
extern int start_timer(int, void (*)());
extern time_det get_time();

#endif
