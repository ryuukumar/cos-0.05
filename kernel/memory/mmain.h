#ifndef MMAIN_H
#define MMAIN_H

#include <sys/multiboot.h>

#ifdef __cplusplus
extern "C" void* memcpy(void*, const void*, size_t);
extern "C" void* memset(void*, char, size_t);
extern "C" unsigned char inb (unsigned short);
extern "C" void outb (unsigned short, unsigned char);
#else
extern void* memcpy(void*, const void*, size_t);
extern void* memset(void*, char, size_t);
extern unsigned char inb (unsigned short);
extern void outb (unsigned short, unsigned char);
#endif

extern bool initMem(multiboot_info_t*);
extern bool isLocFree (uint64_t*);
extern uint64_t getMemSize (void);

#ifdef mpmng_user
extern int getnUnBlocks (void);
extern multiboot_mmap_entry* getUnBlock (int);
#endif

#endif
