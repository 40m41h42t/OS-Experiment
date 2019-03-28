> #### 练习1：理解通过make生成执行文件的过程。（要求在报告中写出对下述问题的回答）
>
> 列出本实验各练习中对应的OS原理的知识点，并说明本实验中的实现部分如何对应和体现了原理中的基本概念和关键知识点。
>
> 在此练习中，大家需要通过静态分析代码来了解：
>
> 1. 操作系统镜像文件ucore.img是如何一步一步生成的？(需要比较详细地解释Makefile中每一条相关命令和命令参数的含义，以及说明命令导致的结果)
> 2. 一个被系统认为是符合规范的硬盘主引导扇区的特征是什么？
>
> 补充材料：
>
> 如何调试Makefile
>
> 当执行make时，一般只会显示输出，不会显示make到底执行了哪些命令。
>
> 如想了解make执行了哪些命令，可以执行：
>
> ```
> $ make "V="
> ```
>
> 要获取更多有关make的信息，可上网查询，并请执行
>
> ```
> $ man make
> ```



![](assets/ucore-lab1-dependency-tree.png)

一个非常好的结构（来源：https://xr1s.me/2018/05/15/ucore-lab1-report/）

lab1 包含了两个 makefile 文件，一个位于实验目录下，另一个位于 `./tools/function.mk` 下。可以推测另一个 makefile 文件主要定义了函数。

在文档中提到 `make "V="` 可以查看日志，刚开始没看懂，后来查文档才明白它自己实现了一个 `V`。

对于问题一而言，通过 log 我们可以看到，它首先编译了 `libs` 和 `kern` 下所有的  .c 文件，生成了 `bin/kernel`，之后编译了 `boot` 下的文件，生成 `bin/bootblock` 这个文件。这个过程还是蛮 tricky 的，我们看到的这段 log 大概是这样子的：

```
+ cc boot/bootasm.S
gcc -Iboot/ -fno-builtin -fno-PIC -Wall -ggdb -m32 -gstabs -nostdinc  -fno-stack-protector -Ilibs/ -Os -nostdinc -c boot/bootasm.S -o obj/boot/bootasm.o
+ cc boot/bootmain.c
gcc -Iboot/ -fno-builtin -fno-PIC -Wall -ggdb -m32 -gstabs -nostdinc  -fno-stack-protector -Ilibs/ -Os -nostdinc -c boot/bootmain.c -o obj/boot/bootmain.o
+ cc tools/sign.c
gcc -Itools/ -g -Wall -O2 -c tools/sign.c -o obj/sign/tools/sign.o
gcc -g -Wall -O2 obj/sign/tools/sign.o -o bin/sign
+ ld bin/bootblock
ld -m    elf_i386 -nostdlib -N -e start -Ttext 0x7C00 obj/boot/bootasm.o obj/boot/bootmain.o -o obj/bootblock.o
'obj/bootblock.out' size: 488 bytes
```

它首先编译出了 `bootasm.o` 和 `bootmain.o`，而我们看到的相关的 Makefile 代码是这样子的：

```makefile
# create bootblock
bootfiles = $(call listf_cc,boot)
$(foreach f,$(bootfiles),$(call cc_compile,$(f),$(CC),$(CFLAGS) -Os -nostdinc))

bootblock = $(call totarget,bootblock)

$(bootblock): $(call toobj,$(bootfiles)) | $(call totarget,sign)
	@echo + ld $@
	$(V)$(LD) $(LDFLAGS) -N -e start -Ttext 0x7C00 $^ -o $(call toobj,bootblock)
	@$(OBJDUMP) -S $(call objfile,bootblock) > $(call asmfile,bootblock)
	@$(OBJCOPY) -S -O binary $(call objfile,bootblock) $(call outfile,bootblock)
	@$(call totarget,sign) $(call outfile,bootblock) $(bootblock)

$(call create_target,bootblock)
```

`bootfiles` 进行了编译，`bootblock` 则进行了接下来的过程。

刚开始没有看懂为什么它还生成了 `bootblock.out`，接着看 Makefile 就能看见它隐式地调用了 objcopy，也就是说，它先生成了 `bootblock.o`，然后通过该函数生成了 `bootblock.out`。接下来则是调用 `tools/sign` 进行检测，根据 `sign.c` 和 `Makefile` 中的代码，不难判断，传入检测的参数分别是 `bin/bootblock.out` 和 `bin/bootblock`。而 `sign.c` 则是另外一段比较有趣的文件了。

在 `sign.c` 中，`stat` 结构体将文件加载进来，之后进行判断。分析 `sign.c` 就是回答第二问了。根据逻辑我们可以得到如下结论，一个被系统认为是符合规范的硬盘主引导扇区的特征有：

- 有且只有 512 字节
- 最后两位是 `0x55 0xAA`

回到 Makefile 中，主引导区合法后继续链接 `ucore` 。log 如下：

```
dd if=/dev/zero of=bin/ucore.img count=10000
10000+0 records in
10000+0 records out
5120000 bytes (5.1 MB, 4.9 MiB) copied, 1.86576 s, 2.7 MB/s
dd if=bin/bootblock of=bin/ucore.img conv=notrunc
1+0 records in
1+0 records out
512 bytes copied, 0.00074265 s, 689 kB/s
dd if=bin/kernel of=bin/ucore.img seek=1 conv=notrunc
146+1 records in
146+1 records out
74828 bytes (75 kB, 73 KiB) copied, 0.0417151 s, 1.8 MB/s
```

首先从 `/dev/zero`创建 `bin/ucore` 文件，接着按照逐字节的方式将 `bootblock` 和 `kernel` 拷贝进去。

接下来我们分析一下编译过程中调用的程序的相关参数。

# 总结



# 参考资料：

[ucore lab1 实验报告](https://xr1s.me/2018/05/15/ucore-lab1-report/)