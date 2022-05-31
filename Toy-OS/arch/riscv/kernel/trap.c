// trap.c 
#include"printk.h"
#include"clock.h"
#include"proc.h"
#include"syscall.h"
#include"vm.h"
#include"defs.h"
extern struct task_struct* current;
extern uint64 uapp_start;
extern struct task_struct* task[NR_TASKS];
extern uint64 uapp_start, uapp_end;
extern uint64 swapper_pg_dir[512];
//extern void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm);
//register unsigned long page_addr __asm__("sbadaddr");
void do_page_fault(struct pt_regs *regs) {
    printk("[S] PAGE_FAULT: scause: %d, sepc: 0x%lx, badaddr: 0x%lx\n",regs->scause,regs->sepc,regs->stval);
    struct vm_area_struct* vma;
    vma=find_vma(current->mm,regs->stval);
    //printk("vma->vm_start=%u\n",vma->vm_start);
    //printk("vma->vm_end=%u\n",vma->vm_end);
    //printk("vma->vm_flags=%u\n",vma->vm_flags);
    if(regs->scause==12){
 	create_mapping(current->pgd,vma->vm_start,(uint64)(&uapp_start)-PA2VA_OFFSET,vma->vm_end-vma->vm_start,vma->vm_flags*2+1+16);
    }else if(regs->scause==13||regs->scause==15){
        create_mapping(current->pgd,vma->vm_start,current->thread_info->user_sp-PGSIZE,vma->vm_end-vma->vm_start,vma->vm_flags*2+1+16);
    }
    /*
    1. 通过 stval 获得访问出错的虚拟内存地址（Bad Address）
    2. 通过 scause 获得当前的 Page Fault 类型
    3. 通过 find_vm() 找到对应的 vm_area_struct
    4. 通过 vm_area_struct 的 vm_flags 对当前的 Page Fault 类型进行检查
        4.1 Instruction Page Fault      -> VM_EXEC
        4.2 Load Page Fault             -> VM_READ
        4.3 Store Page Fault            -> VM_WRITE
    5. 最后调用 create_mapping 对页表进行映射
    */
}
static int pid_count=2;
void forkret() {
    ret_from_fork(current->trapframe);
}

uint64 do_fork(struct pt_regs *regs) {
    uint64 i,j;
    struct task_struct* child=(struct task_struct*)kalloc();
    unsigned long ret;
    ret = kalloc();
    child = ret;
    child -> state = TASK_RUNNING;
    child -> counter = 0;
    child -> priority = rand();
    child -> pid = pid_count++;
    task[child->pid]=child;
    child -> thread.ra = &forkret;
    child -> thread.sp = ret + PGSIZE;
    child -> thread_info=(struct thread_info *)kalloc();
    child -> thread_info->kernel_sp = child -> thread.sp - PA2VA_OFFSET;//PA
    child -> thread_info->user_sp = kalloc()-PA2VA_OFFSET+PGSIZE;//PA
    //printk("child->thread_info->user_sp=0x%lx\n",child->thread_info->user_sp);
    //printk("child->thread_info->user_sp+PGSIZE=0x%lx\n",child->thread_info->user_sp+PGSIZE);
    //printk("child->thread_info->user_sp+PGSIZE-PA2VA_OFFSET=0x%lx\n",child->thread_info->user_sp+PGSIZE-PA2VA_OFFSET);
    //printk("PA2VA_OFFSET=0x%lx\n",PA2VA_OFFSET);
    //printk("child->thread.sp=0x%lx\n",child->thread.sp);
    //printk("child->thread.sp-PA2VA_OFFSET=0x%lx\n",child->thread.sp-PA2VA_OFFSET);
    for(i=child->thread_info->user_sp+PA2VA_OFFSET-PGSIZE,j=current->thread_info->user_sp+PA2VA_OFFSET-PGSIZE;i<child->thread_info->user_sp+PA2VA_OFFSET;i+=1,j+=1){
        //将父进程用户栈的内容拷贝到子进程的用户栈中
        *(char*)i=*(char*)j;
    }
    child -> thread.sscratch = child -> thread.sp;
    child -> thread.sepc = regs->sepc+4;
    //child -> thread.sstatus = child -> thread.sp;
    child -> thread.sstatus = 0x40020;
    child -> pgd = kalloc();
    for(j=0;j<512;j++)
    {
        child -> pgd[j]=swapper_pg_dir[j];
    }
    child -> mm=(struct mm_struct*)kalloc();
    do_mmap(child->mm,USER_START,(uint64)(&uapp_end)-(uint64)(&uapp_start),7);
    do_mmap(child->mm,USER_END-PGSIZE,PGSIZE,3);
    child -> trapframe = (struct pg_regs*)kalloc();
    child -> trapframe->x0 = regs->x0;
    child -> trapframe->ra = regs->ra;
    child -> trapframe->x2 = regs->sscratch;
    //child -> trapframe->x2 = regs->x2;
    child -> trapframe->gp = regs->gp;
    child -> trapframe->tp = regs->tp;
    child -> trapframe->t0 = regs->t0;
    child -> trapframe->t1 = regs->t1;
    child -> trapframe->t2 = regs->t2;
    child -> trapframe->s0 = regs->s0;
    child -> trapframe->s1 = regs->s1;
    child -> trapframe->a0 = 0;//???
    child -> trapframe->a1 = regs->a1;
    child -> trapframe->a2 = regs->a2;
    child -> trapframe->a3 = regs->a3;
    child -> trapframe->a4 = regs->a4;
    child -> trapframe->a5 = regs->a5;
    child -> trapframe->a6 = regs->a6;
    child -> trapframe->a7 = regs->a7;
    child -> trapframe->s2 = regs->s2;
    child -> trapframe->s3 = regs->s3;
    child -> trapframe->s4 = regs->s4;
    child -> trapframe->s5 = regs->s5;
    child -> trapframe->s6 = regs->s6;
    child -> trapframe->s7 = regs->s7;
    child -> trapframe->s8 = regs->s8;
    child -> trapframe->s9 = regs->s9;
    child -> trapframe->s10 = regs->s10;
    child -> trapframe->s11 = regs->s11;
    child -> trapframe->t3 = regs->t3;
    child -> trapframe->t4 = regs->t4;
    child -> trapframe->t5 = regs->t5;
    child -> trapframe->t6 = regs->t6;
    child -> trapframe->sp = regs->sscratch;//????
    child -> trapframe->sstatus = regs->sstatus;
    child -> trapframe->sepc = regs->sepc+4;//???
    child -> trapframe->scause = regs->scause;
    child -> trapframe->stval = regs->stval;
    //if(current->pid==child->pid)return child->pid;
    //else return 0;
    return child->pid;
}

uint64 clone(struct pt_regs *regs) {
    return do_fork(regs);
}
//下面这两个函数的参数可以自己设置
int sys_wait(struct pt_regs *regs) {
    //set self as pending state;
    // schedule;
}
void sys_exit() {
	/*
     while(true) {
          if( parent is pending )
               break;
          else
               schedule to give up cpu;
     }
     set parent as running state;
     schedule to give up cpu;
	 */
}
void trap_handler(unsigned long scause, unsigned long sepc, struct pt_regs *regs) {
    current->trapframe=regs;
    unsigned long temp = scause >> 63;
    if(temp == 1)
    {
        temp = scause & 0x7fffffffffffffff;
        if(temp == 5)//Supervisor timer interrupt
        {
            //printk("[S] Supervisor Mode Timer Interrupt\n");
            clock_set_next_event();
            do_timer();
        }
    }
    else if(temp == 0)
    {
        temp = scause & 0x7fffffffffffffff;
        if(temp == 8)//Environment call from U-mode
        {
            if(regs->a7==SYS_GETPID)
            {
                regs->sepc+=4;
                regs->a0=current->pid;
            }else if(regs->a7==SYS_WRITE)
            {
                char* buffer=(char*)(regs->a1);
                regs->a0=printk(buffer);
                regs->sepc+=4;
            }else if(regs->a7==SYS_CLONE){
                regs->a0=clone(regs);
                regs->sepc+=4;
                //forkret();
            }
        }
        /*else if(temp == 9)//Environment call from S-mode
        {
            
        }*/
        else if(temp==12 || temp==13 || temp==15){
            do_page_fault(regs);
        }
    }
}
