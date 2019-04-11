# 文件读写编程题目

## myecho.c

- myecho.c的功能与系统echo程序相同

- 接受命令行参数，并将参数打印出来，例子如下：

  ```bash
  $ ./myecho x
  x
  $ ./myecho a b c
  a b c
  ```

## mycat.c

- mycat.c的功能与系统cat程序相同

- mycat将指定的文件内容输出到屏幕，例子如下：

- 要求使用系统调用open/read/write/close实现

  ```bash
  $ cat /etc/passwd 
  root:x:0:0:root:/root:/bin/bash
  daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
  bin:x:2:2:bin:/bin:/usr/sbin/nologin
  ...
  $ ./mycat /etc/passwd 
  root:x:0:0:root:/root:/bin/bash
  daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
  bin:x:2:2:bin:/bin:/usr/sbin/nologin
  ...
  ```

## mycp.c

- mycp.c的功能与系统cp程序相同

- 将源文件复制到目标文件，例子如下：

- 要求使用系统调用open/read/write/close实现

  ```bash
  $ cat /etc/passwd
  root:x:0:0:root:/root:/bin/bash
  daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
  bin:x:2:2:bin:/bin:/usr/sbin/nologin
  ...
  $ ./mycp /etc/passwd passwd.bak 
  $ cat passwd.bak
  root:x:0:0:root:/root:/bin/bash
  daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
  bin:x:2:2:bin:/bin:/usr/sbin/nologin
  ...
  ```

# 多进程题目

## mysys.c: 实现函数mysys，用于执行一个系统命令，要求如下

- mysys的功能与系统函数system相同，要求用进程管理相关系统调用自己实现一遍

- 使用fork/exec/wait系统调用实现mysys

- 不能通过调用系统函数system实现mysys

- 测试程序

  ```c
  #include <stdio.h>
  
  void mysys(char *command)
  {
      实现该函数，该函数执行一条命令，并等待该命令执行结束
  }
  
  int main()
  {
      printf("--------------------------------------------------\n");
      mysys("echo HELLO WORLD");
      printf("--------------------------------------------------\n");
      mysys("ls /");
      printf("--------------------------------------------------\n");
      return 0;
  }
  ```

- 测试程序的输出结果

  ```
  --------------------------------------------------
  HELLO WORLD
  --------------------------------------------------
  bin    core  home	     lib	 mnt   root  snap  tmp	vmlinuz
  boot   dev   initrd.img      lost+found  opt   run   srv   usr	vmlinuz.old
  cdrom  etc   initrd.img.old  media	 proc  sbin  sys   var
  --------------------------------------------------
  ```

## sh1.c

- 该程序读取用户输入的命令，调用函数mysys(上一个作业)执行用户的命令，示例如下

  ```
  # 编译sh1.c
  $ cc -o sh1 sh1.c
  
  # 执行sh1
  $ ./sh 
  
  # sh1打印提示符>，同时读取用户输入的命令echo，并执行输出结果
  > echo a b c
  a b c
  
  # sh1打印提示符>，同时读取用户输入的命令cat，并执行输出结果
  > cat /etc/passwd
  root:x:0:0:root:/root:/bin/bash
  daemon:x:1:1:daemon:/usr/sbin:/usr/sbin/nologin
  bin:x:2:2:bin:/bin:/usr/sbin/nologin
  ```

- 请考虑如何实现内置命令cd、pwd、exit

## sh2.c: 实现shell程序，要求在第1版的基础上，添加如下功能

- 实现文件重定向

  ```
  # 执行sh2
  $ ./sh2
  
  # 执行命令echo，并将输出保存到文件log中
  > echo hello >log
  
  # 打印cat命令的输出结果
  > cat log
  hello
  ```

## sh3.c: 实现shell程序，要求在第2版的基础上，添加如下功能

- 实现管道

  ```
  # 执行sh3
  $ ./sh3
  
  # 执行命令cat和wc，使用管道连接cat和wc
  > cat /etc/passwd | wc -l
  ```