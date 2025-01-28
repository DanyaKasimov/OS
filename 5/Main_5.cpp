#include <stdio.h>
#include <dirent.h>
#include <string.h>

void list_directory(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening dir.");
        return;
    }

    struct dirent *entry;
    printf("Contents of the catalog '%s':\n", path);
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
        }
    }

    if (closedir(dir) != 0) {
        perror("Error closing dir.");
    }
}

int main(int argc, char *argv[]) {
    list_directory(".");

    if (argc < 2) {
        printf("Not enough arguments.\n");
    }

    list_directory(argv[1]);

    return 0;
}