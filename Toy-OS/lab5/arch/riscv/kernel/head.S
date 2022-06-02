.extern start_kernel
.extern _traps
.extern early_pgtbl

    .section .text.init
    .globl _start
_start:
    la sp, boot_stack_top
    call setup_vm
    call relocate 
    call mm_init

    la t0, _traps
    csrw stvec, t0
#--------------------------
    # set sie[STIE] = 1
    csrr t0, sie
    ori t0, t0, 0x0000000000000020
    csrw sie, t0
#--------------------------
    # set first time interrupt
    rdtime t1
    li t0, 10000000
    add t1, t1, t0

    li a7, 0
    li a6, 0
    #a value that is not comfirmed
    
    mv a0, t1
    li a1, 0
    li a2, 0
    li a3, 0
    li a4, 0
    li a5, 0
    ecall
#--------------------------
    # set sstatus[SIE] = 1
    #csrr t0, sstatus
    #ori t0, t0, 0x0000000000000002
    #csrw sstatus, t0
# -------------------------
    
    call setup_vm_final
    call task_init
    j start_kernel

relocate:
    li t0, 0xFFFFFFDF80000000
    add ra, ra, t0
    add sp, sp, t0
    #set satp with early_pgtbl
    li t0, 0x8000000000000000
    #add t0, t0, t1
    la t1, early_pgtbl
    srli t1, t1, 12
    add t0, t0, t1
    csrw satp, t0
    
    sfence.vma zero, zero
    
    ret

    .section .bss.stack
    .globl boot_stack
boot_stack:
    .space 0x1000, 0x0

    .globl boot_stack_top
boot_stack_top:
