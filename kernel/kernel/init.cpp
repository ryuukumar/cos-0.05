#include <stddef.h>
#include <stdint.h>

#include <sys/multiboot.h>
#include <stdio.h>
#include <mmain.h>
#include <mpmng.h>
#include <mvmng.h>

#include <sys/drivers/cursor.h>

extern "C" void kernel_main(multiboot_info_t*);
extern "C" void A20_Enable();
extern "C" void gdt_install();
extern "C" void idt_install();
extern "C" void irq_install();
extern "C" void pic_install();
extern void pit_install();
extern void tinitialize();

extern "C" void init(multiboot_info_t* bootinfo) {	

	disable_cursor();

	// initialize important stuff
	//A20_Enable();
	gdt_install();
	idt_install();
	irq_install();
	
	tinitialize();
	initMem(bootinfo);
	
	pit_install();
	init_pm();
	init_vm();
	
	// call the kernel
	kernel_main(bootinfo);
	
	// after the kernel returns, halt
	printf("All processes ended. Suspended OS.");
	__asm__ ("cli");
	while(1);
	
}
