#include "defs.h"
#include "proc.h"

extern struct task_struct* current;
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
    //...
}