#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Funcoes de entrada segura pelo teclado */
void   ler_texto(const char *prompt, char *dest, size_t tam);
int    ler_int(const char *prompt);
double ler_double(const char *prompt);
void   pausar(void);

#endif
