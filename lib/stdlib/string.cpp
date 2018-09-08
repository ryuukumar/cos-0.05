#include <stddef.h>
#include <stdint.h>

#include "../../include/string.h"
 
size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

const char* itos (uint64_t i, uint32_t b) {
	static char r[]= "0123456789ABCDEF";
	static char buf[100]; 
	char *ptr; 
		
	ptr = &buf[99]; 
	*ptr = 0; 	
	do { 
		*--ptr = r[i%b]; 
		i /= b; 
	} while(i != 0); 
	
	return(ptr); 
}
