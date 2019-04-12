# NUAA OS-Experiment

update 190412

### 通知

1. **本周六(4月6日)，清明节机房不开放，停一次课**
2. 4专业的同学已经可以注册了，请注册后，在本站提供的虚拟机中完成布置的作业
3. 1、2、3专业的同学尽量在A区上机，4专业的同学在B区上机

### 预习

1. 连续2周停课，在停课的这段时间，请大家预习如下内容
2. 阅读完教程"多进程"和"文件描述符"
3. 完成题目"sh1.c"和"sh2.c"
4. 有余力的同学，尝试完成"sh3.c"

### 作业提交

1. 4月13日前，需要提交完成的作业: myecho.c mycat.c mycp.c mysys.c sh1.c

2. 将作业保存在目录/home/guest/jobs下，最终文件位置如下：

   ```
   home
      guest
         jobs
            mycat.c
            mycp.c
            myecho.c
            mysys.c
            sh1.c
   ```

3. 在命令行中运行程序check，该程序检查以上的c文件是否位于jobs目录，并编译生成可执行程序，如果成功，输出如下

   ```
   guest@box:~/jobs$ check
   + Check /home/guest/jobs/myecho.c
   - Check file 'myecho.c' exists or not            OK
   - Compile 'myecho.c'                             OK
   
   + Check /home/guest/jobs/mycat.c
   - Check file 'mycat.c' exists or not             OK
   - Compile 'mycat.c'                              OK
   
   + Check /home/guest/jobs/mycp.c
   - Check file 'mycp.c' exists or not              OK
   - Compile 'mycp.c'                               OK
   
   + Check /home/guest/jobs/mysys.c
   - Check file 'mysys.c' exists or not             OK
   - Compile 'mysys.c'                              OK
   
   + Check /home/guest/jobs/sh1.c
   - Check file 'sh1.c' exists or not               OK
   - Compile 'sh1.c'                                OK
   ```

4. **请保证check程序的输出结果为OK**