#include "defs.h"
#include "proc.h"
#include "elf.h"
#include "fs.h"

extern void __dummy();
extern struct task_struct* current;
extern unsigned long  swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));
// 同学们可直接复制sys_exec()以及exec()
int sys_exec(const char *path) {
    int ret = exec(path);
    return ret;
}

int exec(const char *path) {
    int ret = proc_exec(current, path);
    return ret;
}

// 需要同学们实现proc_exec函数
int proc_exec(struct task_struct *proc, const char *path) {
        /*
    inode = namei(path);
    
    create a new page table newpgtbl, copy kernel page table from kpgtbl (same as lab5);
    
    readi() read the elf header(denoted as elf_header) from elf file;
    check whether elf_header.magic == ELF_MAGIC;
    
    for(i=0; i<elf_header.phnum; i++) {
        readi() read the program header(denoted as prog_header) of each segment;
        check whether prog_header.type == LOAD;
        parse_ph_flags() parse the prog_header.flags to permissions;
        uvmalloc() allocate user pages for [prog_header.vaddr, prog_header.vaddr+prog_header.memsz] of this segment in newpgtbl, and set proper permissions;
        loadseg() copy the content of this segment from prog_header.off to its just allocated memory space;
    }
    
    allocate a page for user stack and update the newpgtbl for it. The user stack va range: [USER_END-PAGE_SIZE, USER_END];
    
    set proc's sstatus, sscratch, and sepc like task_init() in Lab5;
    set proc->pgtbl to newpgtbl;
    */
   //
    proc->state = TASK_RUNNING;
    proc->counter=1;
    proc->priority=rand();
    //proc->pid=1;
    proc->thread.ra=&__dummy;
    proc->thread.sp=(uint64)(proc)+PGSIZE;
    proc -> thread_info=(struct thread_info *)kalloc();
    proc -> thread_info->kernel_sp = proc -> thread.sp - PA2VA_OFFSET;//PA

    struct inode* ip = namei(path);
    struct elfhdr* elf=kalloc();
    struct proghdr* prog=kalloc();
    

    proc->pgd = kalloc();
    int i;
    for(i = 0; i < 512; i++) {
        proc->pgd[i] = swapper_pg_dir[i];
    }

    if(readi(ip, 0, elf, 0, sizeof(struct elfhdr)) != sizeof(struct elfhdr)) {
        printk("elf header loading error!\n");
        return -1;
    }

    //printk("elf magic number = %u\n",elf->magic);
    if(elf->magic != ELF_MAGIC) {
        printk("elf magic number doesn't match!\n");
        return -1;
    }
    int ofs;
    for(i = 0, ofs = elf->phoff; i < elf->phnum; i++, ofs += sizeof(struct proghdr)) {
        if(readi(ip, 0, prog, ofs, sizeof(struct proghdr)) != sizeof(struct proghdr)) {
            printk("load program error!\n");
            return -1;
        } 
        if(prog->type != ELF_PROG_LOAD 
        || prog->memsz < prog->filesz
        || prog->vaddr + prog->memsz < prog->vaddr) {
            //printk("parse ph flags failed!\n");
            //return -1;
            continue;
        }
        void *segment=kalloc();
        create_mapping(proc->pgd, prog->vaddr, segment-PA2VA_OFFSET,prog->filesz , (uint64)(prog->flags +8)); // PTE_U 和 PTE_X
        if(prog->vaddr % PGSIZE != 0) {
            printk("prog vadder is not aligned!\n");
            return -1;
        }
        //readi(ip,0,segment,prog->off,prog->filesz);
        if(loadseg(proc->pgd, prog->vaddr, ip, prog->off, prog->filesz) == -1) {
            printk("loadseg failed!\n");
            return -1;
        }
    }
    proc -> thread_info->user_sp =(uint64)(kalloc() - PA2VA_OFFSET + PGSIZE);//PA
    //create_mapping(pgd, USER_END - PGSIZE, (uint64)kalloc(), PGSIZE, 15);
    create_mapping(proc->pgd,USER_END-PGSIZE,(uint64)proc->thread_info->user_sp,PGSIZE,11);
    //create_mapping(proc->pgd, USER_END - PGSIZE, (uint64)kalloc(), PGSIZE, (1 << 1) | (1 << 2) | (1 << 4));

    proc -> thread.sstatus = 0x40020;
    csr_write(sstatus, proc -> thread.sstatus);

    proc -> thread.sscratch = (uint64)USER_END;
    asm volatile("li sp, 0x4000000000");
    //csr_write(sscratch, (uint64)USER_END);

    proc -> thread.sepc = elf->entry;
    csr_write(sepc, elf->entry);

    uint64 content = ((uint64)proc->pgd-PA2VA_OFFSET)>>12;
    content+=(uint64)0x8000000000000000;

    csr_write(satp, content);
    // asm volatile("li t0, 0x8000000000000000");
    // asm volatile("la t1, proc->pgd");
    // asm volatile("li t2, 0xFFFFFFDF80000000");
    // asm volatile("sub t1, t1, t2");
    // asm volatile("srli t1,t1, 12");
    // asm volatile("add t0, t0, t1");
    // asm volatile("csrw satp, t0");
    // flush TLB
    asm volatile("sfence.vma zero, zero");
    //readi(ip, 0, elf, 0, sizeof(struct elfhdr));
    //printk("entry=%d\n",elf->entry);
    asm volatile("sret");
    //schedule();
    return 0;
}

int loadseg(pagetable_t pagetable, uint64 va, struct inode *ip, int offset, int filesz) {
    /*
    check whether va is aligned to PAGE_SIZE;
    for(i=0; i<filesz; i+=PAGE_SIZE) {
        walk the pagetable, get the corresponding pa of va;
        use readi() to read the content of segment to address pa;
    }
    */
   uint64 pa, pte;
   unsigned long load_size;
   if(va % PGSIZE != 0) {
       printk("Invalid virtual address : va should be aligned\n");
       return -1;
   }

   for(unsigned long i = 0; i < filesz; i+=PGSIZE) {
        pagetable_t current_page_addr = pagetable;
        
        pagetable_t current_entry_addr = (unsigned long)current_page_addr + (((va >> 30) & 0x1ff) << 3);
        current_page_addr = (pagetable_t)(((*current_entry_addr >> 10) << 12) + PA2VA_OFFSET);

        current_entry_addr = (unsigned long)current_page_addr + (((va >> 21) & 0x1ff) << 3);
        current_page_addr = (pagetable_t)(((*current_entry_addr >> 10) << 12) + PA2VA_OFFSET);

        current_entry_addr = (unsigned long)current_page_addr + (((va >> 12) & 0x1ff) << 3);
        
        pte = (*current_entry_addr) >> 10;
        pa = (pte << 12) + va % PGSIZE;
        if(pa == 0) {
            printk("load address error!\n");
            return -1;
        }
        
        if(filesz - i < PGSIZE) {
            load_size = filesz - i;
        } else {
            load_size = PGSIZE;
        }

        if(readi(ip, 0, (unsigned long*)(pa+PA2VA_OFFSET), offset + i, load_size) != load_size){ 
            return -1;
        }
   }

   return 0;
}