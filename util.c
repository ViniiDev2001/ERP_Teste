#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void ler_texto(const char *prompt, char *dest, size_t tam) {
    printf("%s", prompt);
    if (fgets(dest, (int)tam, stdin) == NULL) {
        /* EOF (Ctrl+D ou entrada redirecionada acabou): encerra sem travar.
         * Os dados ja foram gravados a cada alteracao. */
        printf("\n[fim da entrada - encerrando]\n");
        exit(0);
    }
    size_t len = strlen(dest);
    if (len > 0 && dest[len - 1] == '\n') {
        dest[len - 1] = '\0';
    } else {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }  /* descarta excesso */
    }
}

int ler_int(const char *prompt) {
    char buf[64];
    char *fim;
    for (;;) {
        ler_texto(prompt, buf, sizeof buf);
        long v = strtol(buf, &fim, 10);
        if (fim != buf && *fim == '\0') return (int)v;
        printf("Valor invalido. Digite um numero inteiro.\n");
    }
}

double ler_double(const char *prompt) {
    char buf[64];
    char *fim;
    for (;;) {
        ler_texto(prompt, buf, sizeof buf);
        for (char *p = buf; *p; p++) if (*p == ',') *p = '.';
        double v = strtod(buf, &fim);
        if (fim != buf && *fim == '\0') return v;
        printf("Valor invalido. Digite um numero (ex: 12.50).\n");
    }
}

void pausar(void) {
    char buf[8];
    ler_texto("\nENTER para continuar...", buf, sizeof buf);
}
