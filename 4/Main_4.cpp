#include <stdio.h>
#include <sys/stat.h>

void copy_file(const char *source, const char *target) {
    FILE *src_file = fopen(source, "r");
    if (src_file == NULL) {
        perror("Error opening source file");
        return;
    }

    FILE *dest_file = fopen(target, "w");
    if (dest_file == NULL) {
        perror("Error opening destination file");
        fclose(src_file);
        return;
    }

    int ch;
    while ((ch = fgetc(src_file)) != EOF) {
        if (ferror(src_file)) {
            perror("Error reading from source file");
            fclose(src_file);
            fclose(dest_file);
            return;
        }

        if (fputc(ch, dest_file) == EOF) {
            perror("Error writing to destination file");
            fclose(src_file);
            fclose(dest_file);
            return;
        }
    }

    if (fclose(src_file) != 0) {
        perror("Error closing source file");
        fclose(dest_file);
        return;
    }

    if (fclose(dest_file) != 0) {
        perror("Error closing destination file");
        return;
    }

    struct stat file_stat;
    if (stat(source, &file_stat) == 0) {
        if (chmod(target, file_stat.st_mode) != 0) {
            perror("Error setting permissions on destination file");
        }
    } else {
        perror("Error getting source file status");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Not enough arguments\n");
        return 1;
    }

    copy_file(argv[1], argv[2]);

    printf("Copying was successful.");
    return 0;
}