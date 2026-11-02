#ifndef VENDA_H
#define VENDA_H

#include <stdbool.h>
#include <time.h>

typedef enum {
    PAG_A_VISTA       = 1,
    PAG_FINANCIAMENTO = 2,
    PAG_CARTAO        = 3,
    PAG_TROCA         = 4   /* carro usado na troca + diferenca */
} FormaPagamento;

/*
 * TAD Venda e TAD ListaVendas (tipos opacos).
 * Cada venda guarda o preco de tabela e o custo do veiculo NO MOMENTO da venda,
 * entao o lucro historico nao muda se os precos forem alterados depois.
 */
typedef struct venda Venda;
typedef struct lista_vendas ListaVendas;

/* ---------- TAD Venda ---------- */

/* NULL se valor_final <= 0, forma invalida ou entrada fora de [0, valor_final] */
Venda *venda_criar(int id, int cliente_id, int veiculo_id, double preco_tabela,
                   double custo, double valor_final, FormaPagamento forma,
                   double entrada);
void   venda_destruir(Venda *v);
void   venda_finalizar(Venda *v);   /* grava data/hora atual */

int            venda_id(const Venda *v);
int            venda_cliente_id(const Venda *v);
int            venda_veiculo_id(const Venda *v);
time_t         venda_data(const Venda *v);
double         venda_preco_tabela(const Venda *v);
double         venda_custo(const Venda *v);
double         venda_valor_final(const Venda *v);
double         venda_entrada(const Venda *v);
FormaPagamento venda_forma(const Venda *v);
double         venda_desconto(const Venda *v);  /* tabela - valor final */
double         venda_lucro(const Venda *v);     /* valor final - custo */

const char *forma_pagamento_nome(FormaPagamento f);

/* ---------- TAD ListaVendas ---------- */
ListaVendas *lista_vendas_criar(void);
void         lista_vendas_destruir(ListaVendas *l);

int  lista_vendas_proximo_id(const ListaVendas *l);
/* A lista assume a posse da venda (nao chame venda_destruir depois). */
bool lista_vendas_registrar(ListaVendas *l, Venda *v);

int          lista_vendas_tamanho(const ListaVendas *l);
const Venda *lista_vendas_obter(const ListaVendas *l, int indice);
double       lista_vendas_faturamento(const ListaVendas *l);
double       lista_vendas_lucro_total(const ListaVendas *l);
double       lista_vendas_descontos_total(const ListaVendas *l);

bool lista_vendas_salvar(const ListaVendas *l, const char *arquivo);
bool lista_vendas_carregar(ListaVendas *l, const char *arquivo);

#endif
