export
CROSS_=riscv64-unknown-elf-
GCC=${CROSS_}gcc
LD=${CROSS_}ld
OBJCOPY=${CROSS_}objcopy

ISA=rv64imafd
ABI=lp64

INCLUDE = -I $(shell pwd)/include -I $(shell pwd)/arch/riscv/include # -I $(shell pwd)/user
CF = -march=$(ISA) -mabi=$(ABI) -mcmodel=medany -fno-builtin -ffunction-sections -fdata-sections -nostartfiles -nostdlib -nostdinc -static -lgcc -Wl,--nmagic -Wl,--gc-sections 
CFLAG = -g ${CF} ${INCLUDE} -DSJF 

.PHONY:all run debug clean
all:
	${MAKE} -C user all
	${MAKE} -C fs all
	${MAKE} -C lib all
	${MAKE} -C init all
	${MAKE} -C arch/riscv all	
	@echo -e '\n'Build Finished OK

run: all
	@echo Launch the qemu ......
	@qemu-system-riscv64 -nographic -machine virt -kernel vmlinux -bios default -initrd simple_fs.cpio

debug: all
	@echo Launch the qemu for debug ......
	@qemu-system-riscv64 -nographic -machine virt -kernel vmlinux -bios default -initrd simple_fs.cpio -S -s

clean:
	${MAKE} -C user clean
	${MAKE} -C fs clean
	${MAKE} -C lib clean
	${MAKE} -C init clean
	${MAKE} -C arch/riscv clean
	$(shell test -f vmlinux && rm vmlinux)
	$(shell test -f System.map && rm System.map)
	@echo -e '\n'Clean Finished
