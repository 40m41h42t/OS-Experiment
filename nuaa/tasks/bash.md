bash 对输入字符串的处理

一般来讲，输入字符串的格式可以分为以下几种：

```
> prog ([-<para>] ([value]))([value])
```

参考网上搜到的资料，我们可以用 getopt 函数进行命令行的解析，不过在这里按照要求我们只需要进行简单的字符串分割即可，会用到 strtok 函数。

strtok 的原型是

```c
char *strtok(char s[], const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);
```

该怎么办才能让参数分离地优雅一些呢？

忽然意识到我太执着了，这个时候应该按照 KISS 法则先实现出来再说。。

如果是内置的命令的话则不需要 fork 出新的进程。

实现命令 cd：

chdir 函数可以改变当前的工作目录

```c
int chdir(const char * path);
```

getcwd 函数：返回当前的目录

```c
char * getcwd(char * buf, size_t size);
```

特性：可以自主分配，返回指针。

在 Linux 中，实现文件重定向利用了 `dup` 系列函数

```c
int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

我们继续追溯 dup，会找到 fcntl 函数

```c
int fcntl(int fd, int cmd, ... /* arg */ );
```

实现文件重定向的话也适合先用 KISS 法则，遍历字符串搜索字符 '`>`'，然后分割字符串，左右分别执行命令输出什么的。

重定向的例子如下：

```bash
> echo hello >log
```

这样的话左侧输出到一个文件流中，然后再输出到右侧的文件。本来是想开两个子进程，不过好像这样就不能传数据了。

由于符号 '>' 右侧一定是一个文件，所以可以先把文件名提取出来。

实现完基本的重定向之后，我们可以实现复杂的重定向了：

```
> with <
```





要实现管道的话，这会增大复杂度。我们先举几个例子：

```bash
cat /etc/passwd | wc -l
# 输出 3
cat /etc/passwd | wc -l > test
# 无输出
```

我们可以先认为最复杂的情况就是上面的第二种。

那么首先拆 '>' 再拆 '|'。

也就是说，先判断重定向，再判断管道。

在这里用到的是 pipe

```c
int pipe(int fd[2]);
```

```
fd[0] 管道的读端
fd[1] 管道的写端
```

对于例子

```bash
cat /etc/passwd | wc -l
```

'|' 左侧的结果写入管道，右侧从管道中读取数据。

