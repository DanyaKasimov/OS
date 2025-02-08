#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_BYTES 255

void search_in_file(const char *filename, const char *pattern, int pattern_length, int count) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Ошибка открытия файла");
        return;
    }

    char buffer[MAX_BYTES];
    ssize_t bytes_read;
    off_t total_bytes = 0;
    int matches = 0;

    while ((bytes_read = read(fd, buffer, MAX_BYTES)) > 0) {
        total_bytes += bytes_read;
        for (int i = 0; i <= bytes_read - pattern_length; i++) {
            if (memcmp(buffer + i, pattern, pattern_length) == 0) {
                matches++;
            }
        }
    }

    close(fd);

    printf("PID: %d, Файл: %s, Просмотрено байт: %ld, Найдено совпадений: %d, Номер процесса: %d\n", getpid(), filename, total_bytes, matches, count);
}

int main() {
    char dirname[256];
    char pattern[MAX_BYTES];
    int pattern_length;
    int max_processes;

    printf("Введите имя каталога: ");
    scanf("%s", dirname);
    printf("Введите комбинацию байт для поиска: ");
    scanf("%s", pattern);
    pattern_length = strlen(pattern);
    printf("Введите максимальное количество одновременно работающих процессов: ");
    scanf("%d", &max_processes);

    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        perror("Ошибка открытия каталога");
        return 1;
    }

    struct dirent *entry;
    int active_processes = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", dirname, entry->d_name);

            if (active_processes >= max_processes) {
                wait(NULL);
                active_processes--;
            }

            pid_t pid = fork();
            if (pid == 0) {
                search_in_file(filepath, pattern, pattern_length, active_processes);
                exit(0);
            } else if (pid > 0) {
                active_processes++;
            } else {
                perror("Ошибка при вызове fork()");
                return 1;
            }
        }
    }

    while (active_processes > 0) {
        wait(NULL);
        active_processes--;
    }

    closedir(dir);
    return 0;
}