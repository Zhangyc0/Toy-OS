.extern trap_handler
.extern dummy
.extern do_timer

    .section .text.entry
    .align 2
    .globl _traps 
_traps:
    # YOUR CODE HERE
    # -----------
        # 1. save 32 registers and sepc to stack
        csrr t0, sscratch
        beqz t0, _store
        csrw sscratch, sp
        mv sp, t0
        #csrr t0, sepc
        #addi t0, t0, 4
        #csrw sepc, t0
_store:
        #la t0, current->trapframe
        #la sp, boot_stack_top
        addi sp, sp, -304
        sd x0, 0(sp) 
        sd x1, 8(sp)
        sd x2, 16(sp)
        sd x3, 24(sp)
        sd x4, 32(sp)
        sd x5, 40(sp)
        sd x6, 48(sp)
        sd x7, 56(sp)
        sd x8, 64(sp)
        sd x9,  72(sp)
        sd x10, 80(sp)
        sd x11, 88(sp)
        sd x12, 96(sp)
        sd x13, 104(sp)
        sd x14, 112(sp)
        sd x15, 120(sp)
        sd x16, 128(sp)
        sd x17, 136(sp)
        sd x18, 144(sp)
        sd x19, 152(sp)
        sd x20, 160(sp)
        sd x21, 168(sp)
        sd x22, 176(sp)
        sd x23, 184(sp)
        sd x24, 192(sp)
        sd x25, 200(sp)
        sd x26, 208(sp)
        sd x27, 216(sp)
        sd x28, 224(sp)
        sd x29, 232(sp)
        sd x30, 240(sp)
        sd x31, 248(sp)
        
        #csrrw s0, sscratch, x0
        csrr s1, sstatus
        csrr s2, sepc
        csrr s3, scause
        csrr s4, stval
        csrr s5, sscratch

        sd sp, 256(sp)
        sd s1, 264(sp)
        sd s2, 272(sp)
        sd s3, 280(sp)
        sd s4, 288(sp)
        sd s5, 296(sp)
    # -----------
        # 2. call trap_handler
        csrr a0, scause
        csrr a1, sepc
        mv a2, sp
        call trap_handler
        #call do_timer
    # -----------
        # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack
        ld s1, 264(sp)
        ld s2, 272(sp)
        ld s3, 280(sp)
        ld s4, 288(sp)
        csrw sstatus, s1
        csrw sepc, s2
        csrw scause, s3
        csrw stval, s4

        ld x0, 0(sp) 
        ld x1, 8(sp)
        ld x2, 16(sp)
        ld x3, 24(sp)
        ld x4, 32(sp)
        ld x5, 40(sp)
        ld x6, 48(sp)
        ld x7, 56(sp)
        ld x8, 64(sp)
        ld x9,  72(sp)
        ld x10, 80(sp)
        ld x11, 88(sp)
        ld x12, 96(sp)
        ld x13, 104(sp)
        ld x14, 112(sp)
        ld x15, 120(sp)
        ld x16, 128(sp)
        ld x17, 136(sp)
        ld x18, 144(sp)
        ld x19, 152(sp)
        ld x20, 160(sp)
        ld x21, 168(sp)
        ld x22, 176(sp)
        ld x23, 184(sp)
        ld x24, 192(sp)
        ld x25, 200(sp)
        ld x26, 208(sp)
        ld x27, 216(sp)
        ld x28, 224(sp)
        ld x29, 232(sp)
        ld x30, 240(sp)
        ld x31, 248(sp)

        ld sp, 256(sp)
        addi sp, sp, 304
        
        csrr t0, sscratch
        beqz t0, _return
        csrw sscratch, sp
        mv sp, t0
    # -----------
_return:
        # 4. return from trap
        sret
    # -----------
    .global ret_from_fork
ret_from_fork:
    ld x0, 0(a0)
    ld ra, 8(a0)
    ld sp, 16(a0)
    ld gp, 24(a0)
    ld tp, 32(a0)
    #ld t0, 40(a0)
    ld t1, 48(a0)
    ld t2, 56(a0)
    ld s0, 64(a0)
    ld s1, 72(a0)
    ld a1, 88(a0)
    ld a2, 96(a0)
    ld a3, 104(a0)
    ld a4, 112(a0)
    ld a5, 120(a0)
    ld a6, 128(a0)
    ld a7, 136(a0)
    ld s2, 144(a0)
    ld s3, 152(a0)
    ld s4, 160(a0)
    ld s5, 168(a0)
    ld s6, 176(a0)
    ld s7, 184(a0)
    ld s8, 192(a0)
    ld s9, 200(a0)
    ld s10, 208(a0)
    ld s11, 216(a0)
    ld t3, 224(a0)
    ld t4, 232(a0)
    ld t5, 240(a0)
    ld t6, 248(a0)
    ld t0, 264(a0)
    csrw sstatus, t0
    ld t0, 272(a0)
    csrw sepc, t0
    ld t0, 280(a0)
    csrw scause, t0
    ld t0, 40(a0)
    ld a0, 80(a0)
    
    #csrr t0, sscratch
    #beqz t0, _return
    #csrw sscratch, sp
    #mv sp, t0
    
    sret


    # -----------
    .global __dummy
__dummy:
    # YOUR CODE HERE
    #la t0, dummy
    li t0, 0
    csrw sepc, t0
    
    csrr t0, sscratch
    csrw sscratch, sp
    mv sp, t0

    sret
    # -----------

    # -----------
    .globl __switch_to
__switch_to:
    # save state to prev process
    # YOUR CODE HERE
    mv t0, a0 #t0=current
    #ld t0, 0(t0) //added
    addi t0, t0, 40
    #ld t0, 0(t0)

    sd ra, 0(t0)
    sd sp, 8(t0)
    
    addi t0, t0, 0x10
    sd s0, 0(t0)
    sd s1, 8(t0)
    sd s2, 16(t0)
    sd s3, 24(t0)
    sd s4, 32(t0)
    sd s5, 40(t0)
    sd s6, 48(t0)
    sd s7, 56(t0)
    sd s8, 64(t0)
    sd s9, 72(t0)
    sd s10, 80(t0)
    sd s11, 88(t0)
    #sepc
    csrr t1, sepc
    sd t1, 96(t0)
    #sstatus
    csrr t1, sstatus
    sd t1, 104(t0)
    #sscratch
    csrr t1, sscratch
    sd t1, 112(t0)
#pgd

    #switch the process
    la t0, current
    sd a1,0(t0)

    # restore state from next process
    # YOUR CODE HERE
    mv t0, a1 #t0=next
    #ld t0, 0(t0) //added
    addi t0, t0, 40
    #ld t0, 0(t0)
    
    ld ra, 0(t0)
    ld sp, 8(t0)

    addi t0, t0, 0x10
    ld s0, 0(t0)
    ld s1, 8(t0)
    ld s2, 16(t0)
    ld s3, 24(t0)
    ld s4, 32(t0)
    ld s5, 40(t0)
    ld s6, 48(t0)
    ld s7, 56(t0)
    ld s8, 64(t0)
    ld s9, 72(t0)
    ld s10, 80(t0)
    ld s11, 88(t0)
    #sepc
    ld t1, 96(t0)
    csrw sepc, t1
    #sstatus
    ld t1, 104(t0)
    csrw sstatus, t1
    #sscratch
    ld t1, 112(t0)
    csrw sscratch, t1
    #set satp
    ld t1, 120(t0)
    li t0, 0x8000000000000000
    li t2, 0xFFFFFFDF80000000
    sub t1, t1, t2
    srli t1, t1, 12
    add t0, t0, t1
    csrw satp, t0
    sfence.vma zero, zero
    
    
    ret

# ------------- 
