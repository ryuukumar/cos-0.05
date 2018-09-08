
RED:=\033[1;31m
GREEN:=\033[1;32m
GREEN_UNDERL:=\033[4;1;32m
L_BLUE:=\033[1;36m
L_PURPLE:=\033[1;35m
NC:=\033[0m # No Color

LINK_FILES:=kernel/kernel/init.o boot/boot.o kernel/kernel/kernel.o kernel/*.o lib/stdlib/*.cpp kernel/memory/rw_cr0_cr3.o kernel/memory/code/*.cpp kernel/drivers/*.o kernel/drivers/*.cpp

#	@echo " "
#	@echo "    $(GREEN_UNDERL)Nothing here yet!$(NC)"
#	@echo "    Come back later!"
#	@echo "    Cos $(L_BLUE)0.05-v1-b0$(NC)"
#	@echo " "

all:
	@i686-elf-as boot/boot.s -o boot/boot.o
	@nasm -f elf kernel/sys_asm.asm -o kernel/sys_asm.o
	@nasm -f elf kernel/A20.asm -o kernel/A20.o
	@nasm -f elf kernel/memory/rw_cr0_cr3.asm -o kernel/memory/rw_cr0_cr3.o
	@i686-elf-g++ -c kernel/kernel/kernel.cpp -o kernel/kernel/kernel.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I include -I kernel/memory
	@i686-elf-g++ -c kernel/kernel/init.cpp -o kernel/kernel/init.o -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-rtti -I include -I kernel/memory
	@i686-elf-gcc -c kernel/sys.c -o kernel/sys.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include
	@i686-elf-gcc -c kernel/drivers/fault_hndl.c -o kernel/drivers/fault_hndl.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I include -I kernel/memory
	@i686-elf-g++ -T kernel/linker.ld -o build/boot/cos.bin -ffreestanding -O2 -nostdlib $(LINK_FILES) -lgcc -I include -I kernel/memory
	@if grub-file --is-x86-multiboot build/boot/cos.bin; then \
  		true; \
		else \
  		echo "$'\n  $(RED)Sorry, something broke down.$'\n$(NC) If you want to know, it is a problem with the multiboot (Code M0)$'\n"; \
		fi
	@grub-mkrescue -o ISO/cos.iso build
	@echo "$(GREEN)Done.$(NC)"
	
run:
	@qemu-system-i386 -cdrom ISO/cos.iso -m 1G #-d cpu > log.txt

build-run: all run
#emptynessforlife
#depressed

#test:
#	@echo "Running $(RED)testmode$(NC)"
#	@echo "Cos $(L_BLUE)0.05-v1-b"
#	@while read $Line; do echo "$(Line)"; done < build
#	@echo "$(NC)"
