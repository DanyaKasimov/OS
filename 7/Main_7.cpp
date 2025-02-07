#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdlib.h>

void print_time() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm *tm_info = localtime(&ts.tv_sec);
    printf("%02d:%02d:%02d:%03ld", tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec, ts.tv_nsec / 1000000);
}

int main() {
    pid_t pid1, pid2;

    pid1 = fork();

    if (pid1 == 0) {
        printf("Дочерний процесс 1: PID=%d, PPID=%d, Время: ", getpid(), getppid());
        print_time();
        printf("\n");
        exit(0);
    } else if (pid1 > 0) {
        pid2 = fork();

        if (pid2 == 0) {
            printf("Дочерний процесс 2: PID=%d, PPID=%d, Время: ", getpid(), getppid());
            print_time();
            printf("\n");
            exit(0);
        } else if (pid2 > 0) {
            wait(NULL);
            wait(NULL);

            printf("Родительский процесс: PID=%d, Время: ", getpid());
            print_time();
            printf("\n");

            system("ps -x");
        } else {
            perror("Ошибка при втором вызове fork()");
            exit(1);
        }
    } else {
        perror("Ошибка при первом вызове fork()");
        exit(1);
    }

    return 0;
}