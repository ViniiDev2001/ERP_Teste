#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "venda.h"
#include "colecao.h"

struct venda {
    int    id;
    int    cliente_id;
    int    veiculo_id;
    time_t data;
    double preco_tabela;
    double custo;
    double valor_final;
    int    forma;
    double entrada;
};

struct lista_vendas {
    Colecao dados;
};

/* ---------- Venda ---------- */

Venda *venda_criar(int id, int cliente_id, int veiculo_id, double preco_tabela,
                   double custo, double valor_final, FormaPagamento forma,
                   double entrada) {
    if (valor_final <= 0) return NULL;
    if (forma < PAG_A_VISTA || forma > PAG_TROCA) return NULL;
    if (entrada < 0 || entrada > valor_final) return NULL;

    Venda *v = calloc(1, sizeof *v);
    if (!v) return NULL;
    v->id = id;
    v->cliente_id = cliente_id;
    v->veiculo_id = veiculo_id;
    v->preco_tabela = preco_tabela;
    v->custo = custo;
    v->valor_final = valor_final;
    v->forma = forma;
    v->entrada = entrada;
    return v;
}

void venda_destruir(Venda *v)  { free(v); }
void venda_finalizar(Venda *v) { v->data = time(NULL); }

int            venda_id(const Venda *v)           { return v->id; }
int            venda_cliente_id(const Venda *v)   { return v->cliente_id; }
int            venda_veiculo_id(const Venda *v)   { return v->veiculo_id; }
time_t         venda_data(const Venda *v)         { return v->data; }
double         venda_preco_tabela(const Venda *v) { return v->preco_tabela; }
double         venda_custo(const Venda *v)        { return v->custo; }
double         venda_valor_final(const Venda *v)  { return v->valor_final; }
double         venda_entrada(const Venda *v)      { return v->entrada; }
FormaPagamento venda_forma(const Venda *v)        { return (FormaPagamento)v->forma; }
double         venda_desconto(const Venda *v)     { return v->preco_tabela - v->valor_final; }
double         venda_lucro(const Venda *v)        { return v->valor_final - v->custo; }

const char *forma_pagamento_nome(FormaPagamento f) {
    switch (f) {
        case PAG_A_VISTA:       return "A vista";
        case PAG_FINANCIAMENTO: return "Financiamento";
        case PAG_CARTAO:        return "Cartao";
        case PAG_TROCA:         return "Troca + diferenca";
        default:                return "?";
    }
}

/* ---------- ListaVendas ---------- */

ListaVendas *lista_vendas_criar(void) {
    return calloc(1, sizeof(ListaVendas));
}

void lista_vendas_destruir(ListaVendas *l) {
    if (!l) return;
    colecao_limpar(&l->dados);
    free(l);
}

int lista_vendas_proximo_id(const ListaVendas *l) {
    int max = 0;
    for (size_t i = 0; i < l->dados.tamanho; i++)
        if (((Venda *)l->dados.itens[i])->id > max) max = ((Venda *)l->dados.itens[i])->id;
    return max + 1;
}

bool lista_vendas_registrar(ListaVendas *l, Venda *v) {
    return colecao_adicionar(&l->dados, v, 32);
}

int lista_vendas_tamanho(const ListaVendas *l) { return (int)l->dados.tamanho; }

const Venda *lista_vendas_obter(const ListaVendas *l, int indice) {
    if (indice < 0 || (size_t)indice >= l->dados.tamanho) return NULL;
    return l->dados.itens[indice];
}

double lista_vendas_faturamento(const ListaVendas *l) {
    double total = 0.0;
    for (size_t i = 0; i < l->dados.tamanho; i++) total += ((Venda *)l->dados.itens[i])->valor_final;
    return total;
}

double lista_vendas_lucro_total(const ListaVendas *l) {
    double total = 0.0;
    for (size_t i = 0; i < l->dados.tamanho; i++) total += venda_lucro(l->dados.itens[i]);
    return total;
}

double lista_vendas_descontos_total(const ListaVendas *l) {
    double total = 0.0;
    for (size_t i = 0; i < l->dados.tamanho; i++) total += venda_desconto(l->dados.itens[i]);
    return total;
}

/* ---------- Persistencia ---------- */

bool lista_vendas_salvar(const ListaVendas *l, const char *arquivo) {
    return colecao_salvar(&l->dados, arquivo, sizeof(Venda));
}

bool lista_vendas_carregar(ListaVendas *l, const char *arquivo) {
    return colecao_carregar(&l->dados, arquivo, sizeof(Venda), 32);
}
