#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define RESET "\033[0m"
#define BLUE  "\033[34m"
#define GREEN "\033[32m"

int is_keyword(const char *s) {
    const char *kw[] = {
        "int", "return", "if", "else", "for", "while",
        "void", "char", "float", "double"
    };

    for (int i = 0; i < 9; i++) {
        if (strcmp(s, kw[i]) == 0)
            return 1;
    }
    return 0;
}

int read_token(const char *src, int i, char *buf) {
    int j = 0;

    // 跳过空格
    while (src[i] == ' ' || src[i] == '\t')
        i++;

    // 数字
    if (isdigit(src[i])) {
        while (isdigit(src[i])) {
            buf[j++] = src[i++];
        }
        buf[j] = '\0';
        return i;
    }

    // 标识符
    if (isalpha(src[i]) || src[i] == '_') {
        while (isalnum(src[i]) || src[i] == '_') {
            buf[j++] = src[i++];
        }
        buf[j] = '\0';
        return i;
    }

    // 其他字符
    buf[0] = src[i];
    buf[1] = '\0';
    return i + 1;
}

char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("open file");
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, f);
    buffer[size] = '\0';

    fclose(f);
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s file.c\n", argv[0]);
        return 1;
    }

    char *code = read_file(argv[1]);

    int i = 0;
    char token[128];

    while (code[i]) {
        i = read_token(code, i, token);

        if (is_keyword(token)) {
            printf(BLUE "%s" RESET, token);
        }
        else if (isdigit(token[0])) {
            printf(GREEN "%s" RESET, token);
        }
        else {
            printf("%s", token);
        }

        printf(" ");
    }

    free(code);
    return 0;
}
