#include <stddef.h>
#include <stdint.h>

#include <mmain.h>
#include <stdio.h>

extern "C" void* memcpy(void *dest, const void *src, size_t count) {
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for(; count != 0; count--) *dp++ = *sp++;
	return dest;
}

extern "C" void* memset(void *dest, char val, size_t count) {
	char *temp = (char *)dest;
	for(; count != 0; count--) *temp++ = val;
	return dest;
}

extern "C" unsigned char inb (unsigned short _port) {
	unsigned char rv;
	__asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

extern "C" void outb (unsigned short _port, unsigned char _data) {
	__asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

//-------------------------------------------------------------------------------------------//

uint64_t usableMem = 0;
uint64_t totalMemSize = 0;

int unusableBlocks = 0;

uint32_t mmapLoc = 0;
uint32_t mmapSize = 0;

bool initMem(multiboot_info_t* info) {	
	multiboot_memory_map_t* mmap = (multiboot_mmap_entry*)info->mmap_addr;
	int mmap_addr = info->mmap_addr;
	int mmap_len = info->mmap_length;
	int counter = 1;
	
	while (mmap < (void*)(mmap_addr + mmap_len)) {
		if (mmap->type != 1) unusableBlocks++;
		if (mmap->type == 1 && mmap->len != 0) 
			usableMem += mmap->len;
		totalMemSize += mmap->len;
		
		counter++;
		mmap = (multiboot_memory_map_t*) ((unsigned int)mmap + mmap->size + sizeof(mmap->size));	//get next entry
	}
	
	usableMem -= info->mmap_length;
	
	mmapLoc = info->mmap_addr;
	mmapSize = info->mmap_length;
}

bool isLocFree (uint64_t* loc) {
	multiboot_memory_map_t* lmmap = (multiboot_mmap_entry*)mmapLoc;
	while (lmmap < (void*)(mmapLoc + mmapSize)) {
		if ((uint64_t*)(lmmap->addr + lmmap->len) > loc && (uint64_t*)lmmap->addr <= loc && lmmap->type == 1) return true;
		lmmap = (multiboot_memory_map_t*) ((unsigned int)lmmap + lmmap->size + sizeof(lmmap->size));	//get next entry
	}
	return false;
}

int getnUnBlocks () {
	return unusableBlocks;
}

multiboot_mmap_entry* getUnBlock (int n) {
	multiboot_memory_map_t* lmmap = (multiboot_mmap_entry*)mmapLoc;
	while (lmmap < (void*)(mmapLoc + mmapSize)) {
		if (lmmap->type != 1 && n == 0) return lmmap;
		if (lmmap->type != 1 && n != 0) n--;
		
		lmmap = (multiboot_memory_map_t*) ((unsigned int)lmmap + lmmap->size + sizeof(lmmap->size));	//get next entry
	}
}

uint64_t getMemSize (void) {return totalMemSize;}

