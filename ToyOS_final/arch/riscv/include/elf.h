#ifndef ELF_H
#define ELF_H
// Format of an ELF executable file

#include "types.h"


#define Elf64



#define ELF_MAGIC 0x464C457FU  // "\x7FELF" in little endian

// File header
struct elfhdr {
  uint magic;  // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint version;
  uint64 entry;
  uint64 phoff;// program header table距离ELF文件开始的偏移(单位：字节)
  uint64 shoff;// section header table距离ELF文件开始的偏移(单位：字节)
  uint flags;
  ushort ehsize;
  ushort phentsize;// 单个program header的大小(单位：字节)
  ushort phnum;// program header数量
  ushort shentsize;// 单个section header的大小(单位：字节)
  ushort shnum;// section header数量
  ushort shstrndx;
};


// Program segment header
struct proghdr {
  uint32 type;// type的值决定了segment的类型，在本实验中我们只需要关注LOAD类型的segment，即type值为1的segment
  uint32 flags; // segment的权限属性，包括R,W,X
  uint64 off; // segment在ELF文件中的偏移(单位：字节)
  uint64 vaddr;// segment的第一个字节在进程虚拟地址空间的起始位置。整个program header table中，所有“LOAD”类型的segment按照vaddr从小到大排列
  uint64 paddr;// segment的物理装载地址，一般与vaddr一样
  uint64 filesz;// segment在ELF文件中所占空间的长度(单位：字节)
  uint64 memsz;// segment在进程虚拟地址空间所占用的长度(单位：字节)
  uint64 align;// segment的对齐属性。实际对齐字节等于2的align次。例如当align等于10，那么实际的对齐属性就是2的10次方，即1024字节
};

// Values for Proghdr type
#define ELF_PROG_LOAD           1

// Flag bits for Proghdr flags
#define ELF_PROG_FLAG_EXEC      1
#define ELF_PROG_FLAG_WRITE     2
#define ELF_PROG_FLAG_READ      4

#endif
