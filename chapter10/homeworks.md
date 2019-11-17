## 家庭作业解答

### 10.6 下面程序的输出

```c
#include "csapp.h"

int main() {
    int fd1, fd2;
    fd1 = Open("foo.txt", O_RDONLY, 0);
    fd2 = Open("bar.txt", O_RDONLY, 0);
    Close(fd2);
    fd2 = Open("baz.txt", O_RDONLY, 0);
    printf("fd2 = %d\n", fd2);
    exit(0);
}
```

fd1打开时为3，fd2第一次打开时为4，但是close之后描述符表就去掉了4，再打开fd2还是4

输出为 fd2 = 4

### 10.7 修改10.5的cpfile程序，使之可以标准输入复制到标准输出。一次max-buf字节

```c
#include "csapp.h"

int main(int argc, char **argv) 
{
    int n;
    rio_t rio;
    char buf[MAXBUF];

    Rio_readinitb(&rio, STDIN_FILENO);
    while((n = Rio_readb(&rio, buf, MAXBUF)) != 0) 
	Rio_writen(STDOUT_FILENO, buf, MAXBUF);
    exit(0);
}
```



### 10.8 

```c
#include "csapp.h"

int main (int argc, char **argv) 
{
    struct stat stat;
    char *type, *readok;

    /* $end statcheck */
    if (argc != 2) {
	fprintf(stderr, "usage: %s <filename>\n", argv[0]);
	exit(0);
    }
    /* $begin statcheck */
    fstat(argv[1], &stat);
    //Stat(argv[1], &stat);
    if (S_ISREG(stat.st_mode))     /* Determine file type */
	type = "regular";
    else if (S_ISDIR(stat.st_mode))
	type = "directory";
    else 
	type = "other";
    if ((stat.st_mode & S_IRUSR)) /* Check read access */
	readok = "yes";
    else
	readok = "no";

    printf("type: %s, read: %s\n", type, readok);
    exit(0);
}
```



