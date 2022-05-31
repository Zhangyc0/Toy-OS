#include "syscall.h"
#include "stdio.h"
#include "string.h"

static inline long getpid() {
    long ret;
    asm volatile ("li a7, %1\n"
                  "ecall\n"
                  "mv %0, a0\n"
                : "+r" (ret) 
                : "i" (SYS_GETPID));
    return ret;
}

static inline long fork()
{
  long ret;
  asm volatile ("li a7, %1\n"
                "ecall\n"
                "mv %0, a0\n"
                : "+r" (ret) : "i" (SYS_CLONE));
  return ret;
}

static inline long exit(int stat)
{
	long ret;
	asm volatile (	"li a7, %2\n"
					"mv a0, %1\n"
					"ecall\n"
					"mv %0, a0\n"
					: "+r" (ret)
					: "r" (stat), "i" (SYS_EXIT)
					);
	return ret;
}
static inline long wait()
{
	long ret;
	asm volatile (	"li a7, %1\n"
					"ecall\n"
					"mv %0, a0\n"
					: "+r" (ret)
					: "i" (SYS_WAIT)
					);
	return ret;
}
static inline long exec(char* path)
{
	long ret;
	asm volatile (	"li a7, %2\n"
					"mv a0, %1\n"
					"ecall\n"
					"mv %0, a0\n"
					: "+r" (ret)
					: "r" (path), "i" (SYS_EXECVE)
					);
	return ret;
}

void runcmd(char* cmd) {
	exec(cmd);
	printf("exec %s failed\n", cmd);
	exit(0);
}

int getcmd(char *buf, int nbuf) {
	printf("$ ");
	memset(buf, 0, nbuf);
	gets(buf, nbuf);
	if(buf[0] == 0)
		return -1;
	return 0;
}

/*
int main() {

    while(1) {
        printf("[PID = %d] is running!\n",getpid());
        for (unsigned int i = 0; i < 0x4FFFFFFF; i++);
    }
}
*/
/*
int main() {
    int pid;
    printf("[U] Enter main\n");
    pid = fork();
    printf("[U] pid: %ld\n", pid);

    if (pid == 0) {
        while (1) {
            printf("[U-CHILD] pid: %ld is running!\n", getpid());
            for (unsigned int i = 0; i < 0x4FFFFFFF; i++);
        } 
    } else {
        while (1) {
            printf("[U-PARENT] pid: %ld is running!\n", getpid());
            for (unsigned int i = 0; i < 0x4FFFFFFF; i++);
        } 
    }
    return 0;
}
*/

int main() {
  int pid;
  pid = fork();
  if(pid > 0)
    printf("[PID = %d] fork [PID = %d]\n", getpid(), pid);
  pid = fork();
  if(pid > 0)
    printf("[PID = %d] fork [PID = %d]\n", getpid(), pid);
   while(1) {
      printf("[PID = %d] is running!\n", getpid());
      for (unsigned int i = 0; i < 0x4FFFFFFF; i++);
  }
}

/*
int main(void) {
	static char buf[100];
	int fd;
	
	while(getcmd(buf,sizeof(buf)) >= 0) {
        printf("%s\n", buf);
		if (strcmp(buf, "hello") == 0) {
			int pid = fork();
			printf("Get pid %d\n", pid);
			if(pid == 0)
				runcmd(buf);
			else if (pid > 0) {
				wait();
			}
			else
				printf("panic fork");
		}
		else {
			printf("Invalid command %s\n", buf);
		}
	}
	exit(0);
}
*/