#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <mmain.h>
#include <mpmng.h>
#include <sys/sys.h>
#include <sys/multiboot.h>

#include <sys/drivers/timer.h>
#include <sys/drivers/cursor.h>

extern void tinitialize();
extern void twritestr(const char*);
extern "C" void gdt_install();
extern "C" gdt_ptr get_gdt_ptr();
extern uint64_t get_ticks();

extern void beep();

gdt_ptr gdtP;
time_det time;

extern "C"
void kernel_main(multiboot_info_t* bootinfo) {

	enable_cursor(14, 15);
	update_cursor(18, 14);

	__asm__ ("sti\n\t");

	gdtP = get_gdt_ptr();
	uint32_t gdtbase = gdtP.base;		//18446744073185380393
	uint16_t gdtlimit = gdtP.limit;		//57413
 
	/* Do stuff */
	int a = 10;
	printf("\n\n    %C[9]Cos 0.05-v1\n    %C[7]Welcome to nothingness!\n\n\n\n    %C[8]Stuff will appear here very soon...%C[7]\n\n    gdt-base: %i, gdt-limit: 0x%x", gdtbase, gdtlimit);
	
	int* lalala = (int*) alloc_block_p();
	*lalala = 789;
	
	printf("\n    m_block size: %i\n    Allocated space for 'lalala', value: %i, addr: 0x%x", sizeof(m_block), *lalala, (int) lalala);
	
	free_block_p((int)lalala);
	printf("\n    Freed 'lalala', value is now %i", *lalala);
	
	//tgoto(0,0); printf("                                                     \n                                                     ");
	tgoto(18, 4); printf("t: 00:00");

	while(1) {
		tgoto(18,7);
		if (time.sec != get_time().sec) {
			if (get_time().hr != 0) printf("%i:", get_time().hr);
			if (get_time().min < 10) printf("0%i:", get_time().min);
			else printf("%i:", get_time().min);
			if (get_time().sec < 10) printf("0%i", get_time().sec);
			else printf("%i", get_time().sec);
			time = get_time();
		}
	};
}
