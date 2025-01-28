#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Not enough arguments\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "w");
    if (file == nullptr) {
        perror("Error opening file");
        return 1;
    }

    printf("Enter text (Ctrl-F to quit):\n");
    int ch;
    while ((ch = getc(stdin)) != 0x06) {
        if (fputc(ch, file) == EOF) {
            perror("Error writing to file");
            fclose(file);
            return 1;
        }
    }

    if (fclose(file) != 0) {
        perror("Error closing file");
        return 1;
    }

    printf("Text successfully saved to %s\n", argv[1]);
    return 0;
}