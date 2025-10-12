// child2.c
#include <stdio.h>
#include <ctype.h>

int main() {
    int c;
    int prev = '\0';

    while ((c = getchar()) != EOF) {
        if (c == ' ' && prev == ' ') {
            continue;
        }
        putchar(c);
        prev = c;
    }
    return 0;
}