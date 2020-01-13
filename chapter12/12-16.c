/*************************************************************************
	> File Name: 12-16.c
	> Author: 
	> Mail: 
	> Created Time: 2019年11月19日 星期二 16时55分53秒
 ************************************************************************/

#include "csapp.h"

void *thread(void *vargp);

int main(int argc, char *argv[]){
    if (argc != 2) {
        exit(-1);
    }
    pthread_t tid;
    int n = atoi(argv[1]);
    for (int i=0; i<n; i++) {
        Pthread_create(&tid, NULL, thread, NULL);
    }
    Pthread_exit(NULL);
}

void *thread(void *vargp) {
    printf("Hello world\n");
    return NULL;
}
