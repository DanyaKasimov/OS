#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 1024

typedef struct {
    char name[256];
    off_t size;
    time_t creation_time;
    mode_t permissions;
    ino_t inode;
} FileInfo;

int compareFiles(const char *file1, const char *file2) {
    const int fd1 = open(file1, O_RDONLY);
    if (fd1 == -1) {
        perror("Error opening file1");
        return -1;
    }

    const int fd2 = open(file2, O_RDONLY);
    if (fd2 == -1) {
        perror("Error opening file2");
        close(fd1);
        return -1;
    }

    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    ssize_t bytesRead1;
    ssize_t bytesRead2;

    while ((bytesRead1 = read(fd1, buffer1, BUFFER_SIZE)) > 0 &&
           (bytesRead2 = read(fd2, buffer2, BUFFER_SIZE)) > 0) {
        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {
            close(fd1);
            close(fd2);
            return 0;
        }
    }

    if (bytesRead1 == -1 || bytesRead2 == -1) {
        perror("Error reading file");
        close(fd1);
        close(fd2);
        return -1;
    }

    close(fd1);
    close(fd2);
    return 1;
}

void getFileInfo(const char *path, FileInfo *info) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Error getting file info");
        return;
    }

    strncpy(info->name, path, sizeof(info->name));
    info->size = st.st_size;
    info->creation_time = st.st_ctime;
    info->permissions = st.st_mode;
    info->inode = st.st_ino;
}

void traverseDirectory(const char *dirPath, FileInfo *files, int *fileCount) {
    DIR *dir = opendir(dirPath);
    if (!dir) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char fullPath[512];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        struct stat st;
        if (stat(fullPath, &st) == -1) {
            perror("Error getting file info");
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            traverseDirectory(fullPath, files, fileCount);
        } else if (S_ISREG(st.st_mode)) {
            getFileInfo(fullPath, &files[*fileCount]);
            (*fileCount)++;
        }
    }

    if (closedir(dir) == -1) {
        perror("Error closing directory");
    }
}

void printFileInfo(const FileInfo *info, FILE *output) {
    fprintf(output, "Name: %s\n", info->name);
    fprintf(output, "Size: %ld bytes\n", (long)info->size);
    fprintf(output, "Creation Time: %s", ctime(&info->creation_time));
    fprintf(output, "Permissions: %o\n", info->permissions & 0777);
    fprintf(output, "Inode: %lu\n", (unsigned long)info->inode);
    fprintf(output, "-------------------------\n");
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <directory1> <directory2> <output_file>\n", argv[0]);
        return 1;
    }

    const char *dir1 = argv[1];
    const char *dir2 = argv[2];
    const char *outputFile = argv[3];

    FileInfo files1[1000];
    int fileCount1 = 0;
    traverseDirectory(dir1, files1, &fileCount1);

    FileInfo files2[1000];
    int fileCount2 = 0;
    traverseDirectory(dir2, files2, &fileCount2);

    FILE *output = fopen(outputFile, "w");
    if (!output) {
        perror("Error opening output file");
        return 1;
    }

    for (int i = 0; i < fileCount1; i++) {
        for (int j = 0; j < fileCount2; j++) {
            if (files1[i].size == files2[j].size && compareFiles(files1[i].name, files2[j].name) == 1) {
                printf("Matched file:\n");
                printFileInfo(&files1[i], stdout);
                printFileInfo(&files2[j], stdout);

                fprintf(output, "Matched file:\n");
                printFileInfo(&files1[i], output);
                printFileInfo(&files2[j], output);
            }
        }
    }

    if (fclose(output) == EOF) {
        perror("Error closing output file");
        return 1;
    }

    return 0;
}