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

### 11.8 感觉有问题,可能是错的。

```c
void ch_signal(int sig){
    pid_t pid;
    //子进程中都还没有终止，就立即返回0.否则返回终止子进程号，这样的话，在等待子进程的时候，我们还是可以做自己的事情的。
    while( (pid = waitpid(-1, NULL, WHOHANG)) > 0 )
        ;
}
//main函数中，加一个
signal(SIGCHILD, ch_signal);
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

