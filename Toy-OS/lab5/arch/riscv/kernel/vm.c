// arch/riscv/kernel/vm.c
#include "defs.h"
#include "vm.h"
extern uint64 _stext,_srodata,_sdata,_ekernel,_sbss;
/* early_pgtbl: 用于 setup_vm 进行 1GB 的 映射。 */
unsigned long  early_pgtbl[512] __attribute__((__aligned__(0x1000)));

void setup_vm(void) {
    memset(early_pgtbl, 0x0, PGSIZE);
    int index;
    uint64 va=0x80000000;
    uint64 pa=0x80000000;
    index=(va&0x7FC0000000)>>30;
    early_pgtbl[index]=(pa>>12<<10)+0xF;

    va=0xffffffe000000000;
    pa=0x80000000;
    index=(va&0x7FC0000000)>>30;
    early_pgtbl[index]=(pa>>12<<10)+0xF;
}



/* swapper_pg_dir: kernel pagetable 根目录， 在 setup_vm_final 进行映射。 */
unsigned long  swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));
uint64*  putbl;
uint64*  pmtbl;

void setup_vm_final(void) {
    memset(swapper_pg_dir, 0x0, PGSIZE);

    // No OpenSBI mapping required

    // mapping kernel text X|-|R|V
    create_mapping(swapper_pg_dir,(uint64)(&_stext),(uint64)(&_stext)-PA2VA_OFFSET,(uint64)(&_srodata)-(uint64)(&_stext),11);

    // mapping kernel rodata -|-|R|V
    create_mapping(swapper_pg_dir,(uint64)(&_srodata),(uint64)(&_srodata)-PA2VA_OFFSET,(uint64)(&_sdata)-(uint64)(&_srodata),3);
    
    // mapping kernel data -|W|R|V
    create_mapping(swapper_pg_dir,(uint64)(&_sdata),(uint64)(&_sdata)-PA2VA_OFFSET,(uint64)(uapp_start)-(uint64)(&_sdata),7);
    
    // mapping other memory -|W|R|V
    create_mapping(swapper_pg_dir,(uint64)(&_sbss),(uint64)(&_sbss)-PA2VA_OFFSET,(uint64)(VM_START+PHY_SIZE)-(uint64)(&_sbss),7);
        
    // mapping uapp memory U|X|W|R|V
    //create_mapping(swapper_pg_dir,USER_START,(uint64)(&uapp_start)-PA2VA_OFFSET,(uint64)(&uapp_end)-(uint64)(&uapp_start),31);
    
    
    // set satp with swapper_pg_dir

    //YOUR CODE HERE
    asm volatile("li t0, 0x8000000000000000");
    asm volatile("la t1, swapper_pg_dir");
    asm volatile("li t2, 0xFFFFFFDF80000000");
    asm volatile("sub t1, t1, t2");
    asm volatile("srli t1,t1, 12");
    asm volatile("add t0, t0, t1");
    asm volatile("csrw satp, t0");
    // flush TLB
    asm volatile("sfence.vma zero, zero");
    return;
}


/* 创建多级页表映射关系 */
void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm) {

    int index1, index2, index3;
    //static int flag=0;
    /*index1=(va&0x7FC0000000)>>30;
    index2=(va&0x3FE00000)>>21;
    index3=(va&0x1FF000)>>12;
    if()
    putbl=kalloc();
    pmtbl=kalloc();
    pgtbl[index1]=(((uint64)putbl-PA2VA_OFFSET)>>12<<10)+1;
    putbl[index2]=(((uint64)pmtbl-PA2VA_OFFSET)>>12<<10)+1;*/
    while((int)(sz)>0){
    	index1=(va&0x7FC0000000)>>30;
    	index2=(va&0x3FE00000)>>21;
        index3=(va&0x1FF000)>>12;
        if((pgtbl[index1]&1)!=1){
            putbl=kalloc();
            pgtbl[index1]=(((uint64)putbl-PA2VA_OFFSET)>>12<<10)+1;
        }  
        if((putbl[index2]&1)!=1){
            pmtbl=kalloc();
            putbl[index2]=(((uint64)pmtbl-PA2VA_OFFSET)>>12<<10)+1;
        }
        if((pmtbl[index3]&1)!=1){
            pmtbl[index3]=(pa>>12<<10)+perm;
            pa+=0x1000;
            va+=0x1000;
            sz-=0x1000;
            //flag++;
        }
        else{
            pmtbl[index3]=(pa>>12<<10)+perm;
            pa+=0x1000;
            va+=0x1000;
            sz-=0x1000;
        }
    }
    
}
