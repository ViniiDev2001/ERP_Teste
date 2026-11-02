#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cliente.h"
#include "colecao.h"

struct cliente {
    int  id;
    char nome[60];
    char documento[20];
    char telefone[20];
    int  ativo;
};

struct lista_clientes {
    Colecao dados;
};

/* ---------- Cliente ---------- */

int         cliente_id(const Cliente *c)        { return c->id; }
const char *cliente_nome(const Cliente *c)      { return c->nome; }
const char *cliente_documento(const Cliente *c) { return c->documento; }
const char *cliente_telefone(const Cliente *c)  { return c->telefone; }
bool        cliente_ativo(const Cliente *c)     { return c->ativo != 0; }
void        cliente_desativar(Cliente *c)       { c->ativo = 0; }

/* ---------- ListaClientes ---------- */

ListaClientes *lista_clientes_criar(void) {
    return calloc(1, sizeof(ListaClientes));
}

void lista_clientes_destruir(ListaClientes *l) {
    if (!l) return;
    colecao_limpar(&l->dados);
    free(l);
}

static int proximo_id(const ListaClientes *l) {
    int max = 0;
    for (size_t i = 0; i < l->dados.tamanho; i++)
        if (((Cliente *)l->dados.itens[i])->id > max) max = ((Cliente *)l->dados.itens[i])->id;
    return max + 1;
}

Cliente *lista_clientes_cadastrar(ListaClientes *l, const char *nome,
                                  const char *documento, const char *telefone) {
    Cliente *c = calloc(1, sizeof *c);
    if (!c) return NULL;
    c->id = proximo_id(l);
    snprintf(c->nome, sizeof c->nome, "%s", nome);
    snprintf(c->documento, sizeof c->documento, "%s", documento);
    snprintf(c->telefone, sizeof c->telefone, "%s", telefone);
    c->ativo = 1;
    if (!colecao_adicionar(&l->dados, c, 16)) {
        free(c);
        return NULL;
    }
    return c;
}

Cliente *lista_clientes_buscar(const ListaClientes *l, int id) {
    for (size_t i = 0; i < l->dados.tamanho; i++) { Cliente *c = l->dados.itens[i];
        if (c->id == id && c->ativo) return c; }
    return NULL;
}

const char *lista_clientes_nome_por_id(const ListaClientes *l, int id) {
    for (size_t i = 0; i < l->dados.tamanho; i++) { Cliente *c = l->dados.itens[i];
        if (c->id == id) return c->nome; }
    return "?";
}

int lista_clientes_tamanho(const ListaClientes *l) { return (int)l->dados.tamanho; }

Cliente *lista_clientes_obter(const ListaClientes *l, int indice) {
    if (indice < 0 || (size_t)indice >= l->dados.tamanho) return NULL;
    return l->dados.itens[indice];
}

int lista_clientes_total_ativos(const ListaClientes *l) {
    int total = 0;
    for (size_t i = 0; i < l->dados.tamanho; i++) if (((Cliente *)l->dados.itens[i])->ativo) total++;
    return total;
}

/* ---------- Persistencia ---------- */

bool lista_clientes_salvar(const ListaClientes *l, const char *arquivo) {
    return colecao_salvar(&l->dados, arquivo, sizeof(Cliente));
}

bool lista_clientes_carregar(ListaClientes *l, const char *arquivo) {
    return colecao_carregar(&l->dados, arquivo, sizeof(Cliente), 16);
}
