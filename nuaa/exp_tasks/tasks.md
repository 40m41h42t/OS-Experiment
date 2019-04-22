# 作业 1

编译指令：

```bash
gcc -o task1 task1.c
```

用法：

```bash
# 在非root时
sudo ./task1 <num>
```

主要就是父进程与子进程以及共享内存使用的问题。

# 作业 2

```bash
gcc -o task2 task2.c
```

用法：

```bash
./task2 <maxn>
# 计时
time ./task2 <maxn>
```

思路是先筛法查表再试除。筛法以空间换时间，试除的话就是单纯的慢了。

其实我有一种思路是维护一个子进程队列，在子进程中用筛法的，不过这几天打比赛实在是忙的不得了，所以只开了两个线程还没有用筛法。

之前尝试过用不确定性的素性测试（比如 Miller-Rabin 算法）来验证素数，然后写完脚本验证时发现未有化时实在是慢的可以。优化的话在用快速傅里叶变换理论上可以将时间推进到 $O(k\ log^2n)$ 的时间复杂度[^1]不过还是明显没有筛法快。也许这种算法只适用于对单个大素数的验证吧。但是根据另一个文献[^2]，只需要很少的底数就可以验证很大的素数。我准备找个时间具体地整理一下。

# 作业 3

理解优先。只写了伪代码

```
//写者优先 
int wcount = 0;  //用于记录写者数量 
int rcount = 0;   //用于记录读者数量
semaphore rmutex = 1;   //用于读者进程互斥修改rcount
semaphore wmutex = 1;   //用于写者进程互斥修改wcount
semaphore file = 1;    //用于读者写者互斥访问file 
semphore  read = 1;     //用于阻塞读者进程，实现写者优先 

writer()  // 写者
{
    P(wmutex);   // 写者进程互斥锁
    if(wcount == 0)
        P(read);
    wcount++;
    V(wmutex);   // 释放
   
    P(file);     //写者互斥访问文件 
    do_writing();
    V(file);
   
    P(wmutex)
    wcount--;
    if(wcount == 0)
        V(read);
    V(wmutex);
} 

reader()
{
    P(read);     //检查写者队列是否为空。 
    P(rmutex);
    if(rcount == 0)
       P(file);   //申请文件资源 
    rcount++;
    V(rmutex);
    V(read); 
   
    do_reading();
   
    P(rmutex);
    rcount--;
    if(rcount == 0)
        V(file);
    V(rmutex);
}
```



# 参考文章

[^1]: [米勒-拉宾素性检验](https://zh.wikipedia.org/wiki/%E7%B1%B3%E5%8B%92-%E6%8B%89%E5%AE%BE%E6%A3%80%E9%AA%8C)
[^2]: [64位以内Rabin-Miller强伪素数测试和Pollard *r*因数分解算法的实现](<https://wenku.baidu.com/view/7433bcd9ad51f01dc281f153.html>)



环境用的是 VMware + Ubuntu 18.04 / WSL。