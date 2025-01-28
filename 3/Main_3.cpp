#include <stdlib.h>
   #include <stdio.h>

void print_lines(FILE *file, int n) {
    int line_count = 0;
    int ch;

    while ((ch = fgetc(file)) != EOF) {
        if (ferror(file)) {
            perror("Error reading file");
            return;
        }

        putc(ch, stdout);
        if (n != 0) {
            if (ch == '\n') {
                line_count++;
            }

            if (line_count >= n) {
                printf("Press any key to continue: ");
                getc(stdin);
                while (getc(stdin) != '\n');
                line_count = 0;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Not enough arguments");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    int n = atoi(argv[2]);
    if (n < 0) {
        printf("The number of lines must be a positive integer.\n");
        fclose(file);
        return 1;
    }

    print_lines(file, n);

    if (fclose(file) != 0) {
        perror("Error closing file");
        return 1;
    }

    return 0;
}