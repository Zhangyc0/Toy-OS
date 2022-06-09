#include "fs.h"
#include "mm.h"
#include "defs.h"

#define ALIGN4(value) (((size_t)value + 3) >> 2 << 2)

//uint64 *initrd_start = 0x4200000UL + VM_START;
//int cpio_header_size = 14 * 8 - 2;

/* 把十六进制字符串转化为十进制数 */
size_t getFieldVal(char *field, int length) {
    size_t value = 0;
    for (int i = 0; i < length; i++) {
        switch (field[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                value = (value << 4) | (field[i] - '0');
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
                value = (value << 4) | (field[i] - 'a' + 10);
                break;
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
                value = (value << 4) | (field[i] - 'A' + 10);
                break;
        }
    }

    return value;
}

/*比较两个文件名是否相同*/
int check_file_name(char *tar, char *cmp) {
    while (*tar && *tar == *cmp)
        tar++, cmp++;
    return *tar - *cmp;
}

struct cpio_stat cpio_find_file(char *name) {
    struct cpio_stat stat = {.c_ino = 0};
    uint64 *initrd_start = 0x4200000UL + VM_START;
    struct cpio_newc_header *p = initrd_start;

    if ((name[0] == '.') && (name[1] == '/')) {
        name += 2;
    } else if (name[0] == '/') {
        name += 1;
    }

    while (check_file_name("TRAILER!!!", p->c_name)) {
        //Log("Current is: %s", p->c_name);
        printk("Current is: %s\n", p->c_name);
        //printk("Current *p is: %u\n",*p);
        //printk("Current c_magic is: %c%c%c%c%c%c\n",p->c_magic[0],p->c_magic[1],p->c_magic[2],p->c_magic[3],p->c_magic[4],p->c_magic[5]);
        int cpio_header_size = 14 * 8 - 2;

        if (!check_file_name(name, p->c_name)) {
            stat.c_ino = getFieldVal(p->c_ino, 8);
            stat.c_mode = getFieldVal(p->c_mode, 8);
            stat.c_uid = getFieldVal(p->c_uid, 8);
            stat.c_gid = getFieldVal(p->c_gid, 8);
            stat.c_nlink = getFieldVal(p->c_nlink, 8);
            stat.c_mtime = getFieldVal(p->c_mtime, 8);
            stat.c_filesize = getFieldVal(p->c_filesize, 8);
            stat.c_devmajor = getFieldVal(p->c_devmajor, 8);
            stat.c_devminor = getFieldVal(p->c_devminor, 8);
            stat.c_rdevmajor = getFieldVal(p->c_rdevmajor, 8);
            stat.c_rdevminor = getFieldVal(p->c_rdevminor, 8);
            stat.data = (void *)p + ALIGN4(cpio_header_size + getFieldVal(p->c_namesize, 8));

            return stat;
        }

        p = (void *)p + ALIGN4(cpio_header_size + getFieldVal(p->c_namesize, 8)) + ALIGN4(getFieldVal(p->c_filesize, 8));
    }

    return stat;
}

// 070701
// 002A1C63
// 000081B4
// 000003E8
// 000003E8
// 00000001
// 5FE4A4B8
// 00000018
// 00000008
// 00000011
// 00000000
// 00000000
// 00000005
// 00000000
// flag..
// flag={HappyNewYear2021}.

void fs_test() {
    struct cpio_stat stat = cpio_find_file("flag");
    if (stat.c_ino) {
        printk("Get File: \n");
        printk("\tc_ino->%lx\n", stat.c_ino);
        printk("\tc_mode->%lx\n", stat.c_mode);
        printk("\tc_uid->%lx\n", stat.c_uid);
        printk("\tc_gid->%lx\n", stat.c_gid);
        printk("\tc_nlink->%lx\n", stat.c_nlink);
        printk("\tc_mtime->%lx\n", stat.c_mtime);
        printk("\tc_filesize->%lx\n", stat.c_filesize);
        printk("\tc_devmajor->%lx\n", stat.c_devmajor);
        printk("\tc_devminor->%lx\n", stat.c_devminor);
        printk("\tc_rdevmajor->%lx\n", stat.c_rdevmajor);
        printk("\tc_rdevminor->%lx\n", stat.c_rdevminor);
        printk("File Content: \n");
        char *value = stat.data;
        for (int i = 0; i < stat.c_filesize; i++) {
            putc(value[i]);
        }
        printk("\n");
    }
}

struct inode *namei(char *path) {//path文件名
    struct inode *n = kalloc();
    struct cpio_stat *stat = kalloc();
    n->i_private = stat;
    *stat = cpio_find_file(path);
    return n;
}

int readi(struct inode *ip, int user_dst, void *dst, uint64 off, uint64 n) {
    struct cpio_stat *stat = ip->i_private;
    void *base = stat->data;
    memmove(dst, base + off, n);
    return n;
}
