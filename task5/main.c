#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    pid_t pid;
    // OPEN FILES
    int fd;
    fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC);
    if (fd == -1) {
        /* code */
        fprintf(stderr, "fail on open %s\n", "test.txt");
        return -1;
    }
    //write 'hello fcntl!' to file
    /* code */
    char input[13] = "hello fcntl!";
    int in = write(fd, input, 12);
    if (in == -1) {
        fprintf(stderr, "fail on write in 'hello fcntl!'\n");
        return -1;
    }
    // DUPLICATE FD

    /* code */
    int fd_son = fcntl(fd, F_DUPFD, 0);
    if (fd_son == -1) {
        fprintf(stderr, "fail on fcntl copy the file description.\n");
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        // FAILS
        printf("error in fork");
        return 1;
    }

    struct flock fl;

    if (pid > 0) {
        // PARENT PROCESS
        //set the lock
        memset(&fl, 0, sizeof(fl));
        fl.l_type = F_WRLCK;
        if (fcntl(fd, F_SETLKW, &fl) < 0) {
            fprintf(stderr, "fail on lock the file.\n");
            return 2;
        }
        //append 'b'
        write(fd, "b", 1);
        //unlock
        fl.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLKW, &fl) < 0) {
            fprintf(stderr, "fail on unlock the file.\n");
            return 2;
        }
        sleep(3);
        // printf("%s", str); the feedback should be 'hello fcntl!ba'
        char str[20];
        memset(str, 0, sizeof(str));
        lseek(fd,0,SEEK_SET);
        if (read(fd, str, 20) < 0) {
            fprintf(stderr, "fail on read the file.\n");
            return 2;
        }
        printf("%s\n", str);

    } else {
        // CHILD PROCESS
        sleep(2);
        //get the lock
        if (fcntl(fd_son, F_GETLK, &fl) < 0) {
            fprintf(stderr, "fail on lock the file.\n");
            return 3;
        }
        //append 'a'
        if(fl.l_type==F_UNLCK){
            if (write(fd_son, "a", 1) < 0) {
                perror("fail on write to file.\n");
                return 3;
            }
        }else{
            fprintf(stderr,"fail on read the file, already locked.\n");
            return 3;
        }
        return 0;
    }
    close(fd);
    return 0;
}