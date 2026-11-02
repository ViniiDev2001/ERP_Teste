#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "colecao.h"

bool colecao_adicionar(Colecao *c, void *item, size_t inicial) {
    if (!c || !item) return false;
    if (c->tamanho == c->capacidade) {
        size_t nova = c->capacidade ? c->capacidade * 2 : inicial;
        if (nova < c->capacidade || nova > SIZE_MAX / sizeof *c->itens) return false;
        void **itens = realloc(c->itens, nova * sizeof *itens);
        if (!itens) return false;
        c->itens = itens;
        c->capacidade = nova;
    }
    c->itens[c->tamanho++] = item;
    return true;
}

void colecao_limpar(Colecao *c) {
    if (!c) return;
    for (size_t i = 0; i < c->tamanho; i++) free(c->itens[i]);
    free(c->itens);
    c->itens = NULL;
    c->tamanho = c->capacidade = 0;
}

bool colecao_salvar(const Colecao *c, const char *arquivo, size_t tamanho_item) {
    if (!c || !arquivo || !tamanho_item || c->tamanho > INT32_MAX) return false;
    FILE *f = fopen(arquivo, "wb");
    if (!f) return false;
    int n = (int)c->tamanho;
    bool ok = fwrite(&n, sizeof n, 1, f) == 1;
    for (int i = 0; ok && i < n; i++) ok = fwrite(c->itens[i], tamanho_item, 1, f) == 1;
    if (fclose(f) != 0) ok = false;
    return ok;
}

bool colecao_carregar(Colecao *c, const char *arquivo, size_t tamanho_item,
                      size_t inicial) {
    if (!c || !arquivo || !tamanho_item) return false;
    FILE *f = fopen(arquivo, "rb");
    if (!f) return true;
    int n = 0;
    bool ok = fread(&n, sizeof n, 1, f) == 1 && n >= 0;
    for (int i = 0; ok && i < n; i++) {
        void *item = malloc(tamanho_item);
        if (!item) { ok = false; break; }
        if (fread(item, tamanho_item, 1, f) != 1 || !colecao_adicionar(c, item, inicial)) {
            free(item);
            ok = false;
        }
    }
    if (fclose(f) != 0) ok = false;
    return ok;
}
