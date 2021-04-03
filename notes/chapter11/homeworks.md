## 家庭作业

### 11.6

A 在doit里面添加一个，注意要在sscanf之后

​	printf("%s %s %s\n", method, uri, version); //作业11-6A

B 。。。 

C 。。。

D 。。。

### 11.7

我们可以get_filetype加入一个mp4->viedo/mpge4，但是返回的直接是文件，还没有名字。

```C
else if (strstr(filename, ".mp4"))
        strcpy(filetype, "video/mpeg4");
```

### 11.8 你需要搞清楚，父进程如何回收子进程的，防止zombie产生

```c
void ch_signal(int sig){
    int old_errno = errno;//保存和恢复errno  见csapp P536
    int status;
    pid_t pid;
    //子进程中都还没有终止，就立即返回0.否则返回终止子进程号，这样的话，在等待子进程的时候，我们还是可以做自己的事情的。
    //为什么要用while循环呢，我们知道子进程发送给父进程的信号，没有缓冲，会丢失的，所以我们while是当，还有其他子进程结束的话，一起收回他。
    //为什么要用WHOHANG(非阻塞)呢，如果没有别的子进程终止了，那么就可以退出信号处理函数了。还保证在最后一个子进程回收时，没有别的子进程了，可以退出信号处理函数。回到父进程。
    while( (pid = waitpid(-1, &status, WNOHANG)) > 0 )
        ;
    errno = old_errno;
}
//main函数中，加一个
Signal(SIGCHLD, ch_signal);

//删除
//Wait(NULL);
```

### 11.9 再打开文件之后，拿到文件描述符srcfd。

```c
srcp = (char *) malloc(filesize * sizeof(char));
Rio_t rio;
Rio_readinitb(&rio, srcfd);
Rio_readnb(&rio, srcp, filesize);
//上面三步步骤可以用Rio_readn(srcfd, srcp, filesize)无缓冲函数; 直接代替.题目要求这种。
Rio_writen(fd, srcp, filesize);
free(srcp);
```

### 11.11 在两个server函数上面加上一个method.

```c
void serve_static(int fd, char *filename, int filesize, char *method);
void serve_dynamic(int fd, char *filename, char *cgiargs, char *method);
//然后再各自的函数中处理一下返回。
```

