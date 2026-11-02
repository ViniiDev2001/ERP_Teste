#ifndef COLECAO_H
#define COLECAO_H

#include <stdbool.h>
#include <stddef.h>

/* Colecao dinamica com propriedade dos itens e persistencia de registros. */
typedef struct {
    void **itens;
    size_t tamanho;
    size_t capacidade;
} Colecao;

bool colecao_adicionar(Colecao *colecao, void *item, size_t capacidade_inicial);
void colecao_limpar(Colecao *colecao);
bool colecao_salvar(const Colecao *colecao, const char *arquivo, size_t tamanho_item);
bool colecao_carregar(Colecao *colecao, const char *arquivo, size_t tamanho_item,
                      size_t capacidade_inicial);

#endif
