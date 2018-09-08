#include <stddef.h>
#include <stdint.h>

#define mpmng_user

#include <mmain.h>
#include <mpmng.h>
#include <stdio.h>
#include <sys/drivers/timer.h>

/*#define N_SLOTS 1050000

m_block mem[N_SLOTS] = {{0, 0, false}};

bool init_started = false;
bool init_done = false;

uint32_t* malloc(int size) {
	if (!init_started) return NULL;
	for (uint64_t i = 0x700000; i < getMemSize(); i++) {
		if (!is_range_alloc(i, size)) {
			for (int j = 0; j < N_SLOTS; j++)
				if (!mem[i].occupied)
					mem[i] = {i, size, true};
			return (uint32_t*) i;
		}
	}
	return NULL;
}

bool alloc_s(uint32_t base, uint32_t len) {
	if (!init_started) return false;
	for (int i = 0; i < len; i++)
		if (is_alloc(base+len)) {printf("f"); return false;}
	
	for (int i = 0; i < N_SLOTS; i++) {
		if (!mem[i].occupied) {
			mem[i] = {base, len, true};
			return true;
		}
	}
	return false;
}

bool is_alloc(uint32_t addr) {
	if (!init_started) return true;
	
	for (int i = 0; i < N_SLOTS; i++) {
		if (mem[i].base <= addr && mem[i].base + mem[i].len >= addr && mem[i].occupied && mem[i].len != 0) return true;
	}
	return false;
}

bool is_range_alloc(uint32_t base, int len) {
	for (int i = 0; i < N_SLOTS; i++)
		for (int j = 0; j < len; j++) 
			if (mem[i].base <= base+j && mem[i].base + mem[i].len >= base+j && mem[i].occupied && mem[i].len != 0) return true;
	return false;
}

bool free(uint32_t base) {
	for (int i = 0; i < N_SLOTS; i++) {
		if (mem[i].base = base) {memset ((void*) base, 0, mem[i].len); mem[i] = {0, 0, false}; return true;}
	}
	return false;
}

void init_pm() {
	for (int i = 0; i < N_SLOTS; i++) mem[i] = {0, 0, false};
	mem[0] = {0, 0x700000, true};

	init_started = true;
	
	for (int i = 0; i < getnUnBlocks(); i++)
		mem[i+1] = {getUnBlock(i)->addr, getUnBlock(i)->size, true};		
	
	init_done = true;	
	return;
}*/




#define BLOCKS_PER_BYTE 8
#define BLOCK_SIZE 4096

uint32_t mem[32768] = {0};

void set_mem (int blockn) {mem[blockn/32] |= (1 << (blockn % 32));}
void free_mem (int blockn) {mem[blockn/32] &= ~ (1 << (blockn % 32));}
bool test_mem (int blockn) {return mem[blockn/32] & (1 << (blockn % 32));}

int first_free_mem () {
	for (int i = 0; i < 1048576; i++)
		if (!test_mem(i)) return i;
	return -1;
}
int first_free_s_mem(size_t s) {
	bool temp = false;
	for (int i = 0; i < 1048576-s; i++) {
		temp = true;
		for (int j = 0; j < s; j++)
			if (test_mem(i+j)) temp = false;
		if (temp) return i;
	}
	return -1;
}

void init_pm() {
	for (int i = 0; i < 1048576; i++) free_mem(i);
	for (int i = 0; i < 0x700000/4096; i++) set_mem(i);
	for (int i = 0; i < getnUnBlocks(); i++) {
		if (getUnBlock(i)->size % 0x1000 > 0)
			for (int j = 0; j < ((getUnBlock(i)->len)/0x1000) + 1; j++)
				set_mem(((getUnBlock(i)->addr)/0x1000)+j);
		else for (int j = 0; j < ((getUnBlock(i)->len)/0x1000); j++)
				set_mem(((getUnBlock(i)->addr)/0x1000)+j);
	}
}

void* alloc_block_p() {
	int alloc = first_free_mem();
	if (alloc == -1) {printf("%C[3][%i] %C[4]Fatal error:%C[7] No memory left to allocate. Halted.", get_ticks()); while(1);}
	set_mem(alloc);
	return (uint32_t*) alloc;
}

void* alloc_blocks_p(size_t s) {
	int alloc = first_free_s_mem(s);
	if (alloc == -1) {printf("%C[3][%i] %C[4]Fatal error:%C[7] No memory left to allocate. Halted.", get_ticks()); while(1);}
	for (;s > 0; s--) set_mem(alloc + s - 1);
	return (uint32_t*) alloc;
}

void free_block_p(uint32_t addr) {
	free_mem(addr/4096);
	memset((uint32_t*)addr, 0, BLOCK_SIZE);
}

void free_blocks_p(uint32_t addr, int len) {
	for (; len > 0; len--) free_mem((addr/4096) + len - 1);
}
