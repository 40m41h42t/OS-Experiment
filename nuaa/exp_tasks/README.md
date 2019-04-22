操作系统老师留了一个有趣的作业：

> 用pthread编写一个多线程程序来输出素数。程序应该这样工作：用户输入运行程序时在命令行输入一个数字，然后输出小于或等于用户输入数的所有素数。请大家思考一下如何利用多线程提高计算速度，希望越快越好（注：不可以在程序中预存结果，最后会公布各位同学程序运行速度的排名）。

于是我就整理了一下相关的素数测试方法，也正好复习一下。

<!--more-->

# Rabin-Miller 素性测试

根据《64位以内Rabin-Miller强伪素数测试和Pollard *r*因数分解算法的实现》[^4]中 Riesel 给出的取前 *k* 个素数为基，并用 $\psi_k​$ 来表示以前 *k* 个素数为基的强伪素数的表格，
$$
\begin{align}
&\psi_1=2047 \\
&\psi_2=1373653\\
&\psi_3=25326001\\
&\psi_4=3215031751\\
&\psi_5=2152302898747\\
&\psi_6=3474749660383
\end{align}
$$

我们发现，考虑到 32 位二进制数能表示的范围（0~4 294 967 295(0xFFFFFFFF)），只需取前 6 个素数为基，则对于小于 $\psi_5​$ 的所有大于 1 的整数测试都是正确的，测试样例可以非常的少。而如果我们继续查询资料的话，在《数论部分第一节：素数与素性测试| Matrix67: The Aha Moments》[^3]中我们会了解到如下的知识：

> 对于大数的素性判断，目前 Miller-Rabin 算法应用最广泛。一般底数仍然是随机选取，但当待测数不太大时，选择测试底数就有一些技巧了。比如，如果被测数小于 4 759 123 141，那么只需要测试三个底数 2, 7 和 61 就足够了。当然，你测试的越多，正确的范围肯定也越大。如果你每次都用前 7 个素数 (2, 3, 5, 7, 11, 13 和 17) 进行测试，所有不超过 341 550 071 728 320 的数都是正确的。如果选用 2, 3, 7, 61 和 24251 作为底数，那么 10^16 内唯一的强伪素数为 46 856 248 255 981。这样的一些结论使得 Miller-Rabin 算法在 OI 中非常实用。通常认为，Miller-Rabin 素性测试的正确率可以令人接受，随机选取k个底数进行测试算法的失误率大概为 $4^{-k}$。

也就是说，我们只需要选取 2、7、61 就足够 32 位二进制数的检测了。这又将时间复杂度缩减了一大半。当然啦，对于为什么选取这几个底数就可以，我们可能需要另外一些证明。如果有时间的话我就整理总结相关的文章吧。

形式化的 Miller-Rabin 算法描述如下：

设我们要验证的数是 n。

- 记 $n-1=2^sd$，其中 $d$ 是奇数而 $s$ 非负。
- 如果 $a^d \equiv1(mod \ n)$，或对某个 $0\leq r<s$ 有 $a^{2^rd} \equiv -1(mod \ n)$，则认为 n 通过测试，并称之为一个以 $a$ 为基的**强可能参数**

在具体的算法描述中，对于 $n>1$，如果 $a^{n-1}\equiv 1(mod\ n)$ 则认为 $n​$ 通过测试。

则伪代码可以表示如下：

```
func MillarRabin(n, a):
    if n<2:
    	return false;
    if n==2:
    	return true;
    if(!(n&0x1)):
    	return false;
    for(d=n-1;!(d&0x1);d>>=1,++s);
    mod=getmod(a,d,n);
    if(mod==1):
    	for(i=0;i<s&&(mod!=1||mod!=(n-1));++i):
    		mod=(mod*mod)%n;
    	if(i>=s):
    		return false;
    else:
    	return false;
    return true;
```

C 语言代码表示如下：

```c
BOOL Miller_Rabin(int n, int a)
{
    int d, s, i;
    int mod;
    if (n < 2)
        return FALSE;
    if (n == 2)
        return TRUE;
    if (!(n & 0x1))
        return FALSE;
    s = 0;
    for (d = n - 1; !(d & 0x1); d >>= 1, ++s)
        ;
    mod = getmod(a, d, n);
    if (mod != 1)
    {
        for (i = 0; i < s && mod != n - 1; ++i)
            mod = (mod * mod) % n;
        if (i >= s)
            return FALSE;
    }
    return TRUE;
}
```

优化部分如下：

- 求 $a^{d} \% n$ 可以用快速幂求模。
- 所有的函数都可以写成宏。

尽管如此，米勒-罗宾素性测试的速度还是很慢。

```
real    0m6.423s
user    0m6.375s
sys     0m0.016s
```

# 素数筛法

如果我们用试除法，只需要筛选出 0~

# 参考资料

[^1]: [[数论]Miller-Rabin素性测试](<http://blog.miskcoo.com/2014/07/miller-rabin-primality-test>)
[^2]: [素性测试](<https://zh.wikipedia.org/wiki/%E7%B4%A0%E6%80%A7%E6%B5%8B%E8%AF%95>)
[^3]: [数论部分第一节：素数与素性测试| Matrix67: The Aha Moments](http://www.matrix67.com/blog/archives/234)
[^4]: [64位以内Rabin-Miller强伪素数测试和Pollard *r*因数分解算法的实现]()

[^5]: [米勒-拉宾检验](https://zh.wikipedia.org/wiki/%E7%B1%B3%E5%8B%92-%E6%8B%89%E8%B3%93%E6%AA%A2%E9%A9%97)