# MapleOS

MapleOS是一个基于AMD64(x86-64)架构的微型操作系统，旨在用最精简的代码实现一个操作系统的基本功能，但同时保持较好的代码结构与可读性。

## 动机

目前的操作系统课程教学还基本上都是以i386(IA-32)的保护模式(Protected Mode)为主，而少有基于AMD64长模式(Long Mode)的。AMD64相比于i386新增了很多功能，并借着新架构的机会抛弃了一些历史遗留的设计。虽然为了保证兼容性，这些历史遗留设计并不可能完全删掉，但如果我们只针对AMD64架构编写操作系统的话，就可以少考虑很多问题，而且使用最新的CPU架构也便于与时代接轨。不过，了解CPU架构的发展过程仍然是需要的，这对于我们理解计算机体系结构与操作系统很有帮助。

不仅如此，通常的操作系统课程也不会让你从头编写一个操作系统，一般都是给一个包括BootLoader在内的操作系统框架，简单了解之后直接进行内核代码的编写。但万事开头难，我认为这框架恰恰是最难的一步，只有亲手做过之后方可对计算机体系结构与操作系统有更深刻的了解，而之后的内核代码编写更多的是水到渠成的事情。

但其实如果真的完全从头实现一个操作系统的话，在经验不足的情况下，的确很难做到。因此本项目参考了ucore的设计与实现，它是基于i386的，虽然ucore plus基于AMD64和其他平台也做了一些工作，但似乎太过复杂了。当然，ucore本身也参考了很多国外的教学用操作系统。我对ucore进行了大量修改与精简，并基于AMD64重新实现。

## 编译运行

本项目基于GNU/Linux工具链来完成，内核使用ELF格式，使用QEMU来模拟AMD64环境运行img镜像。

以Ubuntu 18.04 LTS为例，需要安装的编译环境与运行环境如下：

``` sh
sudo apt install build-essential
sudo apt install qemu-system
```

接着，在MapleOS的目录下执行`make qemu`即可编译运行。

## 已实现功能

编写操作系统非一日之功。目前，我已完成如下功能：

* 在MBR中加载BootLoader到内存中
* 在BootLoader中开启PML4E->PDPE->PDE->PTE的四级分页，并做了2M地址空间的内存映射
* 在BootLoader中进入长模式并加载操作系统内核到内存中
* 在内核中实现串口与CGA的写操作
* 实现stdio.h与string.h的部分函数

## 已知问题

暂无。

## 文件结构

``` text
MapleOS/
├── boot
│   ├── bootloader.h
│   ├── bootloader.S（用于进入长模式的16位与64位汇编程序）
│   ├── bootmain.c（加载ELF格式的操作系统内核到内存）
│   └── bootmbr.S（用于编译成MBR的16位汇编程序）
├── kern
│   ├── driver
│   │   ├── console.c（CGA与串口的驱动程序）
│   │   └── console.h
│   ├── init
│   │   └── init.c（内核入口）
│   └── libs
│       └── stdio.c（stdio的部分函数实现）
├── libs
│   ├── defs.h（通用定义）
│   ├── elf.h（ELF文件格式信息）
│   ├── printfmt.c（printf的主要功能实现）
│   ├── stdarg.h（变长参数表的宏定义）
│   ├── stdio.h
│   ├── string.c（string的部分函数实现）
│   ├── string.h
│   └── x86.h（对机器指令的封装）
├── Makefile
├── README.md
└── tools
    ├── bootloader.gdbinit（用于gdb调试BootLoader）
    ├── bootmbr.gdbinit（用于gdb调试mbr）
    ├── kernel.gdbinit（用于gdb调试内核）
    ├── kernel.ld（内核链接时的配置信息）
    └── sign.c（检查MBR与BootLoader的大小合法性，并添加MBR结束符）
```

## 参考资料

* [清华大学操作系统课程](http://www.xuetangx.com/courses/course-v1:TsinghuaX+30240243X+sp/about)、[ucore](https://github.com/chyyuu/ucore_os_lab)、[ucore os docs](https://chyyuu.gitbooks.io/ucore_os_docs/)、[simple os book](https://chyyuu.gitbooks.io/simple_os_book/)
* [AMD64 Architecture Programmer’s Manual](https://search.amd.com/zh-cn/Pages/results-all.aspx?k=AMD64+Architecture+Programmer%E2%80%99s+Manual) Volume 2: System Programming or [《64位微处理器系统编程》](https://books.google.com/books?id=_jM0akapIrkC&printsec=frontcover&hl=zh-CN#v=onepage&q&f=false)
* [OSDev Wiki](https://wiki.osdev.org/Main_Page)
* [x86 and amd64 instruction reference](https://www.felixcloutier.com/x86/) or [Intel® 64 and IA-32 Architectures Software Developer Manuals](https://software.intel.com/en-us/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4)
* [《汇编语言》](https://www.amazon.cn/gp/product/B00EYSPGYE/)
